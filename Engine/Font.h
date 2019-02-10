#pragma once

#include "Graphics.h"
#include "Colors.h"
#include "Surface.h"
#include "Vec2.h"

class Font {
public:
	Font() = default;
	Font(const std::string& filename, Color chroma = Colors::White);
	void DrawText(const std::string& text, const Vei2& pos, Color color, Graphics& gfx) const;
	int GetHeight() const;
	int GetWidth() const;
private:
	RectI MapGlyphRect(char c) const;
private:
	Surface surface;
	int glyphWidth;
	int glyphHeight;
	static constexpr int nColumns = 32;
	static constexpr int nRows = 3;
	Color chroma;
	static constexpr char firstChar = ' ';
	static constexpr char lastChar = '~';
};
