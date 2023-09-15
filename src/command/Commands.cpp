#include "irc.hpp"

// void	Command::SendToChannel()
// {
	
// }

// void	Command::SendToUser(User *user, Server *server)
// {
// 	int	recipientFd = server->GetFdByNickName()

// 	if (recipientFd != -1) // si le user appartient bien au server
// 	{
// 		std::string	response = ":" + user.GetNickName() + " " + this->GetNumericReply() + " " + this->GetMsg() + "\r\n";
// 		send(recipientFd, response,response.lenght(), 0);
// 	}
// 	else // le user est inconnu
// 	{
// 		std::string	response = ":localhost 401" + user.getNickName() + " : No such NickName\r\n";
// 		send(user->GetFd(), response, response.length(), 0);
// 		std::cout << "response =>" << response << std::endl;
// 	}
// }

void	Command::PRIVMSG(int clientFd, User *user, Server *server)
{
	void(int);

	if (this->GetParameters()[0][0] == "#")
		this->SendToChannel();
	else 
		this->SendToUser();
}

// /!
// Remember to handle the case where the params string does not contain a space, which means 
// that the PRIVMSG command is missing required parameters. In this case,
//  you should return an appropriate error message to the user.

