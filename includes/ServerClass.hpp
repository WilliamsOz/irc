#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include "irc.hpp"

class User;

int	server();

class Server
{
	public:

	Server(int port, char *password);
	// ~Server();

	User	*AddUser();
	void	LaunchServer();
	static void signal_handler(int);
	// void	ParseInput(std::string input, int clientFd);
	std::string	GetServerPassword( void ) { return(this->_password); }
	int			GetEpollFd();
	epoll_event	GetClientEvent();

	private:

	int								_port;
	std::string						_password;
	int								_socketServer;
	sockaddr_in						_serverAddress;
	int								_epollfd;
	epoll_event						_serverEvent;
	epoll_event						_clientEvent;
	epoll_event 					_events[MAX_EVENTS];
	std::map<std::string, User *>	_users;
};

# endif