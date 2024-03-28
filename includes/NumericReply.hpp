#ifndef NUMERICREPLY_HPP
#define NUMERICREPLY_HPP

#include <iostream>
#include <string>

#include "Server.hpp"
#include "ft_irc.hpp"

class Server;

#define SERVER_PREFIX (":localhost")
#define AVAIL_USRMOD ("itkol")
#define AVAIL_CHANMOD ("itkol")

#define CLIENT_SOURCE(client)                                      \
  ((client).get_nickname() + "!" + (client).get_username() + "@" + \
   (client).get_hostname())

// define RPL_REPLIES
#define RPL_WELCOME_001(client)                                            \
  ("001 " + (client).get_nickname() + " :Welcome to the FT_IRC Network " + \
   CLIENT_SOURCE((client)) + "\r\n")
#define RPL_YOURHOST_002(client)                                   \
  ("002 " + (client).get_nickname() + " :Your host is localhost" + \
   ", running FT_IRC/1.0\r\n")
#define RPL_CREATED_003(client, created_at)                          \
  ("003 " + (client).get_nickname() + " :This server was created " + \
   (created_at) + "\r\n")
#define RPL_MYINFO_004(client)                                            \
  ("004 " + (client).get_nickname() + " :localhost 1.0 " + AVAIL_USRMOD + \
   " " + AVAIL_CHANMOD + "\r\n")

// define CAP_REPLIES
#define RPL_CAP_LS(client) ("CAP " + (client).get_nickname() + " LS :\r\n")
#define RPL_CAP_LIST(client) ("CAP " + (client).get_nickname() + " LIST :\r\n")
#define RPL_CAP_NAK(client, params) \
  ("CAP " + (client).get_nickname() + " NAK :" + params + "\r\n")

// define ERR_REPLIES
#define ERR_INVALIDCAPCMD_410(client, command)          \
  ("410 " + (client).get_nickname() + " " + (command) + \
   ":Invalid CAP command\r\n")
#define ERR_INPUTTOOLONG_417(client) \
  ("417 " + (client).get_nickname() + " :Input line was too long\r\n")
