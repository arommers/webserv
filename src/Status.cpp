#include "../includes/Status.hpp"

const std::unordered_map<int, std::string> Status::_ReasonPhraseMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
    {400, "Bad Formatting"},
    {403, "Unautorized"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {409, "Conflict"},
    {410, "Gone"},
    {413, "Request Entity Too Large"},
    {500, "Internal Server Error"},
	{504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
};

const std::unordered_map<std::string, std::string> Status::_contentTypes = {
{".html",   "text/html"},
{".css",    "text/css"},
{".js",	    "application/javascript"},
{".json",	"application/json"},
{".png",	"image/png"},
{".jpg", 	"image/jpeg"},
{".jpeg",   "image/jpeg"},
{".gif",	"image/gif"},
{".txt",	"text/plain"},
{".pdf",	"application/pdf"},
{".zip",	"application/zip"},
{".svg",    "image/svg+xml"}
};

Status::Status(){};

Status::~Status(){};

bool	Status::detectError(int statusCode)
{
	if (std::find(_statusCheck.begin(), _statusCheck.end(), statusCode) != _statusCheck.end())
		return (true);
	return (false);
}

std::string Status::getStatusMessage( int statusCode )
{
    return (_ReasonPhraseMap.at(statusCode));
}

ContentTypeMap  Status::getContentTypes( void )
{
    return (_contentTypes);
}
