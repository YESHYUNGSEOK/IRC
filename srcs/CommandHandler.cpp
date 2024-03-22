#include "CommandHandler.hpp"

void pass()  // param: serverPS
{
  // if (_params.empty())
  // ERR_NEEDMOREPARAMS
  // if (_param.front() != serverPS)
  // ERR_PASSWDMISMATCH
}
void nick();     // param: user list(nick, user, host, server, real)
void user();     // param: user list
void oper();     // param: name, password, channel list
void quit();     // param: serv, client socket
void join();     // param: channel list, user list
void part();     // param: channel, user
void topic();    // param: channel list
void mode();     // param: channel
void names();    // param: user list
void list();     // param: channel list
void invite();   // param: user list, channel list
void kick();     // param: user
void privmsg();  // param: channel, user list
void who();
void whois();
void whowas();
void kill();
void ping();
void pong();
