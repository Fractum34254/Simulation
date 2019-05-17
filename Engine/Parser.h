#pragma once
#include "ChiliException.h"
#include "PhilUtil.h"
#include <algorithm>
#include <random>
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
		:
		rng(std::random_device()())
	{
		operate["-"] = [](float f1, float f2) { return f1 - f2; };
		operate["+"] = [](float f1, float f2) { return f1 + f2; };
		operate["*"] = [](float f1, float f2) { return f1 * f2; };
		operate["/"] = [](float f1, float f2) { return f1 / f2; };
		operate["^"] = [](float f1, float f2) { return std::powf(f1, f2); };
		compare[">"] = [](float f1, float f2) { return f1 > f2; };
		compare[">="] = [](float f1, float f2) { return f1 >= f2; };
		compare["<"] = [](float f1, float f2) { return f1 < f2; };
		compare["<="] = [](float f1, float f2) { return f1 <= f2; };
		compare["=="] = [](float f1, float f2) { return f1 == f2; };
		compare["!="] = [](float f1, float f2) { return f1 != f2; };
	}
	void Calculate(std::string term_in, std::unordered_map<std::string, float>& vars, std::string fileName, int line = 0)
	{
		try
		{
			std::string begin;
			if (term_in.empty())
			{
				return;
			}
			///test for blank spaces
			int i = 0;
			char cOut = term_in.at(i);
			while (cOut == ' ')
			{
				if (term_in.size() == ++i)
				{
					return;
				}
				cOut = term_in.at(i);
			}
			begin += term_in.at(i++);
			begin += term_in.at(i);
			///test for commentary -> immediate return
			if (begin == "//")
			{
				return;
			}
			///test for if statement
			if (begin == "if")
			{
				int braces = 0;
				std::string brace;
				std::string op;
				std::istringstream term(term_in);
				///going forward to the brace after 'if'
				for (char c = term.get(); c != '('; c = term.get());
				char c = term.get();
				bool ended = false;
				while (((c != ')') || (braces != 0)) && !ended)
				{
					if (term.eof())
					{
						std::string info = "File \"";
						info += fileName;
						info += "\": Uncompleted if statement in line ";	///standart-syntax
						info += line + 48;										///line number (+48 caused by ascii translation)
						info += ":\n";
						info += term_in;
						info += "\n(missing comparison operator and end brace for test)\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
					}
					brace += c;
					///test for another brace opening
					if (c == '(')
					{
						braces++;
					}
					else if (c == ')')
					{
						braces--;
					}
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
					std::string info = "File \"";
					info += fileName;
					info += "\": Uncompleted if statement in line ";	///standart-syntax
					info += line + 48;										///line number (+48 caused by ascii translation)
					info += ":\n";
					info += term_in;
					info += "\n(missing comparison operator for test)\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
				}
				std::string rightSide;
				while (c != ')')
				{
					if (term.eof())
					{
						std::string info = "File \"";
						info += fileName;
						info += "\": Uncompleted if statement in line ";	///standart-syntax
						info += line + 48;										///line number (+48 caused by ascii translation)
						info += ":\n";
						info += term_in;
						info += "\n(missing end brace for test)\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
					}
					rightSide += c;
					c = term.get();
				}
				///brace:		holds lhs
				///rightSide:	holds rhs
				///op:			holds operator
				if (compare.at(op)(CalculateRHS(brace, vars, fileName, line), CalculateRHS(rightSide, vars, fileName, line)))
				{
					std::string calculation;
					c = term.get();
					while (!term.eof())
					{
						calculation += c;
						c = term.get();
					}
					Calculate(calculation, vars, fileName, line);
				}
			}
			///no if statement
			else
			{
				///every mathematical expression needs an assignment operator
				if (term_in.find('=') == term_in.npos)
				{
					std::string info = "File \"";
					info += fileName;
					info += "\": Expression without '=' in line ";			///standart-syntax
					info += line + 48;										///line number (+48 caused by ascii translation)
					info += "\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
				vars[lval] = CalculateRHS(rval, vars, fileName, line);
			}
		}
		catch (const Exception& e)
		{
			throw e;
		}
		catch (const std::exception& e)
		{
			const char* textCStr = e.what();
			const std::string textStr(textCStr);
			std::string info = "Something went wrong calculating the file \"";
			info += fileName;
			info += "\" in line ";
			info += line;
			info += ":\nCaught a std::exception:\n\n";
			info += textStr;
			info += "\n";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
		}
		catch (...)
		{
			std::string info = "Something went wrong calculating the file \"";
			info += fileName;
			info += "\" in line ";
			info += line;
			info += ":\nCaught a non-defined exception.\n";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
		}
	}
private:
	mutable std::mt19937 rng;
	std::unordered_map<std::string, std::function<float(float f1, float f2)>> operate;
	std::unordered_map<std::string, std::function<bool(float f1, float f2)>> compare;
	float CalculateRHS(std::string rhs_in, const std::unordered_map<std::string, float>& var, std::string fileName, int line = 0) const
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
				int braces = 0;
				std::string brace;
				for (c = rhs.get(); c != ')' || braces != 0; c = rhs.get())
				{
					///test for another brace opening
					if (c == '(')
					{
						braces++;
					}
					else if (c == ')')
					{
						braces--;
					}
					brace += c;
				}
				///test for special operants before brace
				if (s == "sqrt")
				{
					vars.emplace_back(std::sqrtf(CalculateRHS(brace, var, fileName, line)));
				}
				else if (s == "sin")
				{
					vars.emplace_back(std::sin(CalculateRHS(brace, var, fileName, line)));
				}
				else if (s == "cos")
				{
					vars.emplace_back(std::cos(CalculateRHS(brace, var, fileName, line)));
				}
				else if (s == "tan")
				{
					vars.emplace_back(std::tan(CalculateRHS(brace, var, fileName, line)));
				}
				else if (s == "abs")
				{
					vars.emplace_back(std::abs(CalculateRHS(brace, var, fileName, line)));
				}
				else if (s == "rnd")
				{
					std::uniform_real_distribution<float> dist(0.0f, CalculateRHS(brace, var, fileName, line));
					vars.emplace_back(dist(rng));
				}
				else
				{
					vars.emplace_back(CalculateRHS(brace, var, fileName, line));
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
				///if c is a '-', then it's possible that there was no variable or number in front
				if (c == '-' && s == "" && (vars.size() == ops.size()))
				{
					///now setting s to '-1' and c to '*' -> correct further calculation
					c = '*';
					s = "-1";
				}
				///if c is an operator, s is a full variable name
				if (PhilUtil::isFloat(s))
				{
					vars.emplace_back(std::stof(s));
				}
				else {
					///if there was a brace, there is no variable -> vars are greater than ops
					if (var.find(s) != var.end())
					{
						vars.emplace_back(var.at(s));
					}
					else if (s == "")
					{
						///do not put any string in the vars
					}
					else
					{
						std::string info = "File \"";
						info += fileName;
						info += "\": Uninitialized variable in line ";			///standart-syntax
						info += line + 48;										///line number (+48 caused by ascii translation)
						info += ": ";
						info += s;												///uninitialized variable name
						info += "\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
		return AssembleVars(vars, ops, fileName, line);
	}
	float AssembleVars(std::vector<float> varVals, std::vector<std::string> ops, std::string fileName, int line) const
	{
		if (ops.size() + 1 != varVals.size())
		{
			std::string info = "File \"";
			info += fileName;
			info += "\": Unmatching operators, variables and numbers in line ";	///standart-syntax
			info += line + 48;													///line number (+48 caused by ascii translation)	
			info += "\n";														
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
		}
		///first, calculating '^'
		for (int i = 0; i < ops.size(); i++)
		{
			if (ops.at(i) == "^")
			{
				varVals[i] = CalculateValue(ops.at(i), varVals.at(i), varVals.at(i + 1), fileName);
				varVals.erase(varVals.begin() + i + 1);
				ops.erase(ops.begin() + i);
				i--;
			}
		}
		///second, calculating '*' and '/'
		for (int i = 0; i < ops.size(); i++)
		{
			if (ops.at(i) == "*" || ops.at(i) == "/")
			{
				varVals[i] = CalculateValue(ops.at(i), varVals.at(i), varVals.at(i+1), fileName);
				varVals.erase(varVals.begin() + i + 1);
				ops.erase(ops.begin() + i);
				i--;
			}
		}
		///now just line up the rest of the operators with the variable values
		float result = varVals.at(0);
		for (int i = 1; i < varVals.size(); i++)
		{
			result = CalculateValue(ops.at(i - 1), result, varVals.at(i), fileName);
		}
		return result;
	}
	float CalculateValue(std::string op, float f1, float f2, std::string fileName) const
	{
		try {
			return operate.at(op)(f1, f2);
		}
		catch (const std::exception&)
		{
			std::string info = "File \"";
			info += fileName;
			info += "\": Unknown operator: ";	
			info += op;	
			info += "\n";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
};
