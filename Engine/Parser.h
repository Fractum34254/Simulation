#pragma once
#include "PhilUtil.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

class Parser
{
public:
	static float CalculateRHS(std::string rhs_in, const std::unordered_map<std::string, float>& var)
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
			while (!IsOperator(c) && !rhs.eof())
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
		return AssembleVars(vars, ops, var);
	}
private:
	static float AssembleVars(std::vector<std::string> vars, std::vector<char> ops, const std::unordered_map<std::string, float>& var)
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
				varVals[i] = Calculate(ops.at(i), varVals.at(i), varVals.at(i+1));
				varVals.erase(varVals.begin() + i + 1);
				ops.erase(ops.begin() + i);
				i--;
			}
		}
		///now just line up the rest of the operators with the variable values
		float result = varVals.at(0);
		for (int i = 1; i < varVals.size(); i++)
		{
			result = Calculate(ops.at(i - 1), result, varVals.at(i));
		}
		return result;
	}
	static float Calculate(char op, float f1, float f2)
	{
		if (op == '+')
		{
			return f1 + f2;
		}
		else if (op == '-')
		{
			return f1 - f2;
		}
		else if (op == '*')
		{
			return f1 * f2;
		}
		else if (op == '/')
		{
			return f1 / f2;
		}
		else
		{
			throw std::exception("unregistered operator");
		}
	}
	static bool IsOperator(std::string op)
	{
		return (op == "+") || (op == "-") || (op == "*") || (op == "/");
	}
	static bool IsOperator(char op)
	{
		std::string s;
		s += op;
		return IsOperator(s);
	}
	static bool IsComparison(std::string comp)
	{
		return (comp == ">=") || (comp == "<=") || (comp == "<") || (comp == ">") || (comp == "=");
	}
};
