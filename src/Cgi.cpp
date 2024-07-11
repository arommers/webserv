#include "../includes/Cgi.hpp"

Cgi::Cgi()
{

}

Cgi::Cgi(const Cgi& rhs)
{

}

Cgi& Cgi::operator=(const Cgi& rhs)
{

}

Cgi::~Cgi()
{

}

void Cgi::createEnv(std::map<std::string, std::string> _requestMap )
{
    std::vector<std::string>    env_vec;

    env_vec.push_back("METHOD=" + _requestMap.at("Method"));
    env_vec.push_back("QUERY_STRING="); // ADD!


}