#include "paramHelper.h"
#include <sstream>
#include <regex>

void paramHelper::getParams(std::vector<std::string>& words, std::string s, const char* splitAt)
{
	char * pch = NULL;
	char *next = NULL;

	char * writable = new char[s.size() + 1];
	std::copy(s.begin(), s.end(), writable);
	writable[s.size()] = '\0';

	pch = strtok_s(writable, splitAt, &next);
	while(pch != NULL)
	{
		//printf("%s\n", pch);
		words.push_back(pch);
		pch = strtok_s(NULL, splitAt, &next);
	}

	//delete[] pch;
	delete[] writable;
}

int paramHelper::isInt(std::string s)
{
	try
	{
		return std::stoi(s, nullptr, 10);
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

unsigned long paramHelper::isUnsignedInt(std::string s, bool allowHex)
{
	try
	{
		return std::stoul(s, nullptr, (allowHex ? 0 : 10));
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

float paramHelper::isFloat(std::string s)
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

std::string paramHelper::reverseSplit(std::vector<std::string>& v, size_t start)
{
	size_t s = v.size();
	std::ostringstream str;
	for(start; start < s; ++start)
	{
		str << v.at(start);
		if(start + 1 < s)
		{
			str << " ";
		}
	}
	return str.str();
}

std::string paramHelper::DwordToString(int* list, int size)
{
	std::ostringstream s;
	for(int i = 0; i < size; i++)
	{
		s << (char)list[i];
		//LINFO << squery[i];
	}
	return s.str();
}