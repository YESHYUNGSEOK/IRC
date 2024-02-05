#include <iostream>

int main(int argc, char **argv) 
{
	if (argc === 3)
	{
		return EXIT_SUCCESS;
	}
	else
	{
		std::cout << "Correct usage is ./ircserv [port] [password]" << std::endl;
		return EXIT_FAILURE;
	}
}