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
	std::string	GetPassword();
	std::vector<User *> &GetUsers();

	bool	HasUser(User *user);
	void	SetModes(std::string modes);

	private:

	std::vector<User *>	_users;
	std::vector<User *>	_opers;
	std::string			_name;
	std::string			_modes;
	std::string			_topic;
	std::string			_password;
};

#endif