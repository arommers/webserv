#include "config.hpp"

// int	main(int argc, char **argv)
// {
// 	std::string		configFile = "config/server_file.conf";
// 	if (argc >= 2)
// 		configFile = argv[1];
//     try
//     {
//         Config config(configFile);
// 		// SOKETS here
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
//     return 0;
// }

int main()
{
    try
    {
        Config config("server_file.conf");
		// SOKETS here
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
