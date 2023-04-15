/* this file contains the implementation of the ConfigParser class */

#include <ConfigParser.hpp>

ConfigParser::ConfigParser(std::istream& input, Config& config)
	: mConfig(config)
	, mServers(config.getServers())
	, mLexer(input) {
	
	parseGlobal();

}

void ConfigParser::parseGlobal() {

	// searches for server blocks
	Token token = mLexer.next();
	while (token.type != Token::EOS) {
		isToken(token, Token::SRV_BLK);
		parseServer();
		token = mLexer.next();
	}

}

void ConfigParser::parseServer() {

	Token token = mLexer.next();
	// needs a left brace immediately after a server context directive
	isLeftBrace(token);

	// creates new server structure to contain the data within this
		// context
	addNewServer();
	updateServerRef();

	// enters server's context and parses directives within it
	token = mLexer.next();
	// while there are more tokens and a right curly brace hasn't been found
	while (token.type != Token::EOS
		&& token.type != Token::RB) {
		
		switch(token.type) {
			case Token::SRV_NAME:
				parseServerName();
				break;
			case Token::LISTEN:
				parseListen();
				break;
			case Token::ERR_PAGE:
				parseErrorPage();
				break;
			case Token::CLIENT_MAX:
				parseClientBodySizeMax();
				break;
			default:
				handleParsingError(token);
		}
		token = mLexer.next();

	}

	// makes sure server context is closed with a right brace
	isRightBrace(token);

	defaultInitUnfilledServerFields();

}

void parseLocation();

void ConfigParser::handleParsingError(const Token& lastToken) {

	mServers.clear();

	std::string error = "Parsing error: unexpected token '";
	if (lastToken.value.empty() == false)
		error += lastToken.value;
	else
		error += "EOF";
	error += '\'';

	throw std::runtime_error(error);

}

void ConfigParser::updateServerRef() {

	if (!mServers.empty())
		mServerRef = --mServers.end();

}

