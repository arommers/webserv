#pragma once

#include <vector>
#include <string>
#include <iostream>

// For directory listing (autoindex)
# define ON true
# define OFF false

class Location
{
	private:
		std::string					_path; 				// This relates to the part of the url thats being requested >> location "/" {}
		std::string					_root; 				// Local pathing to what directory should be served
		std::string					_index;				// what specific file should be served
		std::vector<std::string>	_allowedMethods;
		bool						_autoindex;
		std::string					_redir; 			// Redirect
		int							_redirStatusCode;	// Status code for the redirect

	public:
		Location();		// Constructor
		~Location() {}	// Destructor

		// Getter
		bool						getAutoindex() const;
		std::string					getPath() const;
		std::string 				getRoot() const;
		std::string 				getIndex() const;
		std::string					getRedir() const;
		std::vector<std::string>	getAllowedMethods() const;
		int							getRedirStatusCode() const;

		// Setter
		void						setPath(const std::string &path);
		void						setRoot(const std::string &root);
		void						setIndex(const std::string &index);
		void						setAllowedMethods(const std::vector<std::string> &methods);
		void						setAutoindex(bool autoindex);
		void						setRedir(const std::string &redir);
		void						setRedirStatusCode(const int &statusCode);
};