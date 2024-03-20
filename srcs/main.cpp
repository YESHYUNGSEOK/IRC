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

#include "Server.hpp"
#include "ft_irc.hpp"

int main(int argc, char **argv) {
  (void)argc;

  try {
    Server server(atoi(argv[1]), atoi(argv[2]));
    server.run();
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
