#ifndef CHANNELCLASS_HPP
# define CHANNELCLASS_HPP

#include "irc.hpp"

class Channel
{
	public:

	Channel();
	Channel(const std::string name);
	~Channel();

	void	AddUser(User *toAdd);
	void	AddOper(User *toAdd);
	void	DelOper(User *toDel);
	bool	IsOper(User *toCheck); // utile?

	std::string GetName();
	std::string GetModes();
	std::string GetTopic();
	std::string	GetClientList();
	std::string	GetPassword();
	std::vector<User *> &GetUsers();

	bool	HasUser(User *user);
	void	SetModes(int mode, std::string param, int *j, Server *server);
	void	UnsetModes(int mode, std::string param, int *j, Server *server);

	private:

	std::vector<User *>	_users;
	std::vector<User *>	_opers;
	std::string			_password;
	std::string			_name;
	std::string			_modes;
	std::string			_topic;
	int					_limit;
};	

#endif