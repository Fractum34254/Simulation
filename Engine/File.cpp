#include "File.h"

File::File(std::string name, float offset, RectI screenRegion)
	:
	name(name),
	offset(offset),
	screenRegion(screenRegion),
	font("Font.bmp")
{
	std::ifstream file(name);
	//test, if file exists																							|-> can lead to EXCEPTION
	if (!file)
	{
		std::string info = "Can't open file \"";	
		info += name;								
		info += "\"";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
	}

	/************************************** LOADING FILE SETTINGS *********************************************************/
	{
		///adding setting names together																			|-> ADD NEW SETTINGS HERE
		std::vector<std::string> settingNames;
		settingNames.emplace_back(axisColorSet);
		settingNames.emplace_back(graphColorSet);
		settingNames.emplace_back(yNameSet);
		settingNames.emplace_back(timeName);
		settingNames.emplace_back(repeatingName);
		///vector of bool to keep track whether a variable is initialized or not
		std::vector<bool> settingInit;
		for (size_t i = 0; i < settingNames.size(); i++)
		{
			settingInit.emplace_back(false);
		}

		///reading out the settings
		bool endReached = false;

		do {
			std::string setting;
			///reading out the name of a setting && testing for end													|-> can lead to EXCEPTION
			for (char c = file.get(); !endReached && !isInVector(settingNames, setting); c = file.get())
			{
				if (file.eof())
				{
					std::string info = "Not enough data in file \"";				///standart-syntax
					info += name;													///file with incorrect settings
					info += "\": Reached end of file";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				setting += c;
				if (setting.size() > std::max(getLongest(settingNames), settingsEnd.size()))
				{
					std::string info = "Wrong spelling of setting name in file ";	///standart-syntax
					info += name;													///file with incorrect settings
					info += ": ";
					info += setting;
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				if (setting == settingsEnd)
				{
					endReached = true;
				}
			}
			///now name of setting is in 'setting'
			if (!endReached && !file.eof())
			{
				///if init is already true, setting was already initialized											|-> can lead to EXCEPTION
				if (settingInit.at(findInVector(settingNames, setting)))
				{
					std::string info = "Double initialized setting in file \"";		///standart-syntax
					info += name;													///file with incorrect settings
					info += "\": ";
					info += setting;
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				///now: setting is not already initialized
				///'switch' on different strings -> test through ALL setting names									|-> can lead to EXCEPTION
				///																									|-> ADD NEW SETTINGS HERE
				if (setting == axisColorSet)
				{
					file.unget();
					///red color value
					const unsigned char rc = toColorChar(file, "red axis", name);
					///green color value
					const unsigned char gc = toColorChar(file, "green axis", name);
					///blue color value
					const unsigned char bc = toColorChar(file, "blue axis", name);
					///assemble them in one color
					axisColor = Color(rc, gc, bc);
				}
				else if (setting == graphColorSet)
				{
					file.unget();
					///red color value
					const char rc = toColorChar(file, "red graph", name);
					///green color value
					const char gc = toColorChar(file, "green graph", name);
					///blue color value
					const char bc = toColorChar(file, "blue graph", name);
					///assemble them in one color
					pixelColor = Color(rc, gc, bc);
				}
				else if (setting == yNameSet)
				{
					file.unget();
					///test for blank space
					char c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
					for (c = file.get(); (c != ' ') && (c != -1) && (c != '\n'); c = file.get())
					{
						yAxisName += c;
					}
				}
				else if (setting == timeName)
				{
					file.unget();
					///test for blank space
					char c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
					for (c = file.get(); (c != ' ') && (c != -1) && (c != '\n'); c = file.get())
					{
						timeVar += c;
					}
				}
				else if (setting == repeatingName)
				{
					file.unget();
					///test for blank space
					char c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
					std::string repeatStr;
					for (c = file.get(); (c != ' ') && (c != -1) && (c != '\n'); c = file.get())
					{
						repeatStr += c;
					}
					///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
					try
					{
						repeatVal = std::stoi(repeatStr);
						if (repeatVal > 10000)
						{
							std::string info = "Too much repeating per second in file \"";
							info += name;
							info += "\":\n";
							info += toString(repeatVal);
							info += " (more than 10000)\n";
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					catch (const std::exception&)
					{
						std::string info = "Bad repeating value in file \"";
						info += name;
						info += "\": ";
						info += repeatStr;
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
					}
				}
				settingInit.at(findInVector(settingNames, setting)) = true;
			}
			else
			{
				///checks, if all settings are initialized															|-> can lead to EXCEPTION	
				if (!std::all_of(settingInit.begin(), settingInit.end(), [](bool b) {return b; }))
				{
					std::string info = "Not enough settings found in \"";
					info += name;
					info += "\"";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
			}
		} while (!endReached);
		///initialize graph
		graph = Graph(screenRegion, axisColor, pixelColor, yAxisName);
	}

	/************************************** LOADING  + EXECUTING START VARS ************************************************/
	{
		bool endReached = false;
		int lineNmr = 1;
		do
		{
			///reading a line
			std::string line;
			for (char c = file.get(); !endReached && (c != '\n'); c = file.get())
			{

				if (c == -1 || file.eof())
				{
					std::string info = "Not enough data in file \"";	
					info += name;										
					info += "\": Reached end of file";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				else {
					line += c;
				}
			}
			///test, if start vars are finished
			if (line == headEnd)
			{
				endReached = true;
			}
			else {
				///calculate and save line and advance line number
				Parser::Calculate(line, vars, lineNmr++);
			}
		} while (!endReached);
	}
	/************************************** LOADING PROGRAM CODE ******************************************************************/
	{
		bool endReached = false;
		int lineNmr = 1;
		do
		{
			///reading a line
			std::string line;
			for (char c = file.get(); !endReached && (c != '\n'); c = file.get())
			{
				if (c == -1 || file.eof())
				{
					endReached = true;
				}
				else {
					line += c;
				}
			}
			code += line;
			code += "\n";
		} while (!endReached);
	}
	/************************************** ASSIGNING VARIABLES TO GRAPH *********************************************************/
	{
		graph.PutData(vars.at(timeVar), vars.at(yAxisName));
	}
}

void File::Calculate(float dt)
{
	for (float i = 0.0f; i < dt * repeatVal; i++)
	{
		bool endReached = false;
		int lineNmr = 1;
		std::stringstream codeFile(code);
		do
		{
			///reading a line
			std::string line;
			for (char c = codeFile.get(); !endReached && (c != '\n'); c = codeFile.get())
			{

				///test, if start vars are finished
				if (codeFile.eof())
				{
					endReached = true;
				}
				else {
					line += c;
				}
			}
			if (!endReached) {
				///calculate and save line and advance line number
				Parser::Calculate(line, vars, lineNmr++);
			}
		} while (!endReached);
		graph.PutData(vars.at(timeVar), vars.at(yAxisName));
	}
}

void File::Draw(Graphics & gfx) const
{
	graph.Draw(font, gfx);
	font.DrawText(yAxisName, Vei2(screenRegion.left - (int)offset, screenRegion.top - font.GetHeight()), axisColor, gfx);
	font.DrawText(timeVar, Vei2(screenRegion.right - 2 * (int)offset, screenRegion.bottom - (graph.IsNegative() ? screenRegion.GetHeight() / 2  - (int) offset : 0)), axisColor, gfx);
}

char File::toColorChar(std::ifstream & file, const std::string colorName, const std::string& fileName)
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
	return colorInt;
}
