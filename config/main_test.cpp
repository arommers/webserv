#include "config.hpp"

int main()
{
    try
    {
        Config config("server_file.conf");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}