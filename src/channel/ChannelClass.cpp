# include "irc.hpp"

Channel::Channel() :  _password(""), _modes(""), _topic("")
{
	return ;
}

Channel::Channel(const std::string name) : _password(""), _name(name), _modes(""), _topic("")
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

void	Channel::DelOper(User *toDel)
{
	User *user = NULL;

	for (std::vector<User *>::iterator it = this->_opers.begin(); it != this->_opers.end(); it++)
	{
		user = *it;
		if (toDel->GetNickname() == user->GetNickname())
		{
			this->_opers.erase(it);
			return ;
		}
	}
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

void	Channel::SetModes(int mode, std::string param, int *j, Server *server)
{
	if (mode != 'o')
		if (!_modes.find(mode))
			_modes += mode;
	if (mode == 'k')
	{
		_password = param;
		(*j)++;
	}
	else if (mode == 'o')
	{
		User	*newOper = server->GetUserByNickname(param);

		if (!IsOper(newOper))
			AddOper(newOper);
		(*j)++;
	}
	else if (mode == 'l')
	{
		_limit = atoi(param.c_str());
		(*j)++;
	}
}

void	Channel::UnsetModes(int mode, std::string param, int *j, Server *server)
{
	size_t	i = _modes.find(mode);

	if (mode != 'o')
		if (i != std::string::npos)
			_modes.erase(i, 1);
	if (mode == 'k')
		_password = "";
	else if (mode == 'o')
	{
		User	*toDel = server->GetUserByNickname(param);

		if (!IsOper(toDel))
			DelOper(toDel);
		(*j)++;
	}
	else if (mode == 'l')
		_limit = INT_MAX;
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
