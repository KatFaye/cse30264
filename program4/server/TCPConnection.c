/* TCPConnection.c
implementation of TCPConnection class
Author: Kat Herring
Date: 11/18/2017
*/

#include "TCPConnection.h"

using namespace std;

// Convert a c string into a cpp string and return the cpp string
string TCPConnection::convertStr(char* buf) {

	string str;
	if(buf == NULL) {
		str = string("");
	}
	else {
		str = string(buf);
	}
	return str;
}

string TCPConnection::rstrip(string str) {
	return str.substr(0, str.find_last_not_of(" \t\n") + 1);
}

void TCPConnection::openSocket(int &sockfd) {

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() failed");
		exit(1);
	}

};

TCPConnection::TCPConnection() {
	this->message_queue = queue<Message>();
	this->is_connected = false;
}

TCPConnection::TCPConnection(int dataSock) {

	this->message_queue = queue<Message>();
	this->dataSock = dataSock;
	fcntl(this->dataSock, F_SETFL, O_NONBLOCK);
	this->is_connected = true;

}

void TCPConnection::connSockClient() {

	if(connect(this->dataSock, (struct sockaddr *) &this->addrClient, sizeof(this->addrClient)) < 0) {
		perror("Err: connect() failed");
		close(this->dataSock);
		exit(1);
	}
};

void TCPConnection::run(char *host, char *port) {

	// create the hostent structure
	this->hp = gethostbyname(host);
	if(!this->hp) {
		fprintf(stderr, "Err: Unknown host: %s\n", host);
		exit(1);
	}

	// create client address struct
	bzero((char	*)&this->addrClient, sizeof(struct sockaddr_in));
	this->addrClient.sin_family = AF_INET;
	bcopy(this->hp->h_addr, (char *)&this->addrClient.sin_addr, this->hp->h_length);
	this->addrClient.sin_port = htons(atoi(port));

	openSocket(dataSock);
	connSockClient();

	fcntl(this->dataSock, F_SETFL, O_NONBLOCK);

	this->is_connected = true;
}

void TCPConnection::sendMessage(Message message) {

	// Encode the object to be sent
	string encoded_message = message.encode();

	// Encode the newly encoded string to escape the message separation delimiter
	string separated_message = msgEncode(encoded_message);

	separated_message += message_delimiter;

	// Send the message across the socket
	sendMessage(separated_message);

}

void TCPConnection::sendMessage(string s) {
	int len = s.length();
	if(send(this->dataSock, s.c_str(), len, 0) == -1) {
		fprintf(stderr, "Err: send() failed");
		closeSock();
		exit(1);
	}
}


string TCPConnection::msgEncode(string message_string) {

	string encoded_string = string(message_string);

	// Escape the delimiter
	for(size_t i = 0; i < encoded_string.length(); i++) {
		if (encoded_string.at(i) == this->message_delimiter) {
			encoded_string.insert(i, 1, this->message_delimiter);
			i++;
		}
	}

	return encoded_string;

}

string TCPConnection::msgDecode(string encoded_string) {

	string text = string(encoded_string);

	// Remove the extra delimiters from the string
	for(size_t i = 0; i < text.length()-1; i++) {
		if(text.at(i) == message_delimiter && text.at(i+1) == message_delimiter) {
			text.erase(i+1, 1);
		}
	}

	return text;

}

void TCPConnection::msgsToQueue() {

	// Retrieve latest data from socket
	string incoming_messages = recvSocket();

	if(incoming_messages.length() <= 0) {
		return;
	}

	// Split messages up
	vector<string> encoded_messages;
	msgSplit(incoming_messages, encoded_messages);

	// Iterate through vector adding messages to queue
	for(auto it = encoded_messages.begin(); it != encoded_messages.end(); it++) {
		string message_text = *it;
		string decoded_message = msgDecode(message_text);
		Message message = Message::decode(decoded_message);
		this->message_queue.push(message);
	}
}

string TCPConnection::recvSocket() {

	size_t in_buffer_size = 8192;
	char in_buffer[in_buffer_size];
	bzero(in_buffer, in_buffer_size);
	if(read(this->dataSock, (void *) &in_buffer, in_buffer_size) == -1 && errno != EAGAIN) {
		perror("Err: read() failed");
		exit(1);
	}
	return rstrip(convertStr(in_buffer));

}

void TCPConnection::msgSplit(string incoming_messages, vector<string>& messages) {

	size_t i;
	size_t start_index = 0;
	for(i = 0; i < incoming_messages.length()-1; i++) {
		if (incoming_messages.at(i) == message_delimiter) {
			if (incoming_messages.at(i+1) != message_delimiter) {
				messages.push_back(incoming_messages.substr(start_index, i));
				start_index = i+1;
			}
			else {
				i++;
			}
		}
	}

	messages.push_back(incoming_messages.substr(start_index, incoming_messages.length()-1));

}

bool TCPConnection::checkMsgs() {

	// Fill queue with most recent messages
	msgsToQueue();

	return !message_queue.empty();

}

Message* TCPConnection::newestMsg() {

	return &message_queue.front();

}

void TCPConnection::popMsg() {

	msgsToQueue();

	message_queue.pop();

}

void TCPConnection::closeSock() {

		close(this->dataSock);

}
