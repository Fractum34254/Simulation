#pragma once
#include <string>
#include <sstream>

namespace phil
{
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
	bool IsOperator(std::string op)
	{
		return (op == "+") || (op == "-") || (op == "*") || (op == "/");
	}
	bool IsOperator(char op)
	{
		std::string s;
		s += op;
		return IsOperator(s);
	}
	bool IsComparison(std::string comp)
	{
		return (comp == ">=") || (comp == "<=") || (comp == "<") || (comp == ">") || (comp == "=");
	}
}
