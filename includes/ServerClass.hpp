#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

class	User;
class	Channel;

class Server
{
	public:

	Server(int port, const char *password);
	// ~Server();

	void		AddUser();
	void		LaunchServer();
	static void SignalHandler(int);
	bool		HasChannel(std::string name);
	Channel*	AddChannel(std::string name);
	void		AddUserToChannel(User *user, std::string name);

	Channel		*GetChannelByName(std::string);
	void        SendMessagetoClient(User* recipient, std::string msg);
	int			GetFdByNickName(std::string nickName);
	User		*GetUserByNickname(std::string nickName);
	User		*GetUserByFd(int fd);
	std::string	GetServerPassword( void ) { return(this->_password); }
	int			GetEpollFd();
	epoll_event*	GetClientEvent();
	std::map<int, User *>&	GetUsers();

	private:

	int									_port;
	std::string							_password;
	int									_socketServer;
	int									_epollfd;
	sockaddr_in							_serverAddress; // a mettre dans LaunchServer()
	epoll_event							_serverEvent; // a mettre dans LaunchServer()
	epoll_event							_clientEvent; // a mettre dans LaunchServer()
	epoll_event 						_events[MAX_EVENTS]; // a mettre dans LaunchServer()
	std::map<int, User *>				_users;
	std::map<std::string, Channel *>	_channels;
};

# endif