#include "../includes/Status.hpp"

const std::unordered_map<int, std::string> Error::_ReasonPhraseMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
    {400, "Bad Formatting"},
    {401, "Unauthorized"},
    {403, "Unautorized"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"},
	{504, "Gateway Timeout"},
};

Error::Error(){};

Error::~Error(){};

bool	Error::detectError(int statusCode)
{
	if (std::find(_statusCheck.begin(), _statusCheck.end(), statusCode) != _statusCheck.end())
		return (true);
	return (false);
}

std::string Error::getStatusMessage( int statusCode )
{
    return (_ReasonPhraseMap.at(statusCode));
}
