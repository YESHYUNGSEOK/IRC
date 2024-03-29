#include "CommandHandler.hpp"

void pass() // param: serverPS
{
  // if (_params.empty())
  // ERR_NEEDMOREPARAMS
  // if (_param.front() != serverPS)
  // ERR_PASSWDMISMATCH
}
void nick(); // param: user list(nick, user, host, server, real)
void user(); // param: user list
void oper(); // param: name, password, channel list
void quit(); // param: serv, client socket
void join(); // param: channel list, user list

void CommandHandler::part(std::set<Channel *> channels, Client *client,
                          Message message)
{
  // ERR_NEEDMOREPARAMS
  if (!message.get_params().size())
    return;

  std::vector<std::string> depart_channels = split(message.get_params()[0], ',');
  std::string reason = message.get_params().size() > 1 ? message.get_params()[1] : "";

  for (std::vector<std::string>::iterator depart_it = depart_channels.begin();
       depart_it != depart_channels.end(); ++depart_it)
  {
    std::set<Channel *>::iterator channel_it = channels.begin();
    for (; channel_it != channels.end(); ++channel_it)
    {
      if ((*channel_it)->get_name() == *depart_it)
        break;
    }

    // ERR_NOSUCHCHANNEL
    if (channel_it == channels.end())
      continue;
    // ERR_NOTONCHANNEL
    if (!(*channel_it)->is_client_in_channel(client))
      continue;

    (*channel_it)->remove_client(client);
    client->part_channel(*channel_it);
    // 클라이언트에게 PART 메시지 전달하는 코드 추가
  }
};

void CommandHandler::topic(std::set<Channel *> channels, Client *client,
                           Message message)
{
  // ERR_NEEDMOREPARAMS
  if (!message.get_params().size())
    return;

  std::string channel_name = message.get_params()[0];
  std::set<Channel *>::iterator channel_it = channels.begin();
  for (; channel_it != channels.end(); ++channel_it)
  {
    if ((*channel_it)->get_name() == channel_name)
      break;
  }

  // ERR_NOSUCHCHANNEL
  if (channel_it == channels.end())
    return;
  // ERR_NOTONCHANNEL
  if (!(*channel_it)->is_client_in_channel(client))
    return;

  // 채널 토픽 확인
  if (message.get_params().size() == 1)
  {
    std::string topic = (*channel_it)->get_topic();
    // RPL_NOTOPIC
    if (topic == "")
      return;
    // RPL_TOPIC
    // RPL_TOPICWHOTIME
  }

  // 채널 토픽 변경
  else
  {
    // ERR_CHANOPRIVSNEEDED
    if ((*channel_it)->get_mode(TOPIC_RESTRICTED) &&
        !(*channel_it)->is_operator(client))
      return;

    std::string new_topic = message.get_params()[1];
    (*channel_it)->set_new_topic(new_topic, client->get_nickname());
    // RPL_TOPIC
    // RPL_TOPICWHOTIME
  }
}

// param: channel list
void mode();  // param: channel
void names(); // param: user list
void list();  // param: channel list

void CommandHandler::invite(std::set<Channel *> channels, Client *client,
                            Message message)
{
  // ERR_NEEDMOREPARAMS
  if (message.get_params().size() < 2)
    return;

  std::string client_name = message.get_params()[0];
  std::string channel_name = message.get_params()[1];
  std::set<Channel *>::iterator channel_it = channels.begin();
  for (; channel_it != channels.end(); ++channel_it)
  {
    if ((*channel_it)->get_name() == channel_name)
      break;
  }

  // ERR_NOSUCHCHANNEL
  if (channel_it == channels.end())
    return;
  // ERR_NOTONCHANNEL
  if (!(*channel_it)->is_client_in_channel(client))
    return;
  // ERR_CHANOPRIVSNEEDED
  if (!(*channel_it)->is_operator(client) ||
      !(*channel_it)->get_channel_mode(INVITE_ONLY))
    return;
  // ERR_USERONCHANNEL
  if ((*channel_it)->is_client_in_channel(client_name))
    return;

  std::set<Client *>::iterator clients_it =
      (*channel_it)->get_clients().begin();
  for (; clients_it != (*channel_it)->get_clients().end(); ++clients_it)
  {
    if ((*clients_it)->get_nickname() == client_name)
    {
      (*channel_it)->invite_client(*clients_it);
      // 클라이언트에게 INVITE 메시지 전달하는 코드 추가
      break;
    }
  }
}

void CommandHandler::kick(std::set<Channel *> channels, Client *client,
                          Message message)
{
  // ERR_NEEDMOREPARAMS
  if (message.get_params().size() < 2)
    return;

  std::string channel_name = message.get_params()[0];
  std::set<Channel *>::iterator channel_it = channels.begin();
  for (; channel_it != channels.end(); ++channel_it)
  {
    if ((*channel_it)->get_name() == channel_name)
      break;
  }

  // ERR_NOSUCHCHANNEL
  if (channel_it == channels.end())
    return;
  // ERR_NOTONCHANNEL
  if (!(*channel_it)->is_client_in_channel(client))
    return;
  // ERR_CHANOPRIVSNEEDED
  if (!(*channel_it)->is_operator(client))
    return;

  std::vector<std::string> kick_list = split(message.get_params()[1], ',');
  for (std::vector<std::string>::iterator kick_it = kick_list.begin();
       kick_it != kick_list.end(); ++kick_it)
  {
    std::set<Client *>::iterator clients_it =
        (*channel_it)->get_clients().begin();
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
