#pragma once
#include <string>
#include <sstream>

template<typename T>
std::string Crop(T in, int n)
	{
		std::stringstream s_in;
		s_in << in;
		std::string sstr = s_in.str();
		while (sstr.length() < n)
		{
			sstr += " ";
		}
		while (sstr.length() > n)
		{
			sstr.pop_back();
		}
		return sstr;
	}
bool isFloat(std::string myString) {
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f;	// noskipws considers leading whitespace invalid
								// Check the entire string was consumed and if either failbit or badbit is set
								// thx to: Bill the Lizard from stackoverflow.com
	return iss.eof() && !iss.fail();
}

