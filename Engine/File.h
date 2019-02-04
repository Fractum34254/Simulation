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
	File(std::string name, float offset, RectI screenRegion)
		:
		name(name),
		offset(offset),
		screenRegion(screenRegion)
	{
		std::ifstream file(name);
		//test, if file exists
		if(!file)
		{
			std::string info = "Can't open file ";	///standart-syntax
			info += name;							///wrong filename
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}

		//loading file settings

		///adding setting names together
		std::vector<std::string> settingNames;
		settingNames.emplace_back(axisColorSet);
		settingNames.emplace_back(graphColorSet);
		settingNames.emplace_back(yNameSet);
		///vector of bool to keep track whether a variable is initialized or not
		std::vector<bool> settingInit;
		for (size_t i = 0; i < settingNames.size(); i++)
		{
			settingInit.emplace_back(false);
		}
		///reading out the settings
		bool endReached = false;
		std::string setting;

		//start of a while loop
		for (char c = file.get(); !endReached && !findInVector(settingNames, setting); c = file.get())
		{
			setting += c;
			if (setting.size() > std::max(getLongest(settingNames), settingsEnd.size()))
			{
				std::string info = "Wrong spelling of setting name in file ";	///standart-syntax
				info += name;													///file with incorrect settings
				info += ": ";
				info += setting;
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			if (setting == settingsEnd)
			{
				endReached = true;
			}
		}
		if (!endReached)
		{

		}
		///end of a while loop?

		//loading startVars

		//loading program code

		//executing startVars
	}
private:
	//Standart expression string
	const std::string axisColorSet = "Axis Color:";
	const std::string graphColorSet = "Graph Color:";
	const std::string yNameSet = "y Value:";
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
	static bool findInVector(std::vector<T> vec, T val)
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
	static size_t getLongest(std::vector<std::string>& vec)
	{
		size_t maxLength = 0;
		for (const std::string& s : vec)
		{
			maxLength = std::max(maxLength, s.size());
		}
		return maxLength;
	}
};
