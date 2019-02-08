#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include "ChiliException.h"
#include "Rect.h"
#include "Parser.h"
#include "Graph.h"

class File
{
public:
	class Exception : public ChiliException
	{
	public:
		using ChiliException::ChiliException;
		virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
		virtual std::wstring GetExceptionType() const override { return L"File Exception"; }
	};
public:
	File(std::string name, float offset, RectI screenRegion);
private:
	//Standart expression string
	///IMPORTANT: if adding new ones: also add them to the constructor (two times!)
	const std::string axisColorSet = "Axis Color:";
	const std::string graphColorSet = "Graph Color:";
	const std::string yNameSet = "y Value:";
	///breaking strings
	const std::string settingsEnd = "#Head";
	const std::string headEnd = "#Program";
	//Graph values
	static constexpr float xMaxStart = 1.0f;
	static constexpr float yMaxStart = 1.0f;
	float offset;
	RectI screenRegion;
	Color axisColor;
	Color pixelColor;
	std::string yAxisName;
	//File values
	std::string name;
	std::string code;
	std::string startVars;
	//Data values
	std::unordered_map<std::string, float> vars;

//utility functions
private:
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
	template<typename T>
	static bool isInVector(std::vector<T> vec, T val)
	{
		for (const T& v : vec)
		{
			if (v == val)
			{
				return true;
			}
		}
		return false;
	}
	template<typename T>
	static size_t findInVector(std::vector<T> vec, T val)
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec.at(i) == val)
			{
				return i;
			}
		}
		return vec.size() + 1;
	}
	static size_t getLongest(std::vector<std::string>& vec)
	{
		size_t maxLength = 0;
		for (const std::string& s : vec)
		{
			maxLength = std::max(maxLength, s.size());
		}
		return maxLength;
	}
	static std::string toString(int i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}
};
