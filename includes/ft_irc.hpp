#pragma once
#ifndef FT_IRC_HPP
#define FT_IRC_HPP

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

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define DEBUG()                                                     \
  (std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ \
             << ": Debug" << std::endl)
#define DEBUG_PRINT(x)                                              \
  (std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ \
             << ": Debug: " << #x << " = " << (x) << std::endl)

#endif
