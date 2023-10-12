# include "irc.hpp"

Channel::Channel() : _topic("")
{
	return ;
}

Channel::Channel(const std::string name) : _name(name), _topic("")
{
	return ;
}

Channel::~Channel()
{
	return ;
}

std::string	Channel::GetName()
{
	return (this->_name);
}

std::string	Channel::GetModes()
{
	return (this->_modes);
}

std::string Channel::GetTopic()
{
	return (this->_topic);
}

std::vector<User *>	&Channel::GetUsers()
{
	return (this->_users);
}

bool	Channel::IsOper(User *toCheck)
{
	User *user = NULL;

	for (std::vector<User *>::iterator it = this->_opers.begin(); it != this->_opers.end(); it++)
	{
		user = *it;
		if (toCheck->GetNickname() == user->GetNickname())
			return (true);
	}
	return (false);
}

std::string	Channel::GetClientList()
{
	User *user = NULL;
	std::string userInfo;

	for (std::vector<User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		user = *it;
		if (this->IsOper(user) == true)
			userInfo += '@';
		else
			userInfo += '+';
		userInfo += user->GetNickname();
		userInfo += ' ';
	}
	return (userInfo);
}

std::string	Channel::GetPassword()
{
	return (this->_password);
}

void	Channel::AddUser(User *toAdd)
{
	this->_users.push_back(toAdd);
	return ;
}

void	Channel::AddOper(User *toAdd)
{
	this->_opers.push_back(toAdd);
	return ;
}

void	Channel::AddUserToInviteList(User *toAdd)
{
	if (IsUserInvited(toAdd) == false)
		this->_invited.push_back(toAdd);
	return;
}

bool	Channel::IsUserInvited(User *toCheck)
{
	if (this->_modes.find('i') == std::string::npos)
	{
		return (true);
	}
	else
	{
		for (std::vector<User *>::iterator it = this->_invited.begin(); it != this->_invited.end(); it++)
		{
			User *user = *it;
			if (user->GetNickname() == toCheck->GetNickname())
				return (true);
		}
		return (false);
	}
}

void	Channel::SetModes(std::string modes)
{
	std::string	modes_available = "it";
	for (size_t i = 0; i < modes.size(); i++)
	{
		if (modes_available.find(modes[i]) != std::string::npos)
			this->_modes += modes[i];
	}
	return ;
}


bool	Channel::HasUser(User *user)
{
	for (std::vector<User *>::iterator it = this->_users.begin();
		it != this->_users.end(); it++)
	{
		User *usertmp = *it;
		if (usertmp->GetNickname() == user->GetNickname())
			return (true);
	}
	return (false);
}
