#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <functional>
#include "ChiliException.h"
#include "Icon.h"
#include "Rect.h"
#include "Parser.h"
#include "Graph.h"
#include "Font.h"
#include "Iconbar.h"

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
	File() = default;
	File(std::string name, float offset, RectI screenRegion);
	void Update(MouseController& mouseControl);
	void Calculate(float dt);
	void Draw(Graphics& gfx) const;
	void SetRepeatValue(int rv);
	int GetRepeatVal() const;
	void Save() const;
	void RefreshAll();
	void RefreshGraph(int i);
	void SetCalculating(bool b);
	bool GetCalculating() const;
	void ToggleVisible();
	void SetVisible(int graph, bool b);
	void CloseAll();
	bool AllVisible() const;
	bool AnyVisible() const;
	std::string GetName() const;
private:
	void SetUpButtons();
	void CalculateOnce();
private:
	//Standart expression string
	///IMPORTANT: if adding new ones: also add them to the constructor!
	const std::string axisColorSet = "Axis Color:";							
	const std::string graphColorSet = "Graph Color:";						
	const std::string yNameSet = "y Values:";								
	const std::string timeName = "Time variable:";							
	const std::string repeatingName = "Repeating per second:";				
	///breaking strings
	const std::string settingsEnd = "#Head";
	const std::string headEnd = "#Program";
	//Graph values
	static constexpr float xMaxStart = 1.0f;
	static constexpr float yMaxStart = 1.0f;
	float offset;
	Color axisColor;
	Color pixelColor;
	std::vector<std::string> yAxisNames;
	std::string timeVar;
	int repeatVal;
	bool calculating = true;
	std::vector<std::unique_ptr<Graph>> graphs;
	std::vector<Iconbar> iconbars;
	Font font;
	Parser parser;
	//File values
	static constexpr int cropVal = 8;
	std::string ownName;
	std::string code;
	std::string startVars;
	float time = 0.0f;
	//Data values
	std::unordered_map<std::string, float> vars;
};
