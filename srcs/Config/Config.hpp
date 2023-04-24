/* this file contains the definition of the Config class.
 * This class contains the details of the configuration of the web server.
 * The configuration comes from a config file that's passed as command line arg to the program
 */

#pragma once

#include <string>
#include <map>
#include <list>
#include <set>
#include <stdexcept>
#include <fstream>
#include <StatusCodeHandler.hpp>

// there are more include directives at the end of class definitions

class Config {

	public:
		/******* alias types *******/
		typedef std::string Path;
		typedef std::string Extension;
		typedef StatusCodeHandler::StatusCode StatusCode;
		typedef unsigned long Size;
		typedef std::map<Extension, Path> CGISystems;

		/******* nested types *******/
		// holds info about a given location
		struct LocationContext {
			Path route;
			// http methods
			bool get;
			bool post;
			bool del;
			// end of http methods
			std::map<Path, StatusCode> redirections;
			Path root;
			bool autoindex;
			Path defaultFile;
			CGISystems supportedCGIs;
			Path uploadRoute;

			// constructor
			LocationContext();
		};

		/******* alias types *******/
		typedef std::map<Path, LocationContext> LocationsCollection;
		typedef int Socket;

		/******* nested types *******/
		// holds information about a given server
		struct ServerContext {
			std::string hostname;
			std::string port;
			std::string server_name;
			Socket socketID;
			std::map<StatusCode, Path> errorPages;
			Size clientBodySizeMax;
			LocationsCollection locations;

			// Config sets hostname and port to these defaults
				// in case they were not provided in the config file
			const static std::string defaultHostname;
			const static std::string defaultPort;

			// constructor
			// initializes socketID to -1 and clientBodySizeMax to 0
			ServerContext();
		};

		/******* alias types *******/
		typedef std::list<ServerContext> Servers;
		typedef ServerContext& ServerRef;
		typedef const ServerContext& ConstServerRef;

		/******* public member functions *******/
		// sets up a configuration from a config file
		// throws std::invalid_argument exception if fileName is NULL
			// or cannot be opened
		Config(const char *fileName);

		Servers& getServers();

		bool isCGIExtensionSupported
			(const Extension& extension);

		// prints all the servers' configuration
		void print();

		// returns a server reference that has same socketID
		// if not found a std::invalid_argument is thrown
		ConstServerRef getServerRef(Socket socketID) const ;
	
	private:
		/******* private member objects *******/
		// contains extensions that are supported for CGI requests
		std::set<Extension> mSupportedCGIExtensions;

		// contains all the user-configured servers
		Servers mServers;

		// default path to configuration file
			// if a path was not specified
		const static std::string defaultConfigFileName;

		/******* private member functions *******/
		void initSupportedCGIExtensions();

		// prints a server block elements
		// indent specify amount of indentation before
			// printing each of the server's elements
		void printServer(const ServerContext& server, int indent);

		// same as above
		void printLocation(const LocationContext& location, int indent);

		// prints elements of directives that have key/value pairs
		// and represented as map like error_page and redirect
		// indent param same as above
		template <class Map>
		void printMap(const Map& elems, int indent);

};

// this include directive was brought down here because the included file
// includes this file (Config.hpp) as well and relies on the full definition
// of the Config class
#include <ConfigParser.hpp>
