#pragma once
#include "Rect.h"
#include "Graphics.h"
#include "Colors.h"
#include <unordered_map>

class Graph
{
private:
	class CoordinateSystem
	{
	public:
		CoordinateSystem(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor)
			:
			xMax(xMax),
			yMax(yMax),
			offset(offset),
			screenRegion(screenRegion),
			axisColor(axisColor),
			pixelColor(pixelColor)
		{}
		void Draw(Graphics& gfx) const
		{
			//drawing coordinate system
			const Vec2 leftTop = { offset + (float)screenRegion.left, offset + (float)screenRegion.top };
			const Vec2 leftBottom = { offset + (float)screenRegion.left, (float)screenRegion.bottom - offset };
			gfx.DrawLine(leftTop, leftBottom, axisColor);
			///arrow drawing
			gfx.DrawLine(leftTop, { leftTop.x - arrowWidth / 2, leftTop.y + arrowLenght }, axisColor);
			gfx.DrawLine(leftTop, { leftTop.x + arrowWidth / 2, leftTop.y + arrowLenght }, axisColor);
			if (negative)
			{
				const Vec2 leftMiddle = { offset + (float)screenRegion.left, ((float)screenRegion.bottom - (float)screenRegion.top) / 2 + (float)screenRegion.top};
				const Vec2 rightMiddle = { (float)screenRegion.right - offset, ((float)screenRegion.bottom - (float)screenRegion.top) / 2 + (float)screenRegion.top };
				gfx.DrawLine(leftMiddle, rightMiddle, axisColor);
				///arrow drawing
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLenght, rightMiddle.y - arrowWidth / 2 }, axisColor);
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLenght, rightMiddle.y + arrowWidth / 2 }, axisColor);
			}
			else 
			{
				const Vec2 rightBottom = { (float)screenRegion.right - offset, (float)screenRegion.bottom - offset };
				gfx.DrawLine(leftBottom, rightBottom, axisColor);
				///arrow drawing
				gfx.DrawLine(rightBottom, { rightBottom.x - arrowLenght, rightBottom.y - arrowWidth / 2 }, axisColor);
				gfx.DrawLine(rightBottom, { rightBottom.x - arrowLenght, rightBottom.y + arrowWidth / 2 }, axisColor);
			}

			//drawing the pixels
			for (const std::pair<const int,std::pair<float,float>>& c : pixel)
			{
				gfx.PutPixel((int)c.second.first, (int)c.second.second, pixelColor);
			}
		}
		void PutCoordinate(float x, float y)
		{
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
		float xMax;
		float yMax;
		float offset;
		bool negative = false;
		RectI screenRegion;
		Color axisColor;
		Color pixelColor;
		static constexpr int arrowWidth = 4;
		static constexpr int arrowLenght = 7;
		int cur = 0;
		std::unordered_map<int, std::pair<float, float>> pixel;
	};
public:
	Graph(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor)
		:
		coords(xMax, yMax, offset, screenRegion, axisColor, pixelColor)
	{}
	void Draw(Graphics& gfx) const
	{
		coords.Draw(gfx);
	}
	void PutData(float x, float y)
	{
		data[cur++] = { x,y };
		coords.PutCoordinate(x, y);
	}
private:
	CoordinateSystem coords;
	int cur = 0;
	std::unordered_map<int, std::pair<float, float>> data;
};
