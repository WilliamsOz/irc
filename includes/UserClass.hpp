#ifndef USERCLASS_HPP
# define USERCLASS_HPP

# include <sys/socket.h>
# include <sys/epoll.h>

class User
{
	public:
	User();
	~User();

	private:
	// reference de tout les channels auquels le user est connecte
	int			clientSocket;
	

};

#endif