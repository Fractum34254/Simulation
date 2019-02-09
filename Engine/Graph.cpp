#include "Graph.h"

Graph::CoordinateSystem::CoordinateSystem(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor)
	:
	xMax(xMax),
	yMax(yMax),
	offset(offset),
	screenRegion(screenRegion),
	axisColor(axisColor),
	pixelColor(pixelColor),
	initialized(true)
{}

void Graph::CoordinateSystem::Draw(const Font & f, Graphics & gfx, float xMaxAxis, float yMaxAxis) const
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
		const Vec2 leftMiddle = { offset + (float)screenRegion.left, ((float)screenRegion.bottom - (float)screenRegion.top) / 2 + (float)screenRegion.top };
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
	for (const std::pair<const int, std::pair<float, float>>& c : pixel)
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
	f.DrawText("0", Vei2((int)yAxis - lineLength / 2 - f.GetWidth() - textOffset, (int)xAxis - f.GetHeight() / 2 + textOffset), axisColor, gfx);
	///maximum number length for rect calculation later
	size_t yNumberLength = 0;
	///yAxis
	for (int i = 1; i < 5; i++)
	{
		gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis - i * deltaY, yAxis + lineLength / 2.0f, xAxis - i * deltaY, axisColor);
		std::stringstream ss;
		ss << yMaxAxis / 5.0f * i;
		f.DrawText(ss.str(), Vei2((int)yAxis - lineLength / 2 - f.GetWidth() * (int)ss.str().size() - textOffset, (int)xAxis - i * (int)deltaY - f.GetHeight() / 2 + textOffset), axisColor, gfx);
		yNumberLength = std::max(yNumberLength, ss.str().size());
	}
	if (IsNegative())
	{
		for (int i = 1; i < 5; i++)
		{
			gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis + i * deltaY, yAxis + lineLength / 2.0f, xAxis + i * deltaY, axisColor);
			std::stringstream ss;
			ss << yMaxAxis / 5.0f * i;
			f.DrawText(ss.str(), Vei2((int)yAxis - lineLength / 2 - f.GetWidth() * (int)ss.str().size() - textOffset, (int)xAxis + i * (int)deltaY - f.GetHeight() / 2 + textOffset), axisColor, gfx);
			yNumberLength = std::max(yNumberLength, ss.str().size());
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

	if (rectOn)
	{
		//draw rectangle
		RectI rect = RectI(screenRegion.left - f.GetWidth() * (int)yNumberLength, screenRegion.right, screenRegion.top - f.GetHeight() - (int)offset, screenRegion.bottom + f.GetHeight() + (int)offset);
		gfx.DrawRectLine(rect, 2, 0, axisColor.GetFaded(0.3f));
	}
}

void Graph::CoordinateSystem::PutCoordinate(float x, float y)
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

void Graph::CoordinateSystem::SetYMax(float newYMax)
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

void Graph::CoordinateSystem::SetXMax(float newXMax)
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

void Graph::CoordinateSystem::SetRectOn(bool b)
{
	rectOn = b;
}

bool Graph::CoordinateSystem::IsNegative() const
{
	return negative;
}

float Graph::CoordinateSystem::GetOffset() const
{
	return offset;
}

float Graph::CoordinateSystem::GetHeight() const
{
	return screenRegion.GetHeight() - 2 * offset;
}

float Graph::CoordinateSystem::GetWidth() const
{
	return screenRegion.GetWidth() - 2 * offset;
}

float Graph::CoordinateSystem::GetYAxis() const
{
	return (float)screenRegion.left + offset;
}

float Graph::CoordinateSystem::GetXAxis() const
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

RectI Graph::CoordinateSystem::GetScreenRegion() const
{
	return screenRegion;
}

void Graph::CoordinateSystem::ConvertToNegative() 
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

Graph::Graph(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName)
	:
	coords(xMax, yMax, offset, screenRegion, axisColor, pixelColor),
	yAxisName(yAxisName),
	initialized(true)
{}

Graph::Graph(RectI screenRegion, Color pixelColor, std::string yAxisName)
	:
	coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColor),
	yAxisName(yAxisName),
	initialized(true)
{}

Graph::Graph(RectI screenRegion, Color axisColor, Color pixelColor, std::string yAxisName)
	:
	coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColor),
	yAxisName(yAxisName),
	initialized(true)
{}

void Graph::Update(MouseController& mouseControl)
{
	if (coords.GetScreenRegion().Contains(mouseControl.GetMousePos()))
	{
		coords.SetRectOn(true);
	}
	else
	{
		coords.SetRectOn(false);
	}
}

void Graph::Draw(const Font & f, Graphics & gfx) const
{
	if (!initialized)
	{
		std::string info = "Unitialized graph!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
	}
	coords.Draw(f, gfx, maxXNumber, maxYNumber);
}

void Graph::PutData(float x, float y)
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

void Graph::WriteToFile(std::string filename) const
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
		file << "t: " << Crop(x, cropVal) << " "
			<< yAxisName << ": " << Crop(y, cropVal) << "\n";
	}
}

bool Graph::IsNegative() const
{
	return coords.IsNegative();
}
