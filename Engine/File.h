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
		//test, if file exists																						|-> can lead to EXCEPTION
		if(!file)
		{
			std::string info = "Can't open file ";	///standart-syntax
			info += name;							///wrong filename
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}

		/************************************** LOADING FILE SETTINGS *********************************************************/
		///adding setting names together																			|-> ADD NEW SETTINGS HERE
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

		do {
			///reading out the name of a setting && testing for end													|-> can lead to EXCEPTION
			for (char c = file.get(); !endReached && !isInVector(settingNames, setting); c = file.get())
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
			///now name of setting is in 'setting'
			if (!endReached)
			{
				///if init is already true, setting was already initialized											|-> can lead to EXCEPTION
				if (settingInit.at(findInVector(settingNames, setting)))
				{
					std::string info = "Double initialized setting in file ";		///standart-syntax
					info += name;													///file with incorrect settings
					info += ": ";
					info += setting;
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				///now: setting is not already initialized
				///'switch' on different strings -> test through ALL setting names									|-> can lead to EXCEPTION
				///																									|-> ADD NEW SETTINGS HERE
				if (setting == axisColorSet)
				{
					///red color value
					std::string r;
					int ri;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							r += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							ri = std::stoi(r);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad red axis color value in file ";
							info += name;
							info += ": ";
							info += r;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char rc = ri;
					///green color value
					std::string g;
					int gi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							g += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							gi = std::stoi(g);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad green axis color value in file ";
							info += name;
							info += ": ";
							info += g;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char gc = gi;
					///blue color value
					std::string b;
					int bi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							b += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							bi = std::stoi(b);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad blue axis color value in file ";
							info += name;
							info += ": ";
							info += b;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char bc = bi;
					///assemble them in one color
					axisColor = Color(rc, gc, bc);
				}
				else if (setting == graphColorSet)
				{
					///red color value
					std::string r;
					int ri;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							r += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							ri = std::stoi(r);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad red graph color value in file ";
							info += name;
							info += ": ";
							info += r;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char rc = ri;
					///green color value
					std::string g;
					int gi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							g += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							gi = std::stoi(g);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad green graph color value in file ";
							info += name;
							info += ": ";
							info += g;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char gc = gi;
					///blue color value
					std::string b;
					int bi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); c != ' '; c = file.get())
						{
							b += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							bi = std::stoi(b);
						}
						catch (const std::exception&)
						{
							std::string info = "Bad blue graph color value in file ";
							info += name;
							info += ": ";
							info += b;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char bc = bi;
					///assemble them in one color
					pixelColor = Color(rc, gc, bc);
				}
				else if (setting == yNameSet)
				{
					///test for blank space
					char c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
					for (c = file.get(); c != ' '; c = file.get())
					{
						yAxisName += c;
					}
				}
				settingInit.at(findInVector(settingNames, setting)) = true;
			}
			else
			{
				///checks, if all settings are initialized															|-> can lead to EXCEPTION	
				if (!std::all_of(settingInit.begin(), settingInit.end(), [](bool b) {return b; }))
				{
					std::string info = "Not enough settings found in ";
					info += name;
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
			}
		} while (!endReached);

		/************************************** LOADING START VARS *********************************************************/

		/************************************** LOADING PROGRAM CODE *********************************************************/

		/************************************** EXECUTING START VARS *********************************************************/
	}
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
};
