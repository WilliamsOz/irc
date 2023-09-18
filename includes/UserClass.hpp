#ifndef USERCLASS_HPP
# define USERCLASS_HPP

# include <sys/socket.h>
# include <sys/epoll.h>

class User
{
	public:
	User();
	// ~User();

	void		SetFd(int fd);
	void		SetUsername(std::string username);
	void		SetHostname(std::string hostname);
	void		SetServername(std::string servername);
	void		SetRealname(std::string realname);
	void		SetNickname( std::string nickname );
	int			GetFd();
	std::string	GetNickname( void );
	std::string	GetUsername(void);
	std::string	GetHostname(void);
	std::string	GetServername(void);
	std::string	GetRealname(void);

	private:
	// reference de tout les channels auquels le user est connecte

	std::string	_nickname;
	std::string	_username;
	std::string	_hostname;
	std::string	_servername;
	std::string	_realname;
	std::string	_mode;
	int			_fd;
	bool		_isAuth;
};

#endif