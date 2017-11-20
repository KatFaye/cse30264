/* client.c
implementation of Client class
Author: Kat Herring
Date: 11/18/2017
*/

#include "client.h"


using namespace std;

bool end_child_thread = false;

void* Client::msgHandler(void *arg) {
	Client *c = (Client *)arg;
	while(!end_child_thread) {
		if(c->conxn.checkMsgs()) {
			Message* m = c->conxn.newestMsg();
			if(m && !m->getPrompted()) {
				string text = m->getMsg();
				cout << "========== NEW MESSAGE: ========== " << endl << text << endl;
				c->conxn.popMsg();
			}
		}
	}
	pthread_exit(NULL);
}

Client::Client(char *h, char *port, char *username) {
	conxn = TCPConnection();
	conxn.run(h, port);
	this->login(username);
};

void Client::start() {
	int ret =  pthread_create(&t_msgHandler, NULL, &msgHandler, (void *)this);
	if(ret != 0) {
		cerr << "Err: pthread_create() failed" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Welcome to the chatroom!" << endl;
	this->usage();

	cout << endl;
	cout << "> ";
	string command;
	while(cin >> command) { // loop until the input is finished
		if(command == "E") {
			cout << "Exiting..." << endl;
			logout();
			end_child_thread = 1;
			pthread_join(t_msgHandler, NULL);
			break;
		} else if(command == "B"){
			msgBroadcast();
		} else if(command == "P"){
			msgPrivate();
		} else {
			cout << "Err: Invalid Command" << endl;
		}

		cout << endl << endl;

		this->usage();

		cout << endl << endl;
		cout << "> ";
		command.clear();
	}
};

void Client::usage() {
	cout << "Menu:\n\tSend private message (P)\n\tSend broadcast message (B)\n\tExit (E)" << endl;
}

void Client::msgBroadcast() {
	Message m = Message("B", true, false);
	conxn.sendMessage(m);

	m = ackWait();

	string text;
	cout << "Enter Message: ";
	do {
		getline(cin, text);
		text = strip(text);
	} while(text.length() == 0);

	m = Message(text, false, true);
	conxn.sendMessage(m);

	m = ackWait();
	cout << m.getMsg() << endl;
}

void Client::msgPrivate() {
	Message m = Message("P", true, false);
	conxn.sendMessage(m);

	m = ackWait();

	string text = m.getMsg();
	cout << "Online users: " << text << endl;

	cout << "Input User to PM: ";
	do {
		getline(cin, text);
		text = strip(text);
	} while(text.length() == 0);

	m = Message(text, false, true);

	cout << "Input message to " << text << ": ";
	do {
		getline(cin, text);
		text = strip(text);
	} while(text.length() == 0);

	Message m2 = Message(text, false, true);

	conxn.sendMessage(m);
	conxn.sendMessage(m2);

	m = ackWait();

	cout << m.getMsg() << endl;
}

void Client::logout() {
	Message m = Message("E", true, false);
}

void Client::login(char *username) {
	if(!username) {
		cerr << "Err: Must enter username to log in!";
	}

	Message m = Message(string(username), false, true);
	this->conxn.sendMessage(m);

	m = ackWait();

	string text = m.getMsg();
	if(text == "login") {
		string password;
		cout << "Account found! Please enter your password: ";
		cin >> password;
		while(1) {
			m = Message(password, false, true);
			conxn.sendMessage(m);
			m = ackWait();
			if(m.getMsg() == "Logged in!") {
				break;
			}
			cout << "Err: Incorrect Password. Re-enter your password: ";
			cin >> password;
		}
	} else if (text == "create") {
		string password;
		cout << "Creating new account! Input new password: ";
		cin >> password;
		cout << "Password: " << password << endl;
		m = Message(password, false, true);
		conxn.sendMessage(m);
		m = ackWait();
	}
}

Message Client::ackWait() {
	while(1) {
		if(conxn.checkMsgs()) {
			Message* m = conxn.newestMsg();
			if(m && m->getPrompted()) {
				conxn.popMsg();
				return *m;
			}
		}
	}
}

string Client::strip(string str) {

	int s=str.find_first_not_of(" \t");
	int e=str.find_last_not_of(" \t");

	if (s!=-1 && e!=-1) {
		return str.substr(s, e-s+1);
	}
	return "";
}
