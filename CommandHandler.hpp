#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <string>
# include <iostream>

typedef enum CMD {
	PASS, NICK, USER, OPER, //0-3
	QUIT, JOIN, PART, TOPIC, MODE, //4-8
	NAMES, LIST, INVITE, KICK, //9-12
	PRIVMSG, WHO, WHOIS, WHOWAS, //13-16
	KILL,PING,PONG,	//17-19
	NONE //20
} e_cmd;

class CommandHandler {
	public:
		static std::string getCommand(e_cmd cmd);
		// every command functions
};

#endif
