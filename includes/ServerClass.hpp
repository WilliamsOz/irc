#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

int	server();

class Server
{
	public:
	Server();
	~Server();
	static void signal_handling(int);

	private:
	int			socketServer;
	sockaddr_in	serverAddress;
	int			epollfd;
};

# endif