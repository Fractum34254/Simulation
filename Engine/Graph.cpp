#include "Graph.h"

Graph::CoordinateSystem::CoordinateSystem(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, std::vector<Color> pixelColors)
	:
	xMax(xMax),
	yMax(yMax),
	offset(offset),
	screenRegion(screenRegion),
	axisColor(axisColor),
	pixelColors(pixelColors),
	initialized(true)
{
	pixel.reserve(pixelColors.size());
}

void Graph::CoordinateSystem::Draw(const Font & f, Graphics & gfx, float xMaxAxis, float yMaxAxis) const
{
	if (!initialized)
	{
		std::string info = "Unitialized coordinate system!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	//drawing coordinate system
	const Vec2 leftTop = { offset + (float)screenRegion.left, (float)screenRegion.top - offset};
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
	for (int i = 0; i < pixel.size(); i++)
	{
		for (const std::pair<const int, std::pair<float, float>>& c : pixel.at(i))
		{
			gfx.PutPixel((int)c.second.first, (int)c.second.second, pixelColors.at(i));
		}
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
	for (int i = 1; i <= 5; i++)
	{
		gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis - i * deltaY, yAxis + lineLength / 2.0f, xAxis - i * deltaY, axisColor);
		std::stringstream ss;
		ss << yMaxAxis / 5.0f * i;
		f.DrawText(ss.str(), Vei2((int)yAxis - lineLength / 2 - f.GetWidth() * (int)ss.str().size() - textOffset, (int)xAxis - i * (int)deltaY - f.GetHeight() / 2 + textOffset), axisColor, gfx);
		yNumberLength = std::max(yNumberLength, ss.str().size());
	}
	if (IsNegative())
	{
		for (int i = 1; i <= 5; i++)
		{
			gfx.DrawLine(yAxis - lineLength / 2.0f, xAxis + i * deltaY, yAxis + lineLength / 2.0f, xAxis + i * deltaY, axisColor);
			std::stringstream ss;
			ss << -yMaxAxis / 5.0f * i;
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
		RectI rect = RectI(
			screenRegion.left - f.GetWidth() * (int)yNumberLength,  ///left
			screenRegion.right,										///right
			screenRegion.top - f.GetHeight() - 2 * (int)offset,		///top
			screenRegion.bottom + f.GetHeight());					///bottom
		gfx.DrawRectLine(rect, 2, 0, axisColor.GetFaded(0.3f));
		RectI zoomRect = RectI::FromCenter(Vei2(rect.right, rect.bottom), 10, 10);
		gfx.DrawRectLine(zoomRect, 2, 0, axisColor.GetFaded(0.3f));
	}
}

void Graph::CoordinateSystem::PutCoordinate(float x, float y, int graph)
{
	if (!initialized)
	{
		std::string info = "Unitialized coordinate system!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	if (!negative && y < 0.0f)
	{
		ConvertToNegative();
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

	if (graph >= pixel.size())
	{
		pixel.resize(graph + 1);
	}
	pixel.at(graph)[cur++] = { std::min((float)xPixMax,std::max(x, (float)xPixMin)),std::min((float)yPixMax,std::max(y, (float)yPixMin)) };
}

void Graph::CoordinateSystem::SetYMax(float newYMax)
{
	if (!initialized)
	{
		std::string info = "Unitialized coordinate system!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	const float scale = yMax / newYMax;

	const int yPixMax = screenRegion.bottom - (int)offset;
	const int yPixMin = screenRegion.top + (int)offset;

	const int deltaY = yPixMax - yPixMin;

	const float yScale = (float)deltaY / ((float)yMax * negative ? 2.0f : 1.0f);

	const float xAxis = negative ? ((float)yPixMin + (float)deltaY / 2) : (float)yPixMax;


	for (int i = 0; i < pixel.size(); i++)
	{
		for (std::pair<const int, std::pair<float, float>>& c : pixel.at(i))
		{
			c.second.second -= xAxis;
			c.second.second /= yScale;
			c.second.second *= scale;
			c.second.second *= yScale;
			c.second.second += xAxis;
		}
	}
	yMax = newYMax;
}

void Graph::CoordinateSystem::SetXMax(float newXMax)
{
	if (!initialized)
	{
		std::string info = "Unitialized coordinate system!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	const float scale = xMax / newXMax;

	const int xPixMax = screenRegion.right - (int)offset;
	const int xPixMin = screenRegion.left + (int)offset;

	const int deltaX = xPixMax - xPixMin;

	const float xScale = (float)deltaX / (float)xMax;

	for (int i = 0; i < pixel.size(); i++)
	{
		for (std::pair<const int, std::pair<float, float>>& c : pixel.at(i))
		{
			c.second.first -= xPixMin;
			c.second.first /= xScale;
			c.second.first *= scale;
			c.second.first *= xScale;
			c.second.first += xPixMin;
		}
	}
	xMax = newXMax;
}

void Graph::CoordinateSystem::Refresh(const std::vector<std::unordered_map<int, std::pair<float, float>>>& data)
{
	pixel.clear();
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data.at(i).size(); j++)
		{
			PutCoordinate(data.at(i).at(j).first, data.at(i).at(j).second, i);
		}
	}
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

void Graph::CoordinateSystem::TranslateScreenRegion(const Vei2 & v)
{
	screenRegion.TranslateClipped(v, Graphics::GetScreenRect());
}

void Graph::CoordinateSystem::ResizeScreenRegion(const Vei2 & v)
{
	screenRegion.Resize(v);
}

float Graph::CoordinateSystem::GetXMax() const
{
	return xMax;
}

float Graph::CoordinateSystem::GetYMax() const
{
	return yMax;
}

Color Graph::CoordinateSystem::GetAxisColor() const
{
	return axisColor;
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

	for (int i = 0; i < pixel.size(); i++)
	{
		for (std::pair<const int, std::pair<float, float>>& c : pixel.at(i))
		{
			c.second.second -= xAxis;
			c.second.second /= yScale;
			c.second.second *= yScaleNew;
			c.second.second += xAxisNew;
		}
	}
}

Graph::Graph(float xMax, float yMax, float offset, RectI screenRegion, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f)
	:
	coords(xMax, yMax, offset, screenRegion, axisColor, pixelColors),
	yAxisNames(yAxisNames),
	pixelColors(pixelColors),
	initialized(true),
	font(f)
{
	cur.resize(yAxisNames.size());
	data.resize(yAxisNames.size());
	for (int& i : cur)
	{
		i = 0;
	}
}

Graph::Graph(RectI screenRegion, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f)
	:
	coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColors),
	yAxisNames(yAxisNames),
	pixelColors(pixelColors),
	initialized(true),
	font(f)
{
	cur.resize(yAxisNames.size());
	data.resize(yAxisNames.size());
	for (int& i : cur)
	{
		i = 0;
	}
}

Graph::Graph(RectI screenRegion, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f)
	:
	coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColors),
	yAxisNames(yAxisNames),
	pixelColors(pixelColors),
	initialized(true),
	font(f)
{
	cur.resize(yAxisNames.size());
	data.resize(yAxisNames.size());
	for (int& i : cur)
	{
		i = 0;
	}
}

Graph::Graph(RectI screenRegion, float offset, Color axisColor, std::vector<Color> pixelColors, std::vector<std::string> yAxisNames, Font f)
	:
	coords(xMaxStart, yMaxStart, offset, screenRegion, axisColor, pixelColors),
	yAxisNames(yAxisNames),
	pixelColors(pixelColors),
	initialized(true),
	font(f)
{
	cur.resize(yAxisNames.size());
	data.resize(yAxisNames.size());
	for (int& i : cur)
	{
		i = 0;
	}
}

void Graph::Update(MouseController& mouseControl)
{
	if (!visible)
	{
		return;
	}
	RectI rect = coords.GetScreenRegion();
	rect.bottom += font.GetHeight() + 10;
	rect.top -= font.GetHeight() + 2 * (int)coords.GetOffset();
	rect.left -= 3 * font.GetWidth();
	rect.right += 10;

	if (rect.Contains(mouseControl.GetMousePos()))
	{
		coords.SetRectOn(true);
		if (mouseInside)
		{
			if (mouseControl.mouse.LeftIsPressed())
			{
				const Vei2 corner = Vei2( coords.GetScreenRegion().right, coords.GetScreenRegion().bottom + font.GetHeight() + (int)coords.GetOffset());
				RectI zoomRect = RectI::FromCenter(corner, 10, 10);

				if (zoomRect.Contains(mouseControl.GetMousePos()))
				{
					//zooming
					mouseZooming = true;
					coords.ResizeScreenRegion(mouseControl.diff);
					coords.Refresh(data);
				}
				else if(!mouseZooming)
				{
					//translation
					coords.TranslateScreenRegion(mouseControl.diff);
					coords.Refresh(data);
				}
			}
			else 
			{
				mouseZooming = false;
			}
		}
		else if (!mouseControl.mouse.LeftIsPressed())
		{
			mouseControl.SetPointer(MouseController::MousePointer::Pointer::cross);
			mouseInside = true;
		}
	}
	else if(mouseInside)
	{
		mouseControl.SetPointer(MouseController::MousePointer::Pointer::normal);
		mouseInside = false;
		coords.SetRectOn(false);
	}
}

void Graph::Draw(std::string name, std::string xName, Graphics& gfx) const
{
	if (!initialized)
	{
		std::string info = "Unitialized graph!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	if (!visible)
	{
		return;
	}
	const size_t s = name.length() * font.GetWidth() / 2;
	const Vei2 pos = Vei2(
		coords.GetScreenRegion().GetWidth() / 2 - (int)s + coords.GetScreenRegion().left,
		coords.GetScreenRegion().top - font.GetHeight() - (int)coords.GetOffset());
	font.DrawText(name, pos, coords.GetAxisColor(), gfx);
	coords.Draw(font, gfx, maxXNumber, maxYNumber);
	//draw yAxis names
	Vei2 rawPos = Vei2(GetScreenRegion().left - (int)offset, GetScreenRegion().top - font.GetHeight() - (int)offset);
	int delta = 0;
	for (int i = 0; i < yAxisNames.size(); i++)
	{
		rawPos.x += delta;
		font.DrawText(yAxisNames.at(i), rawPos,	pixelColors.at(i), gfx);
		delta = (int)yAxisNames.at(i).size() * (font.GetWidth() + 1);
	}
	font.DrawText(xName, Vei2(GetScreenRegion().right - 2 * (int)offset, GetScreenRegion().bottom - (IsNegative() ? GetScreenRegion().GetHeight() / 2 - (int)offset : 0)), axisColor, gfx);
}

void Graph::PutData(float x, float y, int system)
{
	if (!initialized)
	{
		std::string info = "Unitialized graph!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	data.at(system)[cur.at(system)++] = { x,y };
	maxXValue = std::max(maxXValue, std::abs(x));
	maxYValue = std::max(maxYValue, std::abs(y));
	if (maxXNumber < PhilUtil::top(maxXValue))
	{
		maxXNumber = PhilUtil::top(maxXValue);
		coords.SetXMax(maxXNumber);
	}
	if (maxYNumber < PhilUtil::top(maxYValue))
	{
		maxYNumber = PhilUtil::top(maxYValue);
		coords.SetYMax(maxYNumber);
	}
	coords.PutCoordinate(x, y, system);
}

bool Graph::IsNegative() const
{
	return coords.IsNegative();
}

bool Graph::IsVisible() const
{
	return visible;
}

void Graph::ToggleVisible()
{
	visible = !visible;
}

void Graph::SetVisible(bool b)
{
	visible = b;
}

RectI Graph::GetScreenRegion() const
{
	return coords.GetScreenRegion();
}

std::vector<std::string> Graph::GetYAxisNames() const
{
	return yAxisNames;
}

std::string Graph::GetYAxisName() const
{
	std::string tempS;
	tempS = yAxisNames.at(0);
	for (int i = 1; i < yAxisNames.size(); i++)
	{
		tempS += " ";
		tempS += yAxisNames.at(i);
	}
	return tempS;
}

void Graph::Refresh()
{
	coords.Refresh(data);
}

std::vector<std::unique_ptr<std::unordered_map<int, std::pair<float, float>>>> Graph::GetData() const
{
	std::vector<std::unique_ptr<std::unordered_map<int, std::pair<float, float>>>> output;
	for (const auto& map : data)
	{
		output.emplace_back(std::make_unique<std::unordered_map<int, std::pair<float, float>>>(map));
	}
	return output;
}
