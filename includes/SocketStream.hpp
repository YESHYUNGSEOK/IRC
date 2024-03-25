#pragma once
#ifndef SOCKETSTREAM_HPP
#define SOCKETSTREAM_HPP

#include <exception>
#include <string>
#include <vector>

#include "ft_irc.hpp"

class SocketStream
{
private:
    static const int BUFFER_SIZE = 1024;
    static const int LINE_SIZE_MAX = 512;

    const struct sockaddr_in _addr;
    const socklen_t _addr_len;
    const int _fd;
    std::string _read_buffer;
    std::string _write_buffer;
    char *_raw_buffer;

    SocketStream();
    SocketStream(const SocketStream &src);
    SocketStream &operator=(const SocketStream &src);

public:
    SocketStream(const int server_fd);
    ~SocketStream();

    int get_fd() const;
    void recv();
    void send();

    SocketStream &operator<<(const std::string &data);
    SocketStream &operator>>(std::string &data);
    SocketStream &operator>>(std::vector<std::string> &data); // 안쓸수도 있음

    class SystemCallException : public std::exception
    {
    public:
        SystemCallException();
        SystemCallException(const SystemCallException &src);
        SystemCallException &operator=(const SystemCallException &src);
        virtual ~SystemCallException() throw();
        virtual const char *what() const throw();
    };

    class ConnectionClosedException : public std::exception
    {
    public:
        ConnectionClosedException();
        ConnectionClosedException(const ConnectionClosedException &src);
        ConnectionClosedException &operator=(const ConnectionClosedException &src);
        virtual ~ConnectionClosedException() throw();
        virtual const char *what() const throw();
    };

    class NoNewlineException : public std::exception
    {
    public:
        NoNewlineException();
        NoNewlineException(const NoNewlineException &src);
        NoNewlineException &operator=(const NoNewlineException &src);
        virtual ~NoNewlineException() throw();
        virtual const char *what() const throw();
    };
    class MessageTooLongException : public std::exception
    {
    public:
        MessageTooLongException();
        MessageTooLongException(const MessageTooLongException &src);
        MessageTooLongException &operator=(const MessageTooLongException &src);
        virtual ~MessageTooLongException() throw();
        virtual const char *what() const throw();
    };
};

#endif // IOHANDLER_HPP
