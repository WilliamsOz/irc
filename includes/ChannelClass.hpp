#ifndef CHANNELCLASS_HPP
# define CHANNELCLASS_HPP

#include "irc.hpp"

class Command;

class Channel
{
	public:

	Channel();
	Channel(const std::string name);
	~Channel();

	void	AddUser(User *toAdd);
	void	AddOper(User *toAdd);
	void	DelOper(User *toDel);
	bool	IsOper(User *toCheck);
	void	AddUserToInviteList(User *toAdd);
	bool	IsUserInvited(User *toCheck);
	void	SetFounder(std::string founderName);
	void	SetTopic(std::string newTopic);

	std::string GetName();
	std::string GetModes();
	std::string GetTopic();
	std::string	GetClientList();
	std::string	GetPassword();
	std::vector<User *> &GetUsers();

	bool		HasUser(User *user);
	void		SetModes(char mode, std::stack<std::string> *modeParam, Server *server, Command *cmd, User *user);
	void		SetModes(std::string modes);
	void		UnsetModes(char mode, std::stack<std::string> *modeParams, Server *server, Command *cmd, User *user);
	private:

	std::string			_founder;
	std::vector<User *>	_users;
	std::vector<User *>	_opers;
	std::string			_password;
	std::vector<User *>	_invited;
	std::string			_name;
	std::string			_modes;
	std::string			_topic;
	int					_limit;
};	

#endif