#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <sys/epoll.h>

class client
{
	public:
	int			clientSocket;
	epoll_event events[MAX_EVENTS];
	epoll_event eventClient;
	private:

};

#endif