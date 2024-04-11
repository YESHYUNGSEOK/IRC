#ifndef NUMERICREPLY_HPP
#define NUMERICREPLY_HPP

#include <iostream>
#include <string>

#include "ft_irc.hpp"

#define CLIENT_SOURCE(client)                                      \
  ((client).get_nickname() + "!" + (client).get_username() + "@" + \
   (client).get_hostname())
#define SERVER_SOURCE "localhost"

// define RPL_REGISTRATION
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
#define ERR_INVALIDCAPCMD_410(client, command)          \
  ("410 " + (client).get_nickname() + " " + (command) + \
   ":Invalid CAP command\r\n")

// define PASS_REPLIES
#define ERR_ALREADYREGISTRED_462(client) \
  ("462 " + (client).get_nickname() + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH_464(client) \
  ("464 " + (client).get_nickname() + " :Password incorrect\r\n")

// define NICK_REPLIES
#define RPL_BRDCAST_NICKCHANGE(client, old_nick) \
  (":" + (old_nick) + " NICK " + (client).get_nickname() + "\r\n")
#define ERR_NONICKNAMEGIVEN_431(client) \
  ("431 " + (client).get_nickname() + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME_432(client, nickname)       \
  ("432 " + (client).get_nickname() + " " + (nickname) + \
   " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE_433(client, nickname)          \
  ("433 " + (client).get_nickname() + " " + (nickname) + \
   " :Nickname is already in use\r\n")

// define QUIT_REPLIES
#define RPL_QUIT(client, message) \
  (":" + CLIENT_SOURCE((client)) + " QUIT :" + (message) + "\r\n")

// define ERROR_REPLIES
#define RPL_ERROR(message) ("ERROR :" + (message) + "\r\n")

// define JOIN_REPLIES
#define RPL_BRDCAST_JOIN(client, channel) \
  (":" + CLIENT_SOURCE((client)) + " JOIN " + (channel).get_name() + "\r\n")
#define RPL_NAMREPLY(client, channel, user_list)                            \
  ("353 " + (client).get_nickname() + " = " + (channel).get_name() + " :" + \
   (user_list) + "\r\n")
#define RPL_ENDOFNAMES(client, channel)                            \
  ("366 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :End of /NAMES list\r\n")
#define RPL_DEFAULTCHANMODE(client, channel) \
  ("324 " + (client).get_nickname() + " " + (channel).get_name() + " +\r\n")
#define ERR_USERNOTINCHANNEL_441(client, target, channel)                   \
  ("441 " + (client).get_nickname() + " " + (target).get_nickname() + " " + \
   (channel).get_name() + " :They aren't on that channel\r\n")
#define ERR_USERONCHANNEL_443(client, target, channel)                      \
  ("443 " + (client).get_nickname() + " " + (target).get_nickname() + " " + \
   (channel).get_name() + " :is already on channel\r\n")
#define ERR_NOTONCHANNEL_442(client, channel)                      \
  ("442 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :You're not on that channel\r\n")
#define ERR_CHANNELISFULL_471(client, channel)                     \
  ("471 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN_473(client, channel)                    \
  ("473 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY_475(client, channel)                     \
  ("475 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED_482(client, channel)                  \
  ("482 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :You're not channel operator\r\n")

// define PART_REPLIES
#define RPL_BRDCAST_PART(client, channel, message)                          \
  (":" + CLIENT_SOURCE((client)) + " PART " + (channel).get_name() + " :" + \
   (message) + "\r\n")
#define ERR_NOSUCHCHANNEL_403(client, param) \
  ("403 " + (client).get_nickname() + " " + (param) + " :No such channel\r\n")

// define TOPIC_REPLIES
#define RPL_BRDCAST_TOPIC(client, channel, topic)                           \
  (":" + CLIENT_SOURCE((client)) + " TOPIC " + (channel) + " :" + (topic) + \
   "\r\n")
#define RPL_NOTOPIC_331(channel) \
  (":" SERVER_SOURCE " 331 " + (channel) + " :No topic is set\r\n")
#define RPL_TOPIC_332(channel, topic) \
  (":" SERVER_SOURCE " 332 " + (channel) + " :" + (topic) + "\r\n")
#define ERR_TOPICTOOLONG_417(client, channel)                      \
  ("417 " + (client).get_nickname() + " " + (channel).get_name() + \
   " :Topic too long\r\n")

// define INVITE_REPLIES
#define RPL_INVITING_341(client, target, channel)                         \
  (":" + (client).get_nickname() + " INVITE " + (target).get_nickname() + \
   " " + (channel).get_name() + "\r\n")

// define KICK_REPLIES
#define RPL_BRDCAST_KICK(client, channel, target, comment)                 \
  (":" + (client).get_nickname() + " KICK " + (channel).get_name() + " " + \
   (target).get_nickname() + " :" + (comment) + "\r\n")

// define PING_REPLIES
#define RPL_PONG(client, params) ("PONG :" + params + "\r\n")

// define MODE_REPLIES
#define RPL_BRDCAST_MODE(client, channel, mode, params)                    \
  (":" + CLIENT_SOURCE((client)) + " MODE " + (channel).get_name() + " " + \
   (mode) + " " + (params) + "\r\n")
#define ERR_BADCHANMASK_476(client, channel)            \
  ("476 " + (client).get_nickname() + " " + (channel) + \
   " :Bad Channel Mask\r\n")
#define ERR_UMODEUNKNOWNFLAG_501(client) \
  ("501 " + (client).get_nickname() + " :Unknown mode flag\r\n")

// define PRIVMSG_REPLIES
#define RPL_PRIVMSG(client, target, message)                                   \
  (":" + CLIENT_SOURCE((client)) + " PRIVMSG " + (target) + " :" + (message) + \
   "\r\n")

// define ERR_REPLIES
#define ERR_NOSUCHNICK_401(client, target)             \
  ("401 " + (client).get_nickname() + " " + (target) + \
   " :No such nick/channel\r\n")
#define ERR_INPUTTOOLONG_417(client) \
  ("417 " + (client).get_nickname() + " :Input line was too long\r\n")
#define ERR_UNKNOWNCOMMAND_421(client, command)           \
  (("421 ") + (client).get_nickname() + " " + (command) + \
   " :Unknown command\r\n")
#define ERR_USERONCHANNEL_443(client, target, channel)                      \
  ("443 " + (client).get_nickname() + " " + (target).get_nickname() + " " + \
   (channel).get_name() + " :is already on channel\r\n")
#define ERR_NOTREGISTERED_451(client) \
  ("451 " + (client).get_nickname() + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS_461(client) \
  ("461 " + (client).get_nickname() + " :Not enough parameters\r\n")

#endif
