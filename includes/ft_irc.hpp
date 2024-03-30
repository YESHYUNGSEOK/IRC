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

#ifndef __unused
#define __unused __attribute__((unused))
#endif

#ifndef SERVER_NAME
#define SERVER_NAME ("FT_IRC")
#endif

#ifndef SERVER_VERSION
#define SERVER_VERSION ("1.0")
#endif

#ifndef CHANLIMIT
#define CHANLIMIT 50
#endif

#ifndef CHANNELLEN
#define CHANNELLEN 50
#endif

#ifndef CHANTYPES
#define CHANTYPES ("#&")
#endif

#ifndef HOSTLEN
#define HOSTLEN 63
#endif

#ifndef KICKLEN
#define KICKLEN 80
#endif

#ifndef NICKLEN
#define NICKLEN 9
#endif

#ifndef TOPICLEN
#define TOPICLEN 80
#endif

#ifndef USERLEN
#define USERLEN 10
#endif

#ifndef AVAIL_USRMOD
#define AVAIL_USRMOD ("a")
#endif

#ifndef AVAIL_CHANMOD
#define AVAIL_CHANMOD ("itkol")
#endif

#define DEBUG()                                                     \
  (std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ \
             << ": Debug" << std::endl)
#define DEBUG_PRINT(x)                                              \
  (std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ \
             << ": Debug: " << #x << " = " << (x) << std::endl)

#endif
