#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "ft_irc.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
  (void)argc;

  try
  {
    Server server(atoi(argv[1]), atoi(argv[2]));
    server.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  // try
  // {
  //   // if (argc != 3) throw std::runtime_error("Usage: ./ircserv <port> <password>");
  //   struct sockaddr_in addr;
  //   std::memset(&addr, 0, sizeof(addr));
  //   int port = std::atoi(argv[1]);
  //   // std::string password = argv[2];

  //   int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // IPv4, TCP
  //   if (server_fd == -1)
  //     throw std::runtime_error("Failed to create socket");

  //   addr.sin_family = AF_INET;         // IPv4
  //   addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
  //   addr.sin_port = htons(port);       // Little endian to big endian

  //   if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  //     throw std::runtime_error("Failed to bind socket");

  //   if (listen(server_fd, 10) == -1)
  //     throw std::runtime_error("Failed to listen");

  //   std::cout << "Server started on port " << port << std::endl;

  //   struct sockaddr_in client_addr;
  //   socklen_t client_addr_len = sizeof(client_addr);

  //   struct pollfd fds[1];
  //   fds[0].fd = server_fd;
  //   fds[0].events = POLLIN;

  //   while (true)
  //   {
  //     int ret = poll(fds, 1, -1);
  //     if (ret == -1)
  //       throw std::runtime_error("Failed to poll");

  //     if (fds[0].revents & POLLIN)
  //     {
  //       int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len); // Block until client connects
  //       if (client_fd == -1)
  //         throw std::runtime_error("Failed to accept connection");

  //       std::cout << "Client connected" << std::endl;

  //       while (true)
  //       {
  //         /* code */

  //         char buffer[1024] = {0};
  //         int valread = read(client_fd, buffer, 1024);
  //         if (valread == 0)
  //         {
  //           std::cout << "Client disconnected" << std::endl;
  //           break;
  //         }
  //         std::cout << buffer;
  //       }
  //     }
  //   }
  // }
  // catch (std::exception &e)
  // {
  //   std::cerr << "Error: " << e.what() << std::endl;
  // }

  return 0;
}

// int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len); // Block until client connects
// if (client_fd == -1)
//   throw std::runtime_error("Failed to accept connection");

// std::cout << "Client connected" << std::endl;

// while (true)
// {
//   /* code */

//   char buffer[1024] = {0};
//   int valread = read(client_fd, buffer, 1024);
//   if (valread == 0)
//   {
//     std::cout << "Client disconnected" << std::endl;
//     break;
//   }
//   std::cout << buffer;
// }
// }
// catch (std::exception &e)
// {
//   std::cerr << "Error: " << e.what() << std::endl;
// }

// return 0;
// }
