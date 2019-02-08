#pragma once
#include "Rect.h"
#include "Graphics.h"
#include "Colors.h"
#include "PhilUtil.h"
#include "ChiliException.h"
#include "Font.h"
#include <string>
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
		CoordinateSystem(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor)
			:
			xMax(xMax),
			yMax(yMax),
			offset(offset),
			screenRegion(screenRegion),
			axisColor(axisColor),
			pixelColor(pixelColor),
			initialized(true)
		{}
		void Draw(const Font& f, Graphics& gfx, float xMaxAxis, float yMaxAxis) const
		{
			if (!initialized)
			{
				std::string info = "Unitialized coordinate system!";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			//drawing coordinate system
			const Vec2 leftTop = { offset + (float)screenRegion.left, offset + (float)screenRegion.top };
			const Vec2 leftBottom = { offset + (float)screenRegion.left, (float)screenRegion.bottom - offset };
			gfx.DrawLine(leftTop, leftBottom, axisColor);
			///arrow drawing
			gfx.DrawLine(leftTop, { leftTop.x - arrowWidth / 2, leftTop.y + arrowLength }, axisColor);
			gfx.DrawLine(leftTop, { leftTop.x + arrowWidth / 2, leftTop.y + arrowLength }, axisColor);
			if (negative)
			{
				const Vec2 leftMiddle = { offset + (float)screenRegion.left, ((float)screenRegion.bottom - (float)screenRegion.top) / 2 + (float)screenRegion.top};
				const Vec2 rightMiddle = { (float)screenRegion.right - offset, ((float)screenRegion.bottom - (float)screenRegion.top) / 2 + (float)screenRegion.top };
				gfx.DrawLine(leftMiddle, rightMiddle, axisColor);
				///arrow drawing
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLength, rightMiddle.y - arrowWidth / 2 }, axisColor);
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLength, rightMiddle.y + arrowWidth / 2 }, axisColor);
			}
			else 
			{
				const Vec2 rightBottom = { (float)screenRegion.right - offset, (float)screenRegion.bottom - offset };
				gfx.DrawLine(leftBottom, rightBottom, axisColor);
				///arrow drawing
				gfx.DrawLine(rightBottom, { rightBottom.x - arrowLength, rightBottom.y - arrowWidth / 2 }, axisColor);
				gfx.DrawLine(rightBottom, { rightBottom.x - arrowLength, rightBottom.y + arrowWidth / 2 }, axisColor);
			}

			//drawing the pixels
			for (const std::pair<const int,std::pair<float,float>>& c : pixel)
			{
				gfx.PutPixel((int)c.second.first, (int)c.second.second, pixelColor);
			}

			//drawing the axis division
			const float deltaY = GetHeight() / (IsNegative() ? 20.0f : 10.0f) * 2.0f;
			const float deltaX = GetWidth() / 10.0f * 2.0f;
			const float yAxis = GetYAxis();
			const float xAxis = GetXAxis();
			const int textOffset = 2;

			///0
			gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis, yAxis + lineLength / 2.0f, xAxis, axisColor);
			f.DrawText("0", Vei2((int)yAxis - lineLength / 2 - f.GetWidth() - textOffset, (int)xAxis- f.GetHeight() / 2 + textOffset), axisColor, gfx);

			///yAxis
			for (int i = 1; i < 5; i++)
			{
				gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis - i * deltaY, yAxis + lineLength / 2.0f, xAxis - i * deltaY, axisColor);
				std::stringstream ss;
				ss << yMaxAxis / 5.0f * i;
				f.DrawText(ss.str(), Vei2((int)yAxis - lineLength / 2 - f.GetWidth() * (int)ss.str().size() - textOffset, (int)xAxis - i * (int)deltaY - f.GetHeight() / 2 + textOffset), axisColor, gfx);
			}
			if (IsNegative())
			{
				for (int i = 1; i < 5; i++)
				{
					gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis + i * deltaY, yAxis + lineLength / 2.0f, xAxis + i * deltaY, axisColor);
					std::stringstream ss;
					ss << yMaxAxis / 5.0f * i;
					f.DrawText(ss.str(), Vei2((int)yAxis - lineLength / 2- f.GetWidth() * (int)ss.str().size() - textOffset, (int)xAxis + i * (int)deltaY - f.GetHeight() / 2 + textOffset), axisColor, gfx);
				}
			}
			///xAxis
			for (int i = 1; i < 5; i++)
			{
				gfx.DrawLine(yAxis + i * deltaX, xAxis - lineLength / 2.0f, yAxis + i * deltaX, xAxis + lineLength / 2.0f, axisColor);
				std::stringstream ss;
				ss << xMaxAxis / 5.0f * i;
				f.DrawText(ss.str(), Vei2((int)yAxis + i * (int)deltaX - f.GetWidth() * (int)ss.str().size() / 2, (int)xAxis + lineLength / 2 + textOffset), axisColor, gfx);
			}
		}
		void PutCoordinate(float x, float y)
		{
			if (!initialized)
			{
				std::string info = "Unitialized coordinate system!";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			if (!negative && y < 0.0f)
			{
				ConvertToNegative();
			}
			if (x >= xMax)
			{
				SetXMax(1.1f * x);
			}
			if (std::abs(y) >= yMax)
			{
				SetYMax(1.1f * std::abs(y));
			}
			const int xPixMax = screenRegion.right - (int)offset;
			const int xPixMin = screenRegion.left + (int)offset;
			const int yPixMax = screenRegion.bottom - (int)offset;
			const int yPixMin = screenRegion.top + (int)offset;

			const int deltaX = xPixMax - xPixMin;
			const int deltaY = yPixMax - yPixMin;

			const float xScale = (float)deltaX / xMax;
			const float yScale = (float)deltaY / (yMax * (negative ? 2.0f : 1.0f));

			const float xAxis = negative ? ((float)yPixMin + (float)deltaY / 2) : (float)yPixMax;

			x *= xScale;
			y *= -yScale;

			x += xPixMin;
			y += xAxis;

			pixel[cur++] = { x,y };
		}
		void SetYMax(float newYMax)
		{
			if (!initialized)
			{
				std::string info = "Unitialized coordinate system!";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			const float scale = yMax / newYMax;

			const int yPixMax = screenRegion.bottom - (int)offset;
			const int yPixMin = screenRegion.top + (int)offset;

			const int deltaY = yPixMax - yPixMin;

			const float yScale = (float)deltaY / ((float)yMax * negative ? 2.0f : 1.0f);

			const float xAxis = negative ? ((float)yPixMin + (float)deltaY / 2) : (float)yPixMax;


			for (std::pair<const int, std::pair<float, float>>& c : pixel)
			{
				c.second.second -= xAxis;
				c.second.second /= yScale;
				c.second.second *= scale;
				c.second.second *= yScale;
				c.second.second += xAxis;
			}
			yMax = newYMax;
		}
		void SetXMax(float newXMax)
		{
			if (!initialized)
			{
				std::string info = "Unitialized coordinate system!";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
			const float scale = xMax / newXMax;

			const int xPixMax = screenRegion.right - (int)offset;
			const int xPixMin = screenRegion.left + (int)offset;

			const int deltaX = xPixMax - xPixMin;

			const float xScale = (float)deltaX / (float)xMax;

			for (std::pair<const int, std::pair<float, float>>& c : pixel)
			{
				c.second.first -= xPixMin;
				c.second.first /= xScale;
				c.second.first *= scale;
				c.second.first *= xScale;
				c.second.first += xPixMin;
			}
			xMax = newXMax;
		}
		bool IsNegative() const
		{
			return negative;
		}
		float GetOffset() const
		{
			return offset;
		}
		float GetHeight() const
		{
			return screenRegion.GetHeight() - 2 * offset;
		}
		float GetWidth() const
		{
			return screenRegion.GetWidth() - 2 * offset;
		}
		float GetYAxis() const
		{
			return (float)screenRegion.left + offset;
		}
		float GetXAxis() const
		{
			if (IsNegative())
			{
				return (float)screenRegion.bottom - (float)screenRegion.GetHeight() / 2.0f;
			}
			else
			{
				return (float)screenRegion.bottom - offset;
			}
		}
	private:
		void ConvertToNegative()
		{
			const int yPixMax = screenRegion.bottom - (int)offset;
			const int yPixMin = screenRegion.top + (int)offset;

			const int deltaY = yPixMax - yPixMin;

			const float yScale = (float)deltaY / ((float)yMax * negative ? 2.0f : 1.0f);

			const float xAxis = negative ? ((float)yPixMin + (float)deltaY / 2) : (float)yPixMax;

			negative = true;

			const float yScaleNew = (float)deltaY / ((float)yMax * negative ? 2.0f : 1.0f);

			const float xAxisNew = negative ? ((float)yPixMin + (float)deltaY / 2) : (float)yPixMax;

			for (std::pair<const int, std::pair<float, float>>& c : pixel)
			{
				c.second.second -= xAxis;
				c.second.second /= yScale;
				c.second.second *= yScaleNew;
				c.second.second += xAxisNew;
			}
		}
	private:
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
	Graph(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName)
		:
		coords(xMax, yMax, offset, screenRegion, axisColor, pixelColor),
		yAxisName(yAxisName),
		initialized(true)
	{}
	Graph(RectI screenRegion, Color pixelColor, std::string yAxisName)
		:
		coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColor),
		yAxisName(yAxisName),
		initialized(true)
	{}
	Graph(RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName)
		:
		coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColor),
		yAxisName(yAxisName),
		initialized(true)
	{}
	void Draw(const Font& f, Graphics& gfx) const
	{
		if (!initialized)
		{
			std::string info = "Unitialized graph!";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
		coords.Draw(f, gfx, maxXNumber, maxYNumber);
	}
	void PutData(float x, float y)
	{
		if (!initialized)
		{
			std::string info = "Unitialized graph!";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
		data[cur++] = { x,y };
		maxXValue = std::max(maxXValue, std::abs(x));
		maxYValue = std::max(maxYValue, std::abs(y));
		if (maxXNumber < floor(maxXValue))
		{
			maxXNumber = floor(maxXValue);
			coords.SetXMax(maxXNumber);
		}
		if (maxYNumber < floor(maxYValue))
		{
			maxYNumber = floor(maxYValue);
			coords.SetYMax(maxYNumber);
		}
		coords.PutCoordinate(x, y);
	}
	void WriteToFile(std::string filename) const
	{
		if (!initialized)
		{
			std::string info = "Unitialized graph!";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
		std::ofstream file(filename);
		for (int i = 0; i < cur; i++)
		{
			const float x = data.at(i).first;
			const float y = data.at(i).second;
			file << "t: " << Crop(x,cropVal) << " "
				<< yAxisName << ": " << Crop(y, cropVal) << "\n";
		}
	}
	bool IsNegative() const
	{
		return coords.IsNegative();
	}
private:
	//config values
	static constexpr int cropVal = 8;
	bool initialized = false;
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
	static float floor(float in)
	{
		int f = 0;
		if (in >= 1.0f)
		{
			while (std::pow(10, f) < in)
			{
				f++;
			}
		}
		else
		{
			while (std::pow(10, f) > in)
			{
				f--;
			}
		}
		return (float)std::pow(10, f);
	}
};
