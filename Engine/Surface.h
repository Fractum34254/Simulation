#pragma once

#include "ChiliException.h"
#include "Colors.h"
#include "Rect.h"
#include <string>
#include <vector>
#include <memory>

class Surface {
public:
	class Exception : public ChiliException
	{
	public:
		using ChiliException::ChiliException;
		virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
		virtual std::wstring GetExceptionType() const override { return L"Surface Exception"; }
	};
public:
	Surface(const std::string& filename);
	Surface(int width, int height);
	Surface(Surface&& donor);
	Surface() = default;
	Surface(const Surface&) = default;
	Surface& operator=(Surface&& rhs);
	Surface& operator=(const Surface&) = default;
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;
	RectI GetRect() const;
	void Fill(Color c);
	const Color* Data() const;
private:
	int width = 0;
	int height = 0;
	std::vector<Color> pixels;
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
};