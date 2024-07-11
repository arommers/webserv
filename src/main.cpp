#include "Client.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "ServerInfo.hpp"

int main()
{
    int opt = 1;
    Server server;


    server.createServerSocket();
    setsockopt(server.getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    std::cout << GREEN << "Webserv started and listening on port: " << PORT << RESET << std::endl;
    server.createPollLoop();
    
    return 0;
}


// --- For later use ---
// more structured main
//
// /*	main();
//  *	- Checks for correct arguments.
//  *	- If no configuration file is passed (argc <= 2):
//  *		- It will use the default configuration file.
//  *	- Otherwise, it will use argv[1] (the file provided as an argument).
//  *	- Step 1: It will check the configuration file and parse all the arguments (Config(configFile)).
//  *	- Step 2: It will create the sockets and start the server for request/response.
//  *	- If something goes wrong, it will throw an exception.
//  */
// int	main(int argc, char **argv)
// {
// 	std::string		configFile = "config/config_file.conf";  // Default configFile

// 	// Check for the correct number of arguments
// 	if (argc <= 2)
// 	{
// 		// If there is a second argument, use it as the configFile
// 		// Otherwise, use the default configFile
// 		if (argc == 2)
// 			configFile = argv[1];
// 		try
// 		{
// 			Config config(configFile);	// Step 1:
// 			// SOKETS here				// Step 2:
// 		}
// 		catch (const std::exception &e)
// 		{
// 			std::cerr << "Error: " << e.what() << std::endl;
// 			return 1;
// 		}
// 	}
// 	else
// 	{
// 		std::cerr << "Error: wrong arguments " << std::endl;
// 		return 1;
// 	}
//     return 0;
// }