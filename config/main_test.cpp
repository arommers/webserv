#include "config.hpp"

// int	main(int argc, char **argv)
// {
// 	std::string		configFile = "config/server_file.conf";  // Default configFile

// 	// Checking for the correct aruments
// 	if (argc <= 2)
// 	{
// 		// If there is a second argument use that argument/file as the configFile
// 		// Else it will use the default configFile
// 		if (argc == 2)
// 			configFile = argv[1];
// 		try
// 		{
// 			Config config(configFile);
// 			// SOKETS here
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

int main()
{
    try
    {
        // Config config("server_file.conf");
		Config config("test.conf");
		// SOKETS here
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
