#ifndef USERCLASS_HPP
# define USERCLASS_HPP

class User
{
	public:

	User();
	~User();

	// FUNCTION
	void		JoinChannel(Channel *toJoin);

	// SETTER
	void		SetFd(int fd);
	void		SetUsername(std::string username);
	void		SetHostname(std::string hostname);
	void		SetServername(std::string servername);
	void		SetRealname(std::string forname/*, std::string name*/); // (/!\ le realname peux etre un prenom seulement)
	void		SetNickname( std::string nickname );
	void		SetAuth(bool status);

	// GETTER
	bool		GetAuth();
	int			GetFd();
	std::string	GetNickname( void );
	std::string	GetUsername(void);
	std::string	GetHostname(void);
	std::string	GetServername(void);
	std::string	GetRealname(void);

	private:

	std::string				_nickname;
	std::string				_username;
	std::string				_hostname;
	std::string				_servername;
	std::string				_realname;
	std::string				_mode;
	int						_fd;
	bool					_isAuth;
	std::vector<Channel *>	_channels;
};

#endif