#pragma once
#include "Colors.h"
#include "Graphics.h"

namespace SpriteEffect {
	class Chroma {
	public:
		Chroma(Color c)
			:
			chroma(c) 
		{}
		void operator()(Color cSrc, int xDest, int yDest, Graphics& gfx) const {
			if (cSrc != chroma) {
				gfx.PutPixel(xDest, yDest, cSrc);
			}
		}
	private:
		Color chroma = Colors::Magenta;
	};
	class Substitution {
	public:
		Substitution(Color c, Color sub)
			:
			chroma(c),
			substitution(sub)
		{}
		void operator()(Color cSrc, int xDest, int yDest, Graphics& gfx) const {
			if (cSrc != chroma) {
				gfx.PutPixel(xDest, yDest, substitution);
			}
		}
	private:
		Color chroma = Colors::Magenta;
		Color substitution;
	};
	class Copy {
	public:
		void operator()(Color cSrc, int xDest, int yDest, Graphics& gfx) const {
			gfx.PutPixel(xDest, yDest, cSrc);
		}
	};
	class Ghost {
	public:
		Ghost(Color c, float inten)
			:
			chroma(c),
			intensity(inten)
		{}
		void operator()(Color cSrc, int xDest, int yDest, Graphics& gfx) const {
			if (cSrc != chroma) {
				const Color dstPixel = gfx.GetPixel(xDest, yDest);
				const Color blendedPixel = Color{
					unsigned char((1.0f - intensity) * cSrc.GetR() + intensity * dstPixel.GetR()),
					unsigned char((1.0f - intensity) * cSrc.GetG() + intensity * dstPixel.GetG()),
					unsigned char((1.0f - intensity) * cSrc.GetB() + intensity * dstPixel.GetB())
				};
				gfx.PutPixel(xDest, yDest, blendedPixel);
			}
		}
	private:
		float intensity;
		Color chroma = Colors::Magenta;
	};
}
