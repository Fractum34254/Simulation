#pragma once
#include "PhilUtil.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

class Parser
{
	float CalculateRHS(std::string rhs_in, const std::unordered_map<std::string, float>& var)
	{
		std::vector<std::string> vars;
		std::vector<char> ops;
		std::istringstream rhs(rhs_in);

		//split up string in variables and operators
		while (!rhs.eof())
		{
			///variable string
			std::string s;
			///extracted character
			char c = rhs.get();
			while (!phil::IsOperator(c) && !rhs.eof())
			{
				if (c != ' ')
				{
					///ignore blank spaces
					s += c;
				}
				c = rhs.get();
			}
			///if c is an operator, s is a ful variable name
			vars.emplace_back(s);
			if (!rhs.eof()) {
				ops.emplace_back(c);
			}
		}

		//return assembled vectors
		return Assemble(vars, ops, var);
	}
private:
	float Assemble(std::vector<std::string> vars, std::vector<char> ops, const std::unordered_map<std::string, float>& var)
	{
		//vector of floats out of the variables
		std::vector<float> varVals;
		for (const std::string& v : vars)
		{
			varVals.emplace_back(var.at(v));
		}
		///first, calculating '*' and '/'
		for (int i = 0; i < ops.size(); i++)
		{
			if (ops.at(i) == '*' || ops.at(i) == '/')
			{
				varVals[i] = phil::Assemble(ops.at(i), varVals.at(i), varVals.at(++i));
				varVals.erase(varVals.begin() + i);
				ops.erase(ops.begin() + --i);
				i--;
			}
		}
		///now just line up the rest of the operators with the variable values
		float result = varVals.at(0);
		for (int i = 1; i < varVals.size(); i++)
		{
			result = phil::Assemble(ops.at(i - 1), result, varVals.at(i));
		}
		return result;
	}
};
