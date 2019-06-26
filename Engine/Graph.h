#pragma once
#include "Rect.h"
#include "Graphics.h"
#include "Colors.h"
#include "ChiliException.h"
#include "Font.h"
#include "MouseController.h"
#include "PhilUtil.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <cmath>

class Graph
{
public:
	class Exception : public ChiliException
	{
	public:
		using ChiliException::ChiliException;
		virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
		virtual std::wstring GetExceptionType() const override { return L"Graph Exception"; }
	};
private:
	class CoordinateSystem
	{
	public:
		class Exception : public ChiliException
		{
		public:
			using ChiliException::ChiliException;
			virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
			virtual std::wstring GetExceptionType() const override { return L"Coordinate System Exception"; }
		};
	public:
		CoordinateSystem() = default;
		CoordinateSystem(double xMax, double yMax, double offset, RectI screenRegion, Color axisColor, std::vector<Color> pixelColors);
		void Draw(const Font& f, Graphics& gfx, double xMaxAxis, double yMaxAxis) const;
		void PutCoordinate(double x, double y, int graph);
		void SetYMax(double newYMax);
		void SetXMax(double newXMax);
		void Refresh(const std::vector<std::unordered_map<int, std::pair<double, double>>>& data);
		void SetRectOn(bool b);
		bool IsNegative() const;
		double GetOffset() const;
		double GetHeight() const;
		double GetWidth() const;
		double GetYAxis() const;
		double GetXAxis() const;
		RectI GetScreenRegion() const;
		void TranslateScreenRegion(const Vei2& v);
		void ResizeScreenRegion(const Vei2& v);
		double GetXMax() const;
		double GetYMax() const;
		Color GetAxisColor() const;
	private:
		void ConvertToNegative();
	private:
		bool rectOn = false;
		bool initialized = false;
		double xMax;
		double yMax;
		double offset;
		bool negative = false;
		RectI screenRegion;
		Color axisColor;
		std::vector<Color> pixelColors;
		static constexpr int arrowWidth = 4;
		static constexpr int arrowLength = 7;
		static constexpr int lineLength = 4;
		int cur = 0;
		std::vector<std::unordered_map<int, std::pair<double, double>>> pixel;
	};
public:
	Graph() = default;
	Graph(double xMax, double yMax, double offset, RectI screenRegion, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f);
	Graph(RectI screenRegion, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f);
	Graph(RectI screenRegion, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f);
	Graph(RectI screenRegion, double offset, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f);
	void Update(MouseController& mouseControl);									//uses visible
	void Draw(std::string name, std::string xName, Graphics& gfx) const;		//uses visible
	void PutData(double x, double y, int system);
	bool IsNegative() const;
	bool IsVisible() const;
	void ToggleVisible();
	void SetVisible(bool b);
	RectI GetScreenRegion() const;
	std::vector<std::string> GetYAxisNames() const;
	std::string GetYAxisName() const;
	void Refresh();
	std::vector<std::unique_ptr<std::unordered_map<int, std::pair<double, double>>>> GetData() const;
private:
	//config values
	static constexpr int cropVal = 8;
	bool initialized = false;
	bool mouseInside = false;
	bool mouseZooming = false;
	bool visible = false;
	Font font;
	//coordinate system start values
	static constexpr double xMaxStart = 0.0000000000000001;		//minimal start x
	static constexpr double yMaxStart = 0.0000000000000001;		//minimal start y
	static constexpr double offset = 7.0;
	static constexpr Color axisColor = Colors::White;
	//yAxis
	std::vector<std::string> yAxisNames;
	std::vector<Color> pixelColors;
	//coordinate system
	CoordinateSystem coords;
	//data
	std::vector<int> cur;
	std::vector<std::unordered_map<int, std::pair<double, double>>> data;
	double maxYValue = yMaxStart;
	double maxXValue = xMaxStart;
	double maxXNumber = maxXValue * 10.0;		//floor value
	double maxYNumber = maxYValue * 10.0;		//floor value
};
