#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <functional>
#include "ChiliException.h"
#include "Rect.h"
#include "Parser.h"
#include "Graph.h"
#include "Font.h"

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
	void Calculate(float dt);
	void Draw(Graphics& gfx) const;
private:
	//Standart expression string
	///IMPORTANT: if adding new ones: also add them to the constructor!
	const std::string axisColorSet = "Axis Color:";							
	const std::string graphColorSet = "Graph Color:";						
	const std::string yNameSet = "y Value:";								
	const std::string timeName = "Time variable:";							
	const std::string repeatingName = "Repeating per second:";				
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
	std::string timeVar;
	int repeatVal;
	Graph graph;
	Font font;
	//File values
	std::string ownName;
	std::string code;
	std::string startVars;
	float time = 0.0f;
	//Data values
	std::unordered_map<std::string, float> vars;

//utility functions
private:
	static char toColorChar(std::ifstream& file, const std::string colorName, const std::string& fileName);
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
	static std::string toString(int i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}
};
