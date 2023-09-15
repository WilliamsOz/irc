#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include "irc.hpp"

class User;

class Server
{
	public:

	Server(int port, char *password);
	// ~Server();

	void	AddUser();
	void	LaunchServer();
	static void signal_handler(int);

	std::string	GetServerPassword( void ) { return(this->_password); }
	int			GetEpollFd();
	epoll_event	GetClientEvent();
	User		*GetSpecificUser(int fd);

	private:

	int								_port;
	std::string						_password;
	int								_socketServer;
	sockaddr_in						_serverAddress;
	int								_epollfd;
	epoll_event						_serverEvent;
	epoll_event						_clientEvent;
	epoll_event 					_events[MAX_EVENTS];
	std::map<int, User *>			_users;
};

# endif