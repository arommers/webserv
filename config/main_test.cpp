#include "config.hpp"

/*
	(Done)	• Choose the port and host of each ’server’.
	(Done)	• Setup the server_names or not.
	(Done)	• The first server for a host:port will be the default for this host:port (that means
			it will answer to all the requests that don’t belong to an other server).
	(Done)	• Setup default error pages.
	(?)		• Limit client body size.
	()		• Setup routes with one or multiple of the following rules/configuration (routes wont
			be using regexp):
	()			◦ Define a list of accepted HTTP methods for the route.
	(Done)		◦ Define a HTTP redirection. -> index.html / mainPage.html
	(Done)		◦ Define a directory or a file from where the file should be searched (for example,
				if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
				/tmp/www/pouic/toto/pouet).		-> Root
	()			◦ Turn on or off directory listing.
	()			◦ Set a default file to answer if the request is a directory.
	()			◦ Execute CGI based on certain file extension (for example .php).
	()			◦ Make it work with POST and GET methods.
	*/

/* 
 *	for testig run:
 *	----------------
 *	c++ main_test.cpp config.cpp serverInfo.cpp
 *	./a.out
*/



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

// Test main
int main()
{
    try
    {
        // Config config("config_file.conf");
		Config config("test.conf");
		// SOKETS here
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
