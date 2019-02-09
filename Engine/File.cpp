#include "File.h"

File::File(std::string name, float offset, RectI screenRegion)
	:
	ownName(name),
	offset(offset),
	font("Font.bmp")
{
	std::ifstream file(ownName);
	//test, if file exists																							|-> can lead to EXCEPTION
	if (!file)
	{
		std::string info = "Can't open file \"";	
		info += ownName;
		info += "\"";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
	}

	/************************************** LOADING FILE SETTINGS *********************************************************/
	{
		///adding setting names together && define functions														|-> ADD NEW SETTINGS HERE
		std::unordered_map<std::string, std::pair<std::function<void(std::ifstream&)>, bool>> settingNames;
		settingNames[axisColorSet].first = [this](std::ifstream& file) {
			file.unget();
			///red color value
			const unsigned char rc = toColorChar(file, "red axis", ownName);
			///green color value
			const unsigned char gc = toColorChar(file, "green axis", ownName);
			///blue color value
			const unsigned char bc = toColorChar(file, "blue axis", ownName);
			///assemble them in one color
			axisColor = Color(rc, gc, bc);
		};
		settingNames[graphColorSet].first = [this](std::ifstream& file) {
			file.unget();
			///red color value
			const char rc = toColorChar(file, "red graph", ownName);
			///green color value
			const char gc = toColorChar(file, "green graph", ownName);
			///blue color value
			const char bc = toColorChar(file, "blue graph", ownName);
			///assemble them in one color
			pixelColor = Color(rc, gc, bc);
		};
		settingNames[yNameSet].first = [this](std::ifstream& file) {
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
		};
		settingNames[timeName].first = [this](std::ifstream& file) {
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
		};
		settingNames[repeatingName].first = [this](std::ifstream& file) {
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
					info += ownName;
					info += "\":\n";
					info += toString(repeatVal);
					info += " (more than 10000)\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
			}
			catch (const std::exception&)
			{
				std::string info = "Bad repeating value in file \"";
				info += ownName;
				info += "\": ";
				info += repeatStr;
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
			}
		};

		for(std::pair<const std::string, std::pair<std::function<void(std::ifstream& file)>, bool>>& p : settingNames)
		{
			p.second.second = false;
		}

		size_t longestSetting = 0;
		for (const auto& p : settingNames)
		{
			longestSetting = std::max(longestSetting, p.first.size());
		}

		///reading out the settings
		bool endReached = false;

		do {
			std::string setting;
			///reading out the name of a setting && testing for end													|-> can lead to EXCEPTION
			for (char c = file.get(); !endReached && (settingNames.find(setting) == settingNames.end()); c = file.get())
			{
				if (file.eof())
				{
					std::string info = "Not enough data in file \"";				
					info += ownName;													
					info += "\": Reached end of file";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				setting += c;
				if (setting.size() > std::max(longestSetting, settingsEnd.size()))
				{
					std::string info = "Wrong spelling of setting name in file \"";
					info += ownName;													
					info += "\": ";
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
				if (settingNames.at(setting).second)
				{
					std::string info = "Double initialized setting in file \"";		
					info += ownName;													
					info += "\": ";
					info += setting;
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
				}
				///now: setting is not already initialized
				///'switch' on setting string																		|-> can lead to EXCEPTION
				settingNames.at(setting).first(file);
				settingNames.at(setting).second = true;
			}
			else
			{
				///checks, if all settings are initialized															|-> can lead to EXCEPTION	
				for (const std::pair<const std::string, std::pair<std::function<void(std::ifstream& file)>, bool>>& p : settingNames)
				{
					if (!p.second.second)
					{
						std::string info = "Not enough settings found in \"";
						info += ownName;
						info += "\":\nMissing setting: ";
						info += p.first;
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
					}
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
					info += ownName;
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

void File::Update(MouseController& mouseControl)
{
	graph.Update(mouseControl);
}

void File::Calculate(float dt)
{
	time += dt;
	const float delta_t = 1.0f / repeatVal;
	for (; delta_t < time; time -= delta_t)
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
	font.DrawText(yAxisName, Vei2(graph.GetScreenRegion().left - (int)offset, graph.GetScreenRegion().top - font.GetHeight()), axisColor, gfx);
	font.DrawText(timeVar, Vei2(graph.GetScreenRegion().right - 2 * (int)offset, graph.GetScreenRegion().bottom - (graph.IsNegative() ? graph.GetScreenRegion().GetHeight() / 2  - (int) offset : 0)), axisColor, gfx);
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
