#pragma once

#include <vector>
#include <string>

// For directory listing (autoindex)
# define ON "true"
# define OFF "false"

class Location
{
    private:
        std::string					_path; // This relates to the part of the url thats being requested >> location "/" {}
        std::string					_root; // Local pathing to what directory should be served
        std::string					_index; // what specific file should be served
        std::vector<std::string>	_allowedMethods;
		bool						_autoindex;

    public:
        Location();		// Constructor
        ~Location() {}	// Destructor

		// Setter
        std::string getPath() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::vector<std::string> getAllowedMethods() const;
		bool getAutoindex() const;

		// Getter
        void setPath(std::string& path);
        void setRoot(std::string& root);
        void setIndex(std::string& index);
        void setAllowedMethods(std::vector<std::string>& methods);
		void setAutoindex(bool autoindex);
};