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
		CoordinateSystem(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor);
		void Draw(const Font& f, Graphics& gfx, float xMaxAxis, float yMaxAxis) const;
		void PutCoordinate(float x, float y);
		void SetYMax(float newYMax);
		void SetXMax(float newXMax);
		void Refresh(const std::unordered_map<int, std::pair<float, float>>& data);
		void SetRectOn(bool b);
		bool IsNegative() const;
		float GetOffset() const;
		float GetHeight() const;
		float GetWidth() const;
		float GetYAxis() const;
		float GetXAxis() const;
		RectI GetScreenRegion() const;
		void TranslateScreenRegion(const Vei2& v);
		void ResizeScreenRegion(const Vei2& v);
		float GetXMax() const;
		float GetYMax() const;
		Color GetAxisColor() const;
	private:
		void ConvertToNegative();
	private:
		bool rectOn = false;
		bool initialized = false;
		float xMax;
		float yMax;
		float offset;
		bool negative = false;
		RectI screenRegion;
		Color axisColor;
		Color pixelColor;
		static constexpr int arrowWidth = 4;
		static constexpr int arrowLength = 7;
		static constexpr int lineLength = 4;
		int cur = 0;
		std::unordered_map<int, std::pair<float, float>> pixel;
	};
public:
	Graph() = default;
	Graph(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName, Font f);
	Graph(RectI screenRegion, Color pixelColor, std::string yAxisName, Font f);
	Graph(RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName, Font f);
	Graph(RectI screenRegion, float offset, Color axisColor, Color pixelColor, std::string yAxisName, Font f);
	void Update(MouseController& mouseControl);				//uses visible
	void Draw(std::string name, Graphics& gfx) const;		//uses visible
	void PutData(float x, float y);
	bool IsNegative() const;
	bool IsVisible() const;
	void ToggleVisible();
	void SetVisible(bool b);
	RectI GetScreenRegion() const;
	std::string GetYAxisName() const;
	void Refresh();
	std::unique_ptr<std::unordered_map<int, std::pair<float, float>>> GetData() const;
private:
	//config values
	static constexpr int cropVal = 8;
	bool initialized = false;
	bool mouseInside = false;
	bool mouseZooming = false;
	bool visible = false;
	Font font;
	//coordinate system start values
	static constexpr float xMaxStart = 0.0001f;		//minimal start x
	static constexpr float yMaxStart = 0.0001f;		//minimal start y
	static constexpr float offset = 7.0f;
	static constexpr Color axisColor = Colors::White;
	//yAxis name
	std::string yAxisName;
	//coordinate system
	CoordinateSystem coords;
	//data
	int cur = 0;
	std::unordered_map<int, std::pair<float, float>> data;
	float maxYValue = yMaxStart;
	float maxXValue = xMaxStart;
	float maxXNumber = maxXValue * 10.0f;		//floor value
	float maxYNumber = maxYValue * 10.0f;		//floor value
};
