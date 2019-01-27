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
		CoordinateSystem(float xMax, float yMax, float offset, bool negative, RectI screenRegion, Color axisColor, Color pixelColor)
			:
			xMax(xMax),
			yMax(yMax),
			offset(offset),
			negative(negative),
			screenRegion(screenRegion),
			axisColor(axisColor),
			pixelColor(pixelColor)
		{}
		void Draw(Graphics& gfx) const
		{
			//drawing coordinate system
			const Vec2 leftTop = { offset + screenRegion.left, offset + screenRegion.top };
			const Vec2 leftBottom = { offset + screenRegion.left, screenRegion.bottom - offset };
			gfx.DrawLine(leftTop, leftBottom, axisColor);
			///arrow drawing
			gfx.DrawLine(leftTop, { leftTop.x - arrowWidth / 2, leftTop.y + arrowLenght }, axisColor);
			gfx.DrawLine(leftTop, { leftTop.x + arrowWidth / 2, leftTop.y + arrowLenght }, axisColor);
			if (negative)
			{
				const Vec2 leftMiddle = { offset + screenRegion.left, (screenRegion.bottom - screenRegion.top) / 2 + screenRegion.top};
				const Vec2 rightMiddle = { screenRegion.right - offset, (screenRegion.bottom - screenRegion.top) / 2 + screenRegion.top };
				gfx.DrawLine(leftMiddle, rightMiddle, axisColor);
				///arrow drawing
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLenght, rightMiddle.y - arrowWidth / 2 }, axisColor);
				gfx.DrawLine(rightMiddle, { rightMiddle.x - arrowLenght, rightMiddle.y + arrowWidth / 2 }, axisColor);
			}
			else 
			{
				const Vec2 rightBottom = { screenRegion.right - offset, screenRegion.bottom - offset };
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
			const int xPixMax = screenRegion.right - offset;
			const int xPixMin = screenRegion.left + offset;
			const int yPixMax = screenRegion.bottom - offset;
			const int yPixMin = screenRegion.top + offset;

			const int deltaX = xPixMax - xPixMin;
			const int deltaY = yPixMax - yPixMin;

			const float xScale = (float)deltaX / xMax;
			const float yScale = (float)deltaY / (yMax * negative ? 2 : 1);

			const float xAxis = negative ? (yPixMin + deltaY / 2) : yPixMin;

			x *= xScale;
			y *= yScale;

			x += xPixMin;
			y += xAxis;

			pixel[++cur] = { x,y };
		}
		void SetXMax(float newYMax)
		{
			const float scale = newYMax / yMax;

			const int yPixMax = screenRegion.bottom - offset;
			const int yPixMin = screenRegion.top + offset;

			const int deltaY = yPixMax - yPixMin;

			const float yScale = (float)deltaY / (yMax * negative ? 2 : 1);

			const float xAxis = negative ? (yPixMin + deltaY / 2) : yPixMin;


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
	private:
		float xMax;
		float yMax;
		float offset;
		bool negative;
		RectI screenRegion;
		Color axisColor;
		Color pixelColor;
		static constexpr int arrowWidth = 4;
		static constexpr int arrowLenght = 7;
		int cur = 0;
		std::unordered_map<int, std::pair<float, float>> pixel;
	};
public:
	CoordinateSystem coords;
};
