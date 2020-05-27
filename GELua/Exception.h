#ifndef INCLUDE_LUA_EXCEPTION_HPP
#define INCLUDE_LUA_EXCEPTION_HPP

#include <stdexcept>

/**
 * Thrown when an error happens during execution of lua code (like not enough parameters for a function)
 */
class ExecutionErrorException : public std::runtime_error
{
public:
	ExecutionErrorException(const std::string& msg) :
		std::runtime_error(msg)
	{
	}
};

/**
 * Thrown when a syntax error happens in a lua script
 */
class SyntaxErrorException : public std::runtime_error
{
public:
	SyntaxErrorException(const std::string& msg) :
		std::runtime_error(msg)
	{
	}
};

/**
 * Thrown when trying to cast a Lua variable to an unvalid type, eg. trying to read a number when the variable is a string
 */
class WrongTypeException : public std::runtime_error
{
public:
	WrongTypeException(std::string luaType, const std::type_info& destination) :
		std::runtime_error("Trying to cast a lua variable from \"" + luaType + "\" to \"" + destination.name() + "\""),
		luaType(luaType),
		destination(destination)
	{
	}
	
	std::string luaType;
	const std::type_info& destination;
};

#endif
