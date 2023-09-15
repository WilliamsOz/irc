#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

int	server();

class Server
{
	public:

	Server(int port, char *password);
	// ~Server();

	void	LaunchServer();
	// User	*GetUserByFd();
	int		GetFdbyUser();
	static void SignalHandler(int);

	private:

	int			_port;
	std::string	_password;
	int			_socketServer;
	sockaddr_in	_serverAddress;
	int			_epollfd;
	epoll_event	_serverEvent;
	epoll_event	_clientEvent;
	epoll_event _events[MAX_EVENTS];
};

# endif