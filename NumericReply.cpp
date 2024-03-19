#include "NumericReply.hpp"

std::string NumericReply::getNumericReplyERR(e_numeric numeric, Server &serv) {
	std::string reply;
	switch (static_cast<int>(numeric)) {
		case ERR_ALREADYREGISTRED:
			break;
		case ERR_BADCHANMASK:
			break;
		case ERR_BADCHANNELKEY:
			break;
		case ERR_BANNEDFROMCHAN:
			break;
		case ERR_CANNOTSENDTOCHAN:
			break;
		case ERR_CANTKILLSERVER:
			break;
		case ERR_CHANOPRIVSNEEDED:
			break;
		case ERR_CHANNELISFULL:
			break;
		case ERR_ERRONEUSNICKNAME:
			break;
		case ERR_INVITEONLYCHAN:
			break;
		case ERR_KEYSET:
			break;
		case ERR_NEEDMOREPARAMS:
			break;
		case ERR_NICKCOLLISION:
			break;
		case ERR_NICKNAMEINUSE:
			break;
		case ERR_NOSUCHCHANNEL:
			break;
		case ERR_NOSUCHNICK:
			break;
		case ERR_NOTONCHANNEL:
			break;
		case ERR_NOOPERHOST:
			break;
		case ERR_PASSWDMISMATCH:
			break;
		case ERR_TOOMANYTARGETS:
			break;
		case ERR_UMODEUNKNOWNFLAG:
			break;
		case ERR_USERSDONTMATCH:
			break;
		case ERR_WILDTOPLEVEL:
			break;
	}
	return reply;
}

std::string NumericReply::getNumericReplyRPL(e_numeric numeric, Server &serv) {
	std::string reply;
	switch (numeric) {
		case RPL_CHANNELMODEIS:
			break;
		case RPL_ENDOFBANLIST:
			break;
		case RPL_ENDOFNAMES:
			break;
		case RPL_INVITING:
			break;
		case RPL_LIST:
			break;
		case RPL_LISTEND:
			break;
		case RPL_LISTSTART:
			break;
		case RPL_NAMREPLY:
			break;
		case RPL_NOTOPIC:
			break;
		case RPL_TOPIC:
			break;
		case RPL_UMODEIS:
			break;
		case RPL_YOUREOPER:
			break;
	}
	return reply;
}
