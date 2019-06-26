#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "ChiliException.h"


class PhilUtil
{
public:
	class Exception : public ChiliException
	{
	public:
		using ChiliException::ChiliException;
		virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
		virtual std::wstring GetExceptionType() const override { return L"Utility Exception"; }
	};
	template<typename T>
	static std::string Crop(T in, int n)
	{
		std::stringstream s_in;
		s_in << in;
		std::string sstr = s_in.str();
		while (sstr.length() < n)
		{
			sstr += " ";
		}
		while (sstr.length() > n)
		{
			sstr.pop_back();
		}
		return sstr;
	}
	static float Round(float f, int n)
	{
		const float outF = f * (float)std::pow(10.0, (double)n);
		const int outInt = (int)outF;
		const float diff = outF - (float)outInt;
		if (diff >= 0.5f)
		{
			return ((float)(outInt + 1)) / ((float)std::pow(10.0, (double)n));
		}
		else
		{
			return ((float)outInt) / ((float)std::pow(10.0, (double)n));
		}
	}
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
	static bool isFloat(std::string myString) {
		std::istringstream iss(myString);
		float f;
		iss >> std::noskipws >> f;	// noskipws considers leading whitespace invalid
									// Check the entire string was consumed and if either failbit or badbit is set
									// thx to: Bill the Lizard from stackoverflow.com
		return iss.eof() && !iss.fail();
	}
	static double top(double in)
	{
		///calculating tens potency floor
		int f = 0;
		if (in >= 1.0f)
		{
			while (std::pow(10, f) < in)
			{
				f++;
			}
			f--;
		}
		else
		{
			while (std::pow(10, f) >= in)
			{
				f--;
			}
		}
		///calculating factor in front of tens potency
		int n = 1;
		const double floor = std::pow(10, f);
		while (floor * (double)n < in)
		{
			n++;
		}
		return (double)n * floor;
	}
	static std::string toString(int i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}
	static std::pair<int, int> squareFactor(int number)
	{
		const float root = std::sqrtf((float)number);
		int rootFloor = (int)root;
		for (; number % rootFloor != 0; rootFloor--);
		return std::make_pair(rootFloor, number / rootFloor);
	}
	static char toColorChar(std::ifstream& file, const std::string colorName, const std::string& fileName)
	{
		std::string colorStr;
		int colorInt;
		{
			///test for blank space
			char c = file.get();
			while (c == ' ')
			{
				c = file.get();
			}
			file.unget();
			///read out r
			for (c = file.get(); (c != -1) && (c != ' ') && (c != '\n'); c = file.get())
			{
				colorStr += c;
			}
			///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
			try
			{
				colorInt = std::stoi(colorStr);
				if (0 > colorInt || 255 < colorInt)
				{
					std::string info = "Bad ";
					info += colorName;
					info += " color value in file \"";
					info += fileName;
					info += "\":\n";
					info += toString(colorInt);
					info += " (not in range from 0 to 255)\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
			}
			catch (const std::exception&)
			{
				std::string info = "Bad ";
				info += colorName;
				info += " color value in file \"";
				info += fileName;
				info += "\":\n";
				info += colorStr;
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
		}
		file.unget();
		return colorInt;
	}
	template<typename T, typename U>
	static void CopyUnorderedMap(const std::unordered_map<T, U>& input, std::unordered_map<T, U>& output)
	{
		try
		{
			for (const auto& data : input)
			{
				output[data.first] = data.second;
			}
		}
		catch (const std::exception&)
		{
			std::string info = "Copying of unordered map failed.\ncaught std::exception";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
		catch (...)
		{
			std::string info = "Copying of unordered map failed.\ncaught something, but no std::exception";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
		}
	}
};

