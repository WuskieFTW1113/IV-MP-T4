#include "API/apiParamHelper.h"
#include <sstream>
#include <regex>
#include <iostream>

int apiParamHelper::isInt(const char* s)
{
	try
	{
		return std::stoi(s, NULL, 10);
	}
	catch(const std::invalid_argument&)
	{
		return -1;
	}
	catch(const std::out_of_range&)
	{
		return -1;
	}
	return -1;
}

unsigned long apiParamHelper::isUnsignedInt(const char* s, bool allowHex)
{
	try
	{
		return std::stoul(s, NULL, (allowHex ? 0 : 10));
	}
	catch(const std::invalid_argument&)
	{
		return 0;
	}
	catch(const std::out_of_range&)
	{
		return 0;
	}
	return 0;
}

float apiParamHelper::isFloat(const char* s)
{
	try
	{
		std::string::size_type sz;
		return std::stof(s, &sz);
	}
	catch (const std::invalid_argument&)
	{
		return -1.0;
	}
	
	return -1.0;
}
