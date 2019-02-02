#pragma once
#include "PhilUtil.h"
#include "ChiliException.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

class Parser
{
public:
	class Exception : public ChiliException
	{
	public:
		using ChiliException::ChiliException;
		virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
		virtual std::wstring GetExceptionType() const override { return L"Parser Exception"; }
	};
public:
	static float CalculateRHS(std::string rhs_in, const std::unordered_map<std::string, float>& var, int line = 0)
	{
		std::vector<float> vars;
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
			///if c is an operator, s is a full variable name
			if (isFloat(s))
			{
				vars.emplace_back(std::stof(s));
			}
			else {
				if (var.find(s) != var.end())
				{
					vars.emplace_back(var.at(s));
				}
				else
				{
					std::string info = "Uninitialized variable in line ";	///standart-syntax
					info += line + 48;										///line number (+48 caused by ascii translation)
					info += ": ";
					info += s;												///uninitialized variable name
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
			}
			if (!rhs.eof()) {
				ops.emplace_back(c);
			}
		}
		//return assembled vectors
		return AssembleVars(vars, ops);
	}
private:
	static float AssembleVars(std::vector<float> varVals, std::vector<char> ops)
	{
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
	static bool isFloat(std::string myString) {
		std::istringstream iss(myString);
		float f;
		iss >> std::noskipws >> f;	// noskipws considers leading whitespace invalid
									// Check the entire string was consumed and if either failbit or badbit is set
									// thx to: Bill the Lizard from stackoverflow.com
		return iss.eof() && !iss.fail();
	}
	static std::wstring towstring(std::string s)
	{
		const char* pc = s.c_str();
		std::wstring ws;
		for (int i = 0; i < s.size(); i++)
		{
			ws += pc[i];
		}
		return ws;
	}
};
