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
	std::string	_nickname;
	std::string	_userName;
	std::string	_hostName;
	std::string	_serverName;
	std::string	_realName;
	std::string	_mode;
	int			_fd;
};

#endif