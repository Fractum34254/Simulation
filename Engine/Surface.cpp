#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>

Surface::Surface(const std::string & filename)
{
	OutputDebugStringA("Opened file.\n");

	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		std::string info = "Could not open the bitmap named \"";
		info += filename;
		info += "\"!";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}

	BITMAPFILEHEADER bmFileHeader;
	file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));

	BITMAPINFOHEADER bmInfoHeader;
	file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

	assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
	assert(bmInfoHeader.biCompression == BI_RGB);

	const bool bits = (bmInfoHeader.biBitCount == 32);
	height = bmInfoHeader.biHeight;
	int yStart = height - 1;
	int yEnd = -1;
	int dy = -1;
	if (bmInfoHeader.biHeight < 0) {
		height = -height;
		yStart = 0;
		yEnd = height;
		dy = 1;
	}
	width = bmInfoHeader.biWidth;
	pixels.resize(width * height);

	int padding = 0;
	file.seekg(bmFileHeader.bfOffBits);
	if (!bits) {
		padding = (4 - (width * 3) % 4) % 4;
	}

	for (int y = yStart; y != yEnd; y += dy) {
		for (int x = 0; x < width; x++) {
			const unsigned char b = file.get();
			const unsigned char g = file.get();
			const unsigned char r = file.get();
			if (bits) {
				const unsigned char a = file.get();
				PutPixel(x, y, { a,r,g,b });
			}
			else {
				PutPixel(x, y, { r,g,b });
			}
		}
		file.seekg(padding, std::ios::cur);
	}
}

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	pixels(width * height)
{
}

Surface::Surface(Surface && donor)
{
	*this = std::move(donor);
	donor.width = 0;
	donor.height = 0;
}

Surface & Surface::operator=(Surface && rhs)
{
	width = rhs.width;
	height = rhs.height;
	pixels = std::move(rhs.pixels);
	rhs.width = 0;
	rhs.height = 0;
	return *this;
}

void Surface::PutPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	pixels.data()[y * width + x] = c;
}

Color Surface::GetPixel(int x, int y) const
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	return pixels.data()[y * width + x];
}

int Surface::GetWidth() const
{
	return width;
}

int Surface::GetHeight() const
{
	return height;
}

RectI Surface::GetRect() const
{
	return {0, width, 0 , height};
}

void Surface::Fill(Color c)
{
	std::fill(pixels.begin(), pixels.begin() + width * height, c);
}

const Color* Surface::Data() const
{
	return pixels.data();
}
