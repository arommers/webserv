#ifndef SERVER_TEST_HPP
# define SERVER_TEST_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>

class Server
{
    private:
        std::string			_serverName;
		std::vector<int>	_ports;
		std::string			_rootPath;

    public:
        Server();
        ~Server();

};

#endif