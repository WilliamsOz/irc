#ifndef CHANNELCLASS_HPP
# define CHANNELCLASS_HPP

class Channel
{
	public:

	Channel();
	Channel(const std::string name);
	~Channel();

	void	AddUser(User *toAdd);
	void	AddOper(User *toAdd);
	bool	IsOper(User *toCheck);

	std::string GetName();
	std::string GetModes();
	std::string GetTopic();
	std::string	GetClientList();
	std::vector<User *> &GetUsers();
	bool		IsInBlackList(User *user);

	bool	HasUser(User *user);

	void	SetModes(std::string modes);

	private:

	std::string			_name;
	std::vector<User *>	_users;
	std::string			_modes;
	std::vector<User *>	_opers;
	std::vector<User *>	_blacklist;
	std::string			_topic;
};

#endif