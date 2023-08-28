#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <sys/epoll.h>

int	server();

class server
{
	private:
	int			socketServer;
	sockaddr_in	serverAddress;
	int			epollfd;
	epoll_event eventServer;
	public:
};

# endif