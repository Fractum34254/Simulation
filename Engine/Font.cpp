#include "Font.h"
#include "SpriteEffect.h"
#include <cassert>

Font::Font(const std::string & filename, Color chroma)
	:
	surface(filename),
	glyphWidth(surface.GetWidth() / nColumns),
	glyphHeight(surface.GetHeight() / nRows),
	chroma(chroma)
{
	assert(glyphWidth * nColumns == surface.GetWidth());
	assert(glyphHeight * nRows == surface.GetHeight());
}

void Font::DrawText(const std::string & text, const Vei2 & pos, Color color, Graphics & gfx) const
{
	SpriteEffect::Substitution e{ chroma,color };
	auto curPos = pos;
	for (auto c : text) {
		if (c == '\n') {
			curPos.x = pos.x;
			curPos.y += glyphHeight;
			continue;
		}
		else if (c >= firstChar + 1 && c <= lastChar) {
			gfx.DrawSprite(curPos.x, curPos.y, MapGlyphRect(c), surface, e);
		}
		curPos.x += glyphWidth;
	}
}

int Font::GetHeight() const
{
	return glyphHeight;
}

int Font::GetWidth() const
{
	return glyphWidth;
}

RectI Font::MapGlyphRect(char c) const
{
	assert(c >= firstChar && c <= lastChar);
	const int glyphIndex = c - ' ';
	const int glyphY = glyphIndex / nColumns;
	const int glyphX = glyphIndex % nColumns;
	return RectI(glyphX * glyphWidth, (glyphX + 1) * glyphWidth, glyphY * glyphHeight, (glyphY + 1) * glyphHeight);
}
