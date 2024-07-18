#pragma once

#include <vector>
#include <string>

class Location
{
    private:
        std::string _path; // This relates to the part of the url thats being requested >> location "/" {}
        std::string _root; // Local pathing to what directory should be served
        std::string _index; // what specific file should be served
        std::vector<std::string> _allowedMethods;

    public:
        Location();
        Location(std::string& path, std::string& root, std::string& index, std::vector<std::string>& allowedMethods);
        Location(Location& rhs);
        Location& operator=(const Location& rhs);
        ~Location();

        std::string getPath() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::vector<std::string> getAllowedMethods() const;

        void setPath(std::string& path);
        void setRoot(std::string& root);
        void setIndex(std::string& index);
        void setAllowedMethods(std::vector<std::string>& methods);
};