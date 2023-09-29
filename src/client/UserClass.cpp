# include "irc.hpp"

User::User() : _isAuth(false)
{
	return ;
}

User::~User()
{
	return ;
}

void	User::JoinChannel(Channel *toJoin)
{
	this->_channels.push_back(toJoin);
	return ;
}

void	User::SetUsername(std::string username)
{
	this->_username = username;
	return ;
}

void	User::SetHostname(std::string hostname)
{
	this->_hostname = hostname;
	return ;
}

void	User::SetServername(std::string servername)
{
	this->_servername = servername;
	return ;
}

void	User::SetRealname(std::string forname, std::string name)
{
	std::string	realname;

	realname = forname.erase(0, 1); // on supprime les deux points
	realname += " " + name;
	this->_realname = realname;
}

bool	User::IsAvailableNickname(std::string nickname, Server *server)
{
	std::map<int, User*>::iterator	it = server->GetUsers().begin();
	std::map<int, User*>::iterator	ite = server->GetUsers().end();

	while (it != ite)
	{
		if (nickname == it->second->GetNickname())
			return (false);
		it++;
	}
	return (true);
}

void		User::SetNickname(std::string nickname, Server *server)
{
	if (IsAvailableNickname(nickname, server) == true)
		this->_nickname = nickname;
	else
		// error
	return ;
}

void	User::SetFd(int fd)
{
	this->_fd = fd;
	return ;
}

void	User::SetAuth(bool status)
{
	this->_isAuth = status;
}

bool	User::GetAuth()
{
	return (this->_isAuth);
}

int		User::GetFd()
{
	return (this->_fd);
}

std::string		User::GetNickname()
{
	return (this->_nickname);
}

std::string	User::GetUsername()
{
	return (this->_username);
}

std::string	User::GetHostname()
{
	return (this->_hostname);
}

std::string	User::GetServername()
{
	return (this->_servername);
}

std::string	User::GetRealname()
{
	return (this->_realname);
}