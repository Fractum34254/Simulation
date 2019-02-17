#pragma once
#include "PhilUtil.h"
#include "ChiliException.h"
#include <algorithm>
#include <functional>
#include <cmath>
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
	Parser()
	{
		operate["-"] = [](float f1, float f2) { return f1 - f2; };
		operate["+"] = [](float f1, float f2) { return f1 + f2; };
		operate["*"] = [](float f1, float f2) { return f1 * f2; };
		operate["/"] = [](float f1, float f2) { return f1 / f2; };
		compare[">"] = [](float f1, float f2) { return f1 > f2; };
		compare[">="] = [](float f1, float f2) { return f1 >= f2; };
		compare["<"] = [](float f1, float f2) { return f1 < f2; };
		compare["<="] = [](float f1, float f2) { return f1 <= f2; };
		compare["=="] = [](float f1, float f2) { return f1 == f2; };
		compare["!="] = [](float f1, float f2) { return f1 != f2; };
	}
	void Calculate(std::string term_in, std::unordered_map<std::string, float>& vars, int line = 0)
	{
		std::string begin;
		///test for blank spaces
		int i = 0;
		char cOut = term_in.at(i);
		while (cOut == ' ')
		{
			cOut = term_in.at(++i);
		}
		begin += term_in.at(i++);
		begin += term_in.at(i);
		///test for commentary
		if (begin == "//")
		{
			return;
		}
		///test for if statement
		if (begin == "if")
		{
			std::string brace;
			std::string op;
			std::istringstream term(term_in);
			///going foward to the brace after 'if'
			for (char c = term.get(); c != 'i'; c = term.get());
			char c = term.get();

			c = term.get();
			c = term.get();
			bool ended = false;
			while (c != ')' && !ended)
			{
				if (term.eof())
				{
					std::string info = "Uncompleted if statement in line ";	///standart-syntax
					info += line + 48;										///line number (+48 caused by ascii translation)
					info += ":\n";
					info += term_in;
					info += "\n(missing comparison operator and end brace for test)";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				brace += c;
				///test for wider comparator
				if (IsComparator(c))
				{
					std::string posOp;
					posOp += c;
					posOp += term.get();
					term.unget();
					if (IsComparator(posOp))
					{
						brace += term.get();
					}
				}
				///search for operator
				if (std::any_of(compare.begin(), compare.end(),
					[&brace, &op](const std::pair<const std::string, std::function<bool(float f1, float f2)>>& pair)
						{
							const size_t length = std::min(pair.first.size(), brace.size());
							std::string end;
							for (size_t i = 0; i < length; i++)
							{
								end += brace.at(brace.size() - length + i);
							}
							if (end == pair.first)
							{
								op = pair.first;
								return true;
							}
							return false;
						}))
				{
					const size_t length = op.size();
					///eliminate operator from string
					for (size_t i = 0; i < length; i++)
					{
						brace.pop_back();
					}
					ended = true;
				}
				c = term.get();
			}
			if (!ended)
			{
				std::string info = "Uncompleted if statement in line ";	///standart-syntax
				info += line + 48;										///line number (+48 caused by ascii translation)
				info += ":\n";
				info += term_in;
				info += "\n(missing comparison operator for test)";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			std::string rightSide;
			c = term.get();
			while (c != ')')
			{
				if (term.eof())
				{
					std::string info = "Uncompleted if statement in line ";	///standart-syntax
					info += line + 48;										///line number (+48 caused by ascii translation)
					info += ":\n";
					info += term_in;
					info += "\n(missing end brace for test)";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				rightSide += c;
				c = term.get();
			}
			///brace:		holds lhs
			///rightSide:	holds rhs
			///op:			holds operator
			if (compare.at(op)(CalculateRHS(brace, vars, line), CalculateRHS(rightSide, vars, line)))
			{
				std::string calculation;
				c = term.get();
				while (!term.eof())
				{
					calculation += c;
					c = term.get();
				}
				Calculate(calculation, vars, line);
			}
		}
		///no if statement
		else
		{
			///every mathematical expression needs an assignment operator
			if (term_in.find('=') == term_in.npos)
			{
				std::string info = "Expression without '=' in line ";	///standart-syntax
				info += line + 48;										///line number (+48 caused by ascii translation)
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			std::istringstream term(term_in);
			std::string lval;
			std::string rval;
			for (char c = term.get(); c != '='; c = term.get())
			{
				if (c != ' ')
				{
					lval += c;
				}
			}
			for (char c = term.get(); !term.eof(); c = term.get())
			{
				rval += c;
			}
			vars[lval] = CalculateRHS(rval, vars, line);
		}
	}
private:
	std::unordered_map<std::string, std::function<float(float f1, float f2)>> operate;
	std::unordered_map<std::string, std::function<bool(float f1, float f2)>> compare;
	float CalculateRHS(std::string rhs_in, const std::unordered_map<std::string, float>& var, int line = 0) const
	{
		std::vector<float> vars;
		std::vector<std::string> ops;
		std::istringstream rhs(rhs_in);

		//split up string in variable(value)s and operators
		while (!rhs.eof())
		{
			///variable string
			std::string s;
			///extracted character
			char c = rhs.get();
			while (!IsOperator(c) && !rhs.eof() && c != '(')
			{
				if (c != ' ')
				{
					///ignore blank spaces
					s += c;
				}
				c = rhs.get();
			}
			if(c == '(') ///c is a brace
			{ 
				std::string brace;
				for (c = rhs.get(); c != ')'; c = rhs.get())
				{
					brace += c;
				}
				///test for root before brace
				if (s == "sqrt")
				{
					vars.emplace_back(std::sqrtf(CalculateRHS(brace, var, line)));
				}
				else if (s == "sin")
				{
					vars.emplace_back(std::sin(CalculateRHS(brace, var, line)));
				}
				else if (s == "cos")
				{
					vars.emplace_back(std::cos(CalculateRHS(brace, var, line)));
				}
				else if (s == "tan")
				{
					vars.emplace_back(std::tan(CalculateRHS(brace, var, line)));
				}
				else
				{
					vars.emplace_back(CalculateRHS(brace, var, line));
				}
				///test if string ends after brace
				c = rhs.get();
				if (!rhs.eof())
				{
					///if it does not end, put character back
					rhs.unget();
				}
			}
			else ///c is not a brace
			{
				///if c is an operator, s is a full variable name
				if (isFloat(s))
				{
					vars.emplace_back(std::stof(s));
				}
				else {
					///if there was a brace, there is no variable -> vars are greater than ops
					if (var.find(s) != var.end())
					{
						vars.emplace_back(var.at(s));
					}
					else if(!(vars.size() > ops.size()))
					{
						std::string info = "Uninitialized variable in line ";	///standart-syntax
						info += line + 48;										///line number (+48 caused by ascii translation)
						info += ": ";
						info += s;												///uninitialized variable name
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
					}
				}
				if (!rhs.eof()) {
					std::string s;
					s += c;
					ops.emplace_back(s);
				}
			}
		}
		//return assembled vectors
		return AssembleVars(vars, ops, line);
	}
	float AssembleVars(std::vector<float> varVals, std::vector<std::string> ops, int line) const
	{
		if (ops.size() + 1 != varVals.size())
		{
			std::string info = "Unmatching operators, variables and numbers in line ";	///standart-syntax
			info += line + 48;															///line number (+48 caused by ascii translation)									///uninitialized variable name
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
		///first, calculating '*' and '/'
		for (int i = 0; i < ops.size(); i++)
		{
			if (ops.at(i) == "*" || ops.at(i) == "/")
			{
				varVals[i] = CalculateValue(ops.at(i), varVals.at(i), varVals.at(i+1));
				varVals.erase(varVals.begin() + i + 1);
				ops.erase(ops.begin() + i);
				i--;
			}
		}
		///now just line up the rest of the operators with the variable values
		float result = varVals.at(0);
		for (int i = 1; i < varVals.size(); i++)
		{
			result = CalculateValue(ops.at(i - 1), result, varVals.at(i));
		}
		return result;
	}
	float CalculateValue(std::string op, float f1, float f2) const
	{
		try {
			return operate.at(op)(f1, f2);
		}
		catch (const std::exception&)
		{
			std::string info = "Unknown operator: ";	
			info += op;							
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
	}
	bool IsComparator(const char c)
	{
		std::string s;
		s += c;
		return IsComparator(s);
	}
	bool IsComparator(const std::string s)
	{
		return compare.find(s) != compare.end();
	}
	bool IsOperator(char c) const
	{
		std::string s;
		s += c;
		return operate.find(s) != operate.end();
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
