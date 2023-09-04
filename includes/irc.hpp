#ifndef IRC_HPP
# define IRC_HPP

// CONST
const int MAX_EVENTS = 10;

// LIB
# include <sys/epoll.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <cstring>
# include <iostream>
# include <csignal>

# include <signal.h>

// CLASS
# include "ServerClass.hpp"
# include "UserClass.hpp"
# include "ChannelClass.hpp"

// GLOBAL
extern volatile sig_atomic_t g_signal;


#endif