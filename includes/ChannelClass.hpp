#ifndef CHANNELCLASS_HPP
# define CHANNELCLASS_HPP

class Channel
{
	public:

	Channel(const std::string name);
	~Channel();
	void	AddUser(User *toAdd);
	void	AddOper(User *toAdd);


	private:

	std::string			_name;
	std::vector<User *>	_users;
	std::string			_mode;
	std::vector<User *>	_opers;
};

#endif