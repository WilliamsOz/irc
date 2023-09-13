#include "irc.hpp"

Command::Command()
{
	return ;
}

Command::~Command()
{
	return ;
}

std::string	Command::GetParam(int which)
{
	switch(which)
	{
		case 1:
			return (this->_param[0]);
			break;
		case 2:
			return (this->_param[1]);
			break;
		case 3:
			return (this->_param[2]);
			break;
		case 4:
			return (this->_param[3]);
			break;
	}
}

void	Command::USER(User user)
{
	user.SetUsername(this->GetParam(1));
	user.SetHostname(this->GetParam(2));
	user.SetServername(this->GetParam(3));
	user.SetRealname(this->GetParam(4));
}