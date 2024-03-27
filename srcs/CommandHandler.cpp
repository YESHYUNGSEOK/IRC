#include "CommandHandler.hpp"

void pass() // param: serverPS
{
  // if (_params.empty())
  // ERR_NEEDMOREPARAMS
  // if (_param.front() != serverPS)
  // ERR_PASSWDMISMATCH
}
void nick();   // param: user list(nick, user, host, server, real)
void user();   // param: user list
void oper();   // param: name, password, channel list
void quit();   // param: serv, client socket
void join();   // param: channel list, user list
void part();   // param: channel, user
void topic();  // param: channel list
void mode();   // param: channel
void names();  // param: user list
void list();   // param: channel list
void invite(); // param: user list, channel list

void CommandHandler::kick(std::set<Channel *> channels, std::set<Client *> clients, Client *client, Message message)
{
  // ERR_NEEDMOREPARAMS
  if (message.get_params().size() < 2)
    return;

  std::string channelName = message.get_params()[0];
  std::set<Channel *>::iterator channel_it = channels.begin();
  for (; channel_it != channels.end(); ++channel_it)
  {
    if ((*channel_it)->get_name() == channelName)
      break;
  }

  // ERR_NOSUCHCHANNEL
  if (channel_it == channels.end())
    return;
  // ERR_NOTONCHANNEL
  if (!isClientInChannel((*channel_it)->get_clients(), client))
    return;
  // ERR_CHANOPRIVSNEEDED
  if (!isOperator((*channel_it)->get_operators(), client))
    return;

  std::vector<std::string> kick_list = split(message.get_params()[1], ',');
  for (std::vector<std::string>::iterator kick_it = kick_list.begin(); kick_it != kick_list.end(); ++kick_it)
  {
    std::set<Client *>::iterator clients_it = (*channel_it)->get_clients().begin();
    for (; clients_it != (*channel_it)->get_clients().end(); ++clients_it)
    {
      if ((*clients_it)->get_nickname() == *kick_it)
      {
        (*clients_it)->part_channel(*channel_it);
        (*channel_it)->remove_client(*clients_it);
        // 클라이언트에게 KICK 메시지 전달하는 코드 추가
        break;
      }
    }
    // ERR_NOSUCHNICK
    if (clients_it == (*channel_it)->get_clients().end())
    {
      // 클라이언트가 채널에 속해있지 않다고 알리는 코드 추가
    }
  }
}

void privmsg(); // param: channel, user list
void who();
void whois();
void whowas();
void kill();
void ping();
void pong();

std::vector<std::string> split(const std::string &str, char delim)
{
  std::vector<std::string> result;
  std::string::size_type start = 0;
  std::string::size_type end = str.find_first_of(delim);

  while (end != std::string::npos)
  {
    result.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find_first_of(delim, start);
  }

  // 마지막 토큰 추가
  result.push_back(str.substr(start));

  return result;
}

bool isClientInChannel(const std::set<Client *> &clients, Client *client)
{
  return clients.find(client) != clients.end();
}

bool isOperator(const std::set<Client *> &operators, Client *client)
{
  return operators.find(client) != operators.end();
}