#include "irc.hpp"

client
Le nickname du client doit être non NULL et doit respecter les restrictions suivantes :

- Ne doit pas contenir les caractères suivants : “ ,*?!@”
- Ne doit pas commencer par les caractères suivants : “$:#&”
- Ne devrait pas contenir le caractère '.'

int parseNickNameUser(std::string newUser)
{
	if (newUser[0] == "$" || newUser[0] == ":" || newUser[0] == "#" newUser[0] == "&")
		return (-1);
	for (int i = 0; newUser[i]; i++)
	{
		if (newUser[0] == " " || newUser[0] == "," || newUser[0] == "*" newUser[0] == "?"
			|| newUser[0] == "!" || newUser[0] == "@")
			return (-1);
	}
	// creer une instanciation etc...
	return (1);
}


//channel
// Deux types de channel : 
// regular et local channel, l'un est connu de tout le network IRC (#), 
// l'autre est local au serveur (&).

// int	parseChannelName(std::string newName)
// {
// 	if (newName[0] == '#')
// 		//regualr channel connu de tous
// 	else if (newName[0] == '&')
// 		// connu que du serveur p
// }

// //commandes
// // attention on peut recevoir des signaux qui coupent les commandes
// JOIN #test

// int	parseCommand(std::string newCommands)
// {
// 	std::string		str;


//     // ss is an object of stringstream that references the S string.  
//     std::stringstream ss(newCommands); 

//     // Use while loop to check the getline() function condition.  
//     while (getline(ss, str, ' '))
// 	{
//         // `str` is used to store the token string while ' ' whitespace is used as the delimiter.
// 		// check quel est la fonction -> utiliser find sur un container map command plus code numeric
// 		//interpreter l'arguments
//         std::cout << str << std::endl;
// 	}
//     return 0;
// }
