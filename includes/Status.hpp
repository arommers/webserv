#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>       // Input and output through streams
#include <algorithm>


class Status
{
    private:
		std::vector<int>						                      _statusCheck = {400, 403, 404, 405, 406, 409, 410, 413, 500, 504, 505};
		static const std::unordered_map<int, std::string>	_ReasonPhraseMap;
    public:
        Status();
        ~Status();
        
		    bool        detectError(int statusCode);
        std::string getStatusMessage( int statusCode );


};