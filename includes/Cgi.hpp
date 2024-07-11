#pragma once

#include <iostream>
#include <vector>
#include <map>

class Cgi
{
    private:

    public:
            Cgi();
            Cgi(const Cgi& rhs);
            Cgi& operator=(const Cgi& rhs);
            ~Cgi();
    void    createEnv(std::map<std::string, std::string> _requestMap );
};