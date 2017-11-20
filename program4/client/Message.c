/* Message.c
implementation of Message class
Author: Kat Herring
Date: 11/18/2017
*/

#include "Message.h"

Message::Message(string message_text, bool cmdType, bool isPrompted) {

	this->message_text = message_text;
	this->cmdType = cmdType;
	this->isPrompted = isPrompted;

}

Message::Message(const Message& m) {

	this->message_text = string(m.message_text);
	this->cmdType = m.cmdType;
	this->isPrompted = m.isPrompted;

}

string Message::encode() {

	string encoded_string = string(this->message_text);

	// Escape the delimiter
	for(size_t i = 0; i < encoded_string.length(); i++) {
		if (encoded_string.at(i) == this->delim) {
			encoded_string.insert(i, 1, this->delim);
			i++;
		}
	}

	encoded_string += this->delim;


	// Add cmdType
	if (this->cmdType) {
		encoded_string += this->bTrue;
	}
	else {
		encoded_string += this->bFalse;
	}

	encoded_string += this->delim;


	// Add isPrompted
	if (this->isPrompted) {
		encoded_string += this->bTrue;
	}
	else {
		encoded_string += this->bFalse;
	}

	return encoded_string;

}

Message& Message::decode(string encoded_message) {

	string text = encoded_message.substr(0, encoded_message.length() - 4);
	bool cmdType = encoded_message.at(encoded_message.length() - 3) == bTrue ? true : false;
	bool isPrompted = encoded_message.at(encoded_message.length() - 1) == bTrue ? true : false;

	// Remove the extra delimiters from the string
	for(size_t i = 0; i < text.length()-1; i++) {
		if(text.at(i) == delim && text.at(i+1) == delim) {
			text.erase(i+1, 1);
		}
	}

	Message* new_message = new Message(text, cmdType, isPrompted);

	return *new_message;

}

string Message::getMsg() {
	return this->message_text;
}

bool Message::getType() {
	return this->cmdType;
}

bool Message:: getPrompted() {
	return this->isPrompted;
}

void Message::setMsg(string new_message_text) {
	this->message_text = new_message_text;
}

void Message::setCmdType(bool cmdType) {
	this->cmdType = cmdType;
}

void Message::setPrompted(bool isPrompted) {
	this->isPrompted = isPrompted;
}
