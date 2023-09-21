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

	std::string GetName();

	void	SetModes(std::string modes);

	private:

	std::string			_name;
	std::vector<User *>	_users;
	std::string			_modes;
	std::vector<User *>	_opers;
};

#endif