#define ERR_UNKNOWNCOMMAND_421(client, command)           \
  (("421 ") + (client).get_nickname() + " " + (command) + \
   " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN_431(client) \
  ("431 " + (client).get_nickname() + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME_432(client, nickname)       \
  ("432 " + (client).get_nickname() + " " + (nickname) + \
   " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE_433(client, nickname)          \
  ("433 " + (client).get_nickname() + " " + (nickname) + \
   " :Nickname is already in use\r\n")
#define ERR_NOTREGISTERED_451(client) \
  ("451 " + (client).get_nickname() + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS_461(client) \
  ("461 " + (client).get_nickname() + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED_462(client) \
  ("462 " + (client).get_nickname() + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH_464(client) \
  ("464 " + (client).get_nickname() + " :Password incorrect\r\n")

typedef enum NUMERIC {
  // ERR
  ERR_ALREADYREGISTRED = 462,
  ERR_BADCHANMASK = 476,
  ERR_BADCHANNELKEY = 475,
  ERR_BANNEDFROMCHAN = 474,
  ERR_CANNOTSENDTOCHAN = 404,
  ERR_CANTKILLSERVER = 483,
  ERR_CHANOPRIVSNEEDED = 482,
  ERR_CHANNELISFULL = 471,
  ERR_ERRONEUSNICKNAME = 432,
  ERR_INVITEONLYCHAN = 473,
  ERR_KEYSET = 467,
  ERR_NEEDMOREPARAMS = 461,
  ERR_NICKCOLLISION = 436,
  ERR_NICKNAMEINUSE = 433,
  ERR_NONICKNAMEGIVEN = 431,
  ERR_NOOPERHOST = 491,
  ERR_NORECIPIENT = 411,
  ERR_NOSUCHCHANNEL = 403,
  ERR_NOSUCHNICK = 401,
  ERR_NOSUCHSERVER = 402,
  ERR_NOTEXTTOSEND = 412,
  ERR_NOTONCHANNEL = 442,
  ERR_NOORIGIN = 409,
  ERR_PASSWDMISMATCH = 464,
  ERR_TOOMANYTARGETS = 407,
  ERR_UMODEUNKNOWNFLAG = 501,
  ERR_USERSDONTMATCH = 502,
  ERR_WILDTOPLEVEL = 414,

  // RPL
  RPL_UMODEIS = 221,
  RPL_CHANNELMODEIS = 324,
  RPL_ENDOFBANLIST = 368,
  RPL_ENDOFNAMES = 366,
  RPL_INVITING = 341,
  RPL_LIST = 322,
  RPL_LISTEND = 323,
  RPL_LISTSTART = 321,
  RPL_NAMREPLY = 353,
  RPL_NOTOPIC = 331,
  RPL_TOPIC = 332,
  RPL_YOUREOPER = 381
} e_numeric;

class NumericReply {
 public:
  static std::string getNumericReplyERR(e_numeric numeric, Server &serv);
  static std::string getNumericReplyRPL(e_numeric numeric, Server &serv);
};

#endif

// typedef enum NUMERIC {
// 	//PASS
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_ALREADYREGISTRED = 462,
// 	ERR_PASSWDMISMATCH = 464,

// 	//NICK
// 	ERR_NONICKNAMEGIVEN = 431,
// 	ERR_ERRONEUSNICKNAME = 432,
// 	ERR_NICKNAMEINUSE = 433,
// 	ERR_NICKCOLLISION = 436,

// 	//USER
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_ALREADYREGISTRED = 462,

// 	//OPER
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOOPERHOST = 491,
// 	ERR_PASSWDMISMATCH = 464,
// 	RPL_YOUREOPER = 381,

// 	//JOIN
// 	ERR_BADCHANMASK = 476,
// 	ERR_INVITEONLYCHAN = 473,
// 	ERR_BANNEDFROMCHAN = 474,
// 	ERR_BADCHANNELKEY = 475,
// 	ERR_CHANNELISFULL = 471,
// 	ERR_NOSUCHCHANNEL = 403,

// 	//PART
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOSUCHCHANNEL = 403,
// 	ERR_NOTONCHANNEL = 442,

// 	//MODE
// 	ERR_NEEDMOREPARAMS = 461,
// 	RPL_CHANNELMODEIS = 324,
// 	ERR_CHANOPRIVSNEEDED = 482,
// 	ERR_NOSUCHNICK = 401,
// 	ERR_UMODEUNKNOWNFLAG = 501,
// 	ERR_USERSDONTMATCH = 502,
// 	ERR_KEYSET = 467,
// 	ERR_BANLIST = 368,
// 	RPL_ENDOFBANLIST = 368,
// 	RPL_UMODEIS = 221,
// 	ERR_UMODEUNKNOWNFLAG = 501,

// 	//TOPIC
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOSUCHCHANNEL = 403,
// 	RPL_NOTOPIC = 331,
// 	RPL_TOPIC = 332,
// 	ERR_CHANOPRIVSNEEDED = 482,

// 	//NAMES
// 	RPL_NAMREPLY = 353,
// 	RPL_ENDOFNAMES = 366,

// 	//LIST
// 	RPL_LIST = 322,
// 	RPL_LISTEND = 323,
// 	RPL_LISTSTART = 321,
// 	ERR_NOSUCHSERVER = 402,

// 	//INVITE
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOSUCHNICK = 401,
// 	ERR_NOTONCHANNEL = 442,
// 	ERR_USERONCHANNEL = 443,
// 	ERR_CHANOPRIVSNEEDED = 482,
// 	RPL_INVITING = 341,
// 	RPL_AWAY = 301,

// 	//KICK
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOSUCHCHANNEL = 403,
// 	ERR_BADCHANMASK = 476,
// 	ERR_CHANOPRIVSNEEDED = 482,
// 	ERR_NOTONCHANNEL = 442,

// 	//PRIVMSG
// 	ERR_NORECIPIENT = 411,
// 	ERR_NOTEXTTOSEND = 412,
// 	ERR_CANNOTSENDTOCHAN = 404,
// 	ERR_NOTOPLEVEL = 413,
// 	ERR_WILDTOPLEVEL = 414,
// 	ERR_TOOMANYTARGETS = 407,
// 	ERR_NOSUCHNICK = 401,
// 	RPL_AWAY = 301,

// 	//PING
// 	ERR_NOORIGIN = 409,
// 	ERR_NOSUCHSERVER = 402,

// 	//PONG
// 	ERR_NOORIGIN = 409,
// 	ERR_NOSUCHSERVER = 402,

// 	//KILL
// 	ERR_NOPRIVILEGES = 481,
// 	ERR_NEEDMOREPARAMS = 461,
// 	ERR_NOSUCHNICK = 401,
// 	ERR_CANTKILLSERVER = 483,

// } e_numeric;
