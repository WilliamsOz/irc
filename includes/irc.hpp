#ifndef IRC_HPP
# define IRC_HPP

// CONST
const int MAX_EVENTS = 10;

// GLOBAL
bool g_signal;

// LIB
# include <sys/epoll.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <cstring>
# include <iostream>
# include <signal.h>
# include <csignal>

// CLASS
# include "ServerClass.hpp"
# include "UserClass.hpp"
# include "ChannelClass.hpp"


#endif