void updateLocationRef() {

void ConfigParser::addNewServer() {

	ServerContext tmp;
	mServers.push_back(tmp);

}

void addNewLocation() {

void ConfigParser::isToken(const Token& token, Token::Type type) {

	if (token.type != type)
		handleParsingError(token);

}

void ConfigParser::isLeftBrace(const Token& token) {
	isToken(token, Token::LB);
}

void ConfigParser::isRightBrace(const Token& token) {
	isToken(token, Token::RB);
}

void ConfigParser::isSemiColon(const Token& token) {
	isToken(token, Token::SM_COL);
}

void ConfigParser::isNum(const Token& token) {
	isToken(token, Token::NUM);
}

void ConfigParser::isNotAKeyword(const Token& token) {

	switch (token.type) {
		case Token::SRV_BLK:
		case Token::SRV_NAME:
		case Token::LISTEN:
		case Token::ERR_PAGE:
		case Token::CLIENT_MAX:
		case Token::LOC:
		case Token::ALLOW:
		case Token::RDR:
		case Token::ROOT:
		case Token::AUTOIN:
		case Token::DFLT:
		case Token::CGI:
		case Token::UPLOAD:
		case Token::LB:
		case Token::RB:
		case Token::SM_COL:
			handleParsingError(token);
		default:
			return;
	}

}

void ConfigParser::isNotEOS(const Token& token) {

	if (token.type == Token::EOS)
		handleParsingError(token);

}

void defaultInitUnfilledServerFields() {

void defaultInitUnfilledLocationFields() {

bool ConfigParser::isStrNumerical(const std::string& str) {
	
	for (std::string::const_iterator it = str.begin();
		it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return false;
	
	return true;

}

void ConfigParser::parseServerName() {

	Token token = mLexer.next();

	// checks if token is a valid name
	isNotEOS(token);
	isNotAKeyword(token);

	mServerRef->server_name = token.value;

	token = mLexer.next();

	// makes sure directive ends in a semi-colon
	// this applies to all directives
	isSemiColon(token);

}

void ConfigParser::parseListen() {

	Token token = mLexer.next();

	// checks if token is a valid string
	isNotEOS(token);
	isNotAKeyword(token);

	// makes sure target fields are empty
	mServerRef->hostname.clear();
	mServerRef->port.clear();

	// position of the ':' that separate hostname and port
	std::string::size_type colonPos = token.value.find(':');
	// if a colon was found
	if (colonPos != std::string::npos) {
		// makes sure that colon is in the middle
		if (colonPos == 0 || colonPos == token.value.size() - 1)
			handleParsingError(token);
		// sets hostname to string before colon
		mServerRef->hostname = token.value.substr(0, colonPos);
		// sets port to string after colon
		mServerRef->port = token.value.substr
			(colonPos + 1, token.value.size() - (colonPos + 1));
	}
	// it's a port value then
	else if (isStrNumerical(token.value))
		mServerRef->port = token.value;
	// otherwise it's a hostname
	else
		mServerRef->hostname = token.value;

	token = mLexer.next();
	isSemiColon(token);

}
		
void ConfigParser::parseErrorPage() {

	// set status code classes that are supported by the error_page directive
	std::vector<StatusCodeClass> supportedClasses(2);
	supportedClasses[0] = 4;
	supportedClasses[1] = 5;

	parseStatusCodeDirective(supportedClasses, mServerRef->errorPages);

}

void parseClientBodySizeMax() {

void ConfigParser::parseStatusCodeDirective
	(const std::vector<StatusCodeClass>& statusCodeClasses,
	StatusCodesWithPaths& saveStructure) {
	
	Token token = mLexer.next();
	// this token must be a status code,
		// so it makes sense to have a NUM type
	isNum(token);

	if (isStatusCodeValid(statusCodeClasses, token.value))
		handleParsingError(token);

	// converts token's value to StatusCode type value
	StatusCode code = convertStrToNumber<StatusCode>(token.value);

	// checks that next token is appropriate for a path/route value
	token = mLexer.next();
	isNotEOS(token);
	isNotAKeyword(token);

	// adds leading slash if missing
	checkPathLeadingSlash(token.value);

	// creates a pair of status code and path
	saveStructure[code] = token.value;

	token = mLexer.next();
	isSemiColon(token);

}
bool ConfigParser::isStatusCodeValid
	(const std::vector<StatusCodeClass>& statusCodeClasses,
	const std::string& statusCodeStr) {

	// checks if the token's value matches any of the status code classes
		// and that the status code supplied (token's value) has exactly 3 digits
	if (!isStatusCodeMatchClasses(statusCodeClasses, statusCodeStr)
		|| statusCodeStr.length() != 3) {

		if (statusCodeStr.length() != 3)
			std::cerr << "status code needs to have exactly 3 digits\n";
		else {
			std::cerr << "status code can only be one of these classes: ";
			for (size_t i = 0; i < statusCodeClasses.size(); ++i) {
				// convert form  StatusCodeClass type to char type
				std::cerr << statusCodeClasses[i] + '0' << "xx";
				// if there are more elements
				if (i + 1 < statusCodeClasses.size())
					std::cerr << ", ";
			}
			std::cerr << '\n';
		}
		return false;

	}
	return true;

}

bool ConfigParser::isStatusCodeMatchClasses
	(const std::vector<StatusCodeClass>& statusCodeClasses,
	const std::string& statusCodeStr) {

	if (statusCodeStr.empty())
		return false;

	bool classMatched = false;
	for (std::vector<StatusCodeClass>::const_iterator it = statusCodeClasses.begin();
		it != statusCodeClasses.end(); ++it) {
		// status code class is given as a single digit int,
			// so it is converted to its digit char equivalent so
			// that it's compared against the first character of the
			// status code string
		if ((*it + '0') == statusCodeStr[0]) {
			classMatched = true;
			break;
		}

	}
	return classMatched;

}

template<class Number>
Number ConfigParser::convertStrToNumber(const std::string& str) {

	std::stringstream converter(str);
	Number result;
	converter >> result;

	if (converter.fail())
		throw std::runtime_error("fatal error: couldn't convert string to number");
	return result;

}

void ConfigParser::checkPathLeadingSlash(std::string& path) {
	
	if (path.empty()) {
		path += '/';
		return ;
	}

	if (path[0] != '/')
		// prepends one slash to path
		path.insert(0, 1, '/');

}
