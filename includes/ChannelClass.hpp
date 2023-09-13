#ifndef CHANNELCLASS_HPP
# define CHANNELCLASS_HPP

class Channel
{
	public:
	Channel();
	~Channel();

	private:
	std::vector<std::string>	_clients;
	std::string					_mode;
	// reference de tout les users present dans le channel
};

#endif