# include "irc.hpp"

Channel::Channel() :  _password(""), _modes(""), _topic("")
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
	std::cout << &this->_users.at(0) << std::endl;
	std::cout << this->_users.at(0)->GetNickname() << std::endl;
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

std::string Channel::IntToString(int number)
{
    std::ostringstream	oss;

    oss << number;
    return oss.str();
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

void	Channel::SetModes(int mode, std::stack<std::string> *modeParams, Server *server, Command *cmd, User *user)
{
	std::string	availableModes = "itkol";
	std::string	needParam = "kol";
	
	if (availableModes.find(mode) == std::string::npos)
	{
		cmd->SendMsgToClient(user, ERR_UMODEUNKNOWNFLAG(user->GetNickname()));
		return ;
	}
	if (needParam.find(mode) != std::string::npos && modeParams->top().empty() == true) // on ignore la commande si le param est manquant
		return ; 
	if (mode != 'o')
		if (!_modes.find(mode))
		{
			_modes += mode; // le mode est ajouté a la liste de mode du canal
			std::vector<User *>::iterator	it = this->_users.begin();
			std::vector<User *>::iterator	ite = this->_users.end();
			while (it != ite) // tous les membres du canal sont informés du changement de mode
				cmd->SendMsgToClient(user, SET_CHANEL_MODE(user->GetNickname(), user->GetUsername(), cmd->GetCmdName(), _name, IntToString(mode)));
		}
	if (mode == 'k')
	{
		_password = modeParams->top();
		modeParams->pop(); // on pop les parametres une fois utilisés
	}
	else if (mode == 'o')
	{
		User	*newOper = server->GetUserByNickname(modeParams->top());

		if (!IsOper(newOper))
			AddOper(newOper);
		std::vector<User *>::iterator	it = this->_users.begin();
		std::vector<User *>::iterator	ite = this->_users.end();
			while (it != ite)
				cmd->SendMsgToClient(user, SET_NEWOPER(user->GetNickname(), user->GetUsername(), cmd->GetCmdName(), _name, IntToString(mode), modeParams->top()));
		modeParams->pop();
	}
	else if (mode == 'l')
	{
		_limit = atoi(modeParams->top().c_str());
		modeParams->pop();
	}
}

void	Channel::UnsetModes(int mode, std::stack<std::string> *modeParams, Server *server, Command *cmd, User *user)
{
	std::string	availableModes = "itkol";
	
	if (availableModes.find(mode) == std::string::npos)
	{
		cmd->SendMsgToClient(user, ERR_UMODEUNKNOWNFLAG(user->GetNickname()));
		return ;
	}
	size_t	i = _modes.find(mode);

	if (mode != 'o')
		if (i != std::string::npos)
		{
			_modes.erase(i, 1);
			std::vector<User *>::iterator	it = this->_users.begin();
			std::vector<User *>::iterator	ite = this->_users.end();

			while (it != ite)
				cmd->SendMsgToClient(user, UNSET_CHANEL_MODE(user->GetNickname(), user->GetUsername(), cmd->GetCmdName(), _name, IntToString(mode)));
		}
	if (mode == 'k')
		_password = "";
	else if (mode == 'o')
	{
		if (modeParams->top().empty())
			return; // fail en silence
		User	*toDel = server->GetUserByNickname(modeParams->top());

		if (!IsOper(toDel))
			DelOper(toDel);
		std::vector<User *>::iterator	it = this->_users.begin();
		std::vector<User *>::iterator	ite = this->_users.end();

			while (it != ite)
				cmd->SendMsgToClient(user, UNSET_OPER(user->GetNickname(), user->GetUsername(), cmd->GetCmdName(), _name, IntToString(mode), modeParams->top()));
		modeParams->pop();
	}
	else if (mode == 'l')
		_limit = INT_MAX;
}

bool	Channel::HasUser(User *user)
{
	std::vector<User *>::iterator it = this->_users.begin();
	std::vector<User *>::iterator ite = this->_users.end(); 
	
	// std::cout << "so quoi?1\n";
	while (it != ite)
	{
		User *usertmp = *it;
		if (usertmp->GetNickname() == user->GetNickname())
			return (true);
		it++;
	}
	// std::cout << "so quoi?2\n";
	return (false);
}
