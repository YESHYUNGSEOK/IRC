#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <iostream>
#include <map>
#include <string>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"

class Client;
class Channel;
class Message;

class CommandHandler
{
public:
  // static void getCommand(e_cmd cmd);
  // every command functions
  static void pass();                                                                // param: serverPS
  static void nick();                                                                // param: user list(nick, user, host, server, real)
  static void user();                                                                // param: user list
  static void oper();                                                                // param: name, password, channel list
  static void quit();                                                                // param: serv, client socket
  static void join();                                                                // param: channel list, user list
  static void part(std::set<Channel *> channels, Client *client, Message message);   // param: channel, user
  static void topic(std::set<Channel *> channels, Client *client, Message message);  // param: channel list
  static void mode();                                                                // param: channel
  static void names();                                                               // param: user list
  static void list();                                                                // param: channel list
  static void invite(std::set<Channel *> channels, Client *client, Message message); // param: user list, channel list
  static void kick(std::set<Channel *> channels, Client *client, Message message);   // param: user
  static void privmsg();                                                             // param: channel, user list
  static void who();
  static void whois();
  static void whowas();
  static void kill();
  static void ping();
  static void pong();
};

std::vector<std::string> split(const std::string &str, char delim);

#endif
