/* client.c
implementation of Server class
Author: Kat Herring
Date: 11/18/2017
*/

#include <cstdio>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unordered_map>
#include "server.h"
using namespace std;

typedef struct thread_data_t {
	Server *server;
	TCPConnection *conn;
} ThreadData;

Server::Server(char *port) {

	// Start server
	this->tcp_server = TCPServer();
	this->tcp_server.start_server(port);

	// Load user login information
	string line, username, password;
	ifstream f("passwords.txt");
	if(f.is_open()) {
		while(getline(f, line)) {
			split(line, username, password);
			this->passwords.insert(pair<string, string>(username, password));
		}
	}
}

void Server::accept_connections() {

	while(true) {
		ThreadData thread_data = ThreadData{this, &this->tcp_server.accept_connection()};
		pthread_t thread_id;
		if((pthread_create( &thread_id, NULL, &Server::handleClient, (void*) &thread_data) < 0)) {
			perror("Err: Failed to create thread");
			continue;
		}
		this->threads.push_back(thread_id);
	}
}

void *Server::handleClient(void *data) {

	// Retrieve username
	ThreadData *thread_data = (ThreadData *) data;
	Server *server = thread_data->server;
	TCPConnection *conn = thread_data->conn;
	Message m = server->ackWait(*conn);
	string username = m.getMsg();

	// Check if user has logged in before
	unordered_map<string, string>::const_iterator it = server->passwords.find(username);
	if(it == server->passwords.end()) {
		// Username not found
		m = Message("Create", false, true);
		conn->sendMessage(m);

		m = server->ackWait(*conn);
		string password = m.getMsg();

		ofstream myfile;
		myfile.open("passwords.txt", ofstream::app);
		myfile << username << " " << password << "\n";
		myfile.close();
		m = Message("Finished", false, true);
		conn->sendMessage(m);
	}
	else {
		// Username found
		Message m = Message("login", false, true);
		conn->sendMessage(m);
		m = server->ackWait(*conn);
		while(m.getMsg() != it->second) {
			conn->sendMessage(Message("Err: Login failed", false, true));
			m = server->ackWait(*conn);
		}
		m = Message("Logged in!", false, true);
		conn->sendMessage(m);
		server->connections.insert(pair<string, TCPConnection>(username, *conn));
	}

	while(true) {
		// Wait for client command
		m = server->idle(*conn);

		// Parse command
		string command = m.getMsg();
		if(command == "B") {
			conn->sendMessage(Message("Sending message...", false, true));
			m = server->ackWait(*conn);
			m.setPrompted(false);
			server->broadcast(m, username);
			conn->sendMessage(Message("Message sent successfully to all logged-in users.", false, true));
		}
		else if(command == "P") {
			string user_list = server->onlineNow();
			conn->sendMessage(Message(user_list, false, true));
			m = server->ackWait(*conn);
			string target = m.getMsg();
			m = server->ackWait(*conn);
			m.setPrompted(false);
			if(server->msgPrivate(m, username, target)) {
				conn->sendMessage(Message("Your message was succeccfully sent to " + target + ".", false, true));
			}
			else {
				conn->sendMessage(Message("Err:" + target + " is offline.", false, true));
			}
		}
		else if(command == "E") {
			conn->closeSock();
			server->connections.erase(username);
			return 0;
		}
	}

	return 0;
}

void Server::broadcast(Message m, string username) {

	for(auto it = this->connections.begin(); it != this->connections.end(); ++it) {
		if(username != it->first) {
			it->second.sendMessage(m);
		}
	}
}

bool Server::msgPrivate(Message m, string sender, string receiver) {

	m.setMsg("(" + sender + ") " + m.getMsg());
	auto it = this->connections.find(receiver);
	if(it == this->connections.end()) {
		return false;
	}
	else {
		it->second.sendMessage(m);
		return true;
	}
}

string Server::onlineNow() {

	string user_list = "Online users:\n";
	for(auto it = this->connections.begin(); it != this->connections.end(); ++it) {
		user_list += ("\t" + it->first + "\n");
	}
	return user_list;
}

// Remove whitespace from the end of a string
string Server::rstrip(string str) {
	return str.substr(0, str.find_last_not_of(" \t\n") + 1);
}

// Split a single string into two strings at the first space
void Server::split(string line, string &s1, string &s2) {

	string::size_type pos;
	pos=line.find(' ',0);
	if(pos != string::npos) {
		s1 = line.substr(0,pos);
		s2 = line.substr(pos+1);
	}
}

Message Server::ackWait(TCPConnection conn) {
	while(1) {
		if(conn.checkMsgs()) {
			Message* m = conn.newestMsg();
			if(m && m->getPrompted()) {
				conn.popMsg();
				return *m;
			}
		}
	}
}

Message Server::idle(TCPConnection conn) {
	while(1) {
		if(conn.checkMsgs()) {
			Message* m = conn.newestMsg();
			if(m && m->getType()) {
				conn.popMsg();
				return *m;
			}
		}
	}
}
