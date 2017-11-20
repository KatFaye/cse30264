/* Message.h
definition of Message class
Author: Kat Herring
Date: 11/18/2017
*/

#ifndef _MESSAGE
#define _MESSAGE

#include <string>
#include <iostream>

using namespace std;

class Message {
	public:
		Message(string message_text, bool cmdType, bool isPrompted);
		Message(const Message& m);
		string encode();
		static Message& decode(string encoded_message);
		string getMsg();
		bool getType();
		bool getPrompted();
		void setMsg(string new_message_text);
		void setCmdType(bool isPrompted);
		void setPrompted(bool isPrompted);

	private:
		string message_text;
		bool cmdType; // command or data
		bool isPrompted; // was prompted
		const static char delim = '#';
		const static char bTrue = 'T';
		const static char bFalse = 'F';
};

#endif
