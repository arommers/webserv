#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>       // Input and output through streams
#include <algorithm>


class Error
{
    private:
		std::vector<int>						            _statusCheck = {400, 401, 404, 405, 500, 503, 504};
		static const std::unordered_map<int, std::string>	_ReasonPhraseMap;
    public:
        Error();
        ~Error();
        
		bool									detectError(int statusCode);
        std::string                             getStatusMessage( int statusCode );


};