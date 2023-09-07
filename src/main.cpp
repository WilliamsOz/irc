#include "irc.hpp"

volatile sig_atomic_t g_signal = false;

int main()
{
    char pass[] = "mdp";

	Server server(6667, pass);
    server.LaunchServer();
}