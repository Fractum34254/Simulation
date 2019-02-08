#include "File.h"

File::File(std::string name, float offset, RectI screenRegion)
	:
	name(name),
	offset(offset),
	screenRegion(screenRegion)
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
					std::string r;
					int ri;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); (c != -1) && (c != ' '); c = file.get())
						{
							r += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							ri = std::stoi(r);
							if (0 > ri || 255 < ri)
							{
								std::string info = "Bad red axis color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(ri);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad red axis color value in file \"";
							info += name;
							info += "\": ";
							info += r;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char rc = ri;
					///green color value
					std::string g;
					int gi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); (c != -1) && (c != ' '); c = file.get())
						{
							g += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							gi = std::stoi(g);
							if (0 > gi || 255 < gi)
							{
								std::string info = "Bad green axis color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(gi);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad green axis color value in file \"";
							info += name;
							info += "\": ";
							info += g;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char gc = gi;
					///blue color value
					std::string b;
					int bi;
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
							b += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							bi = std::stoi(b);
							if (0 > bi || 255 < bi)
							{
								std::string info = "Bad blue axis color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(bi);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad blue axis color value in file \"";
							info += name;
							info += "\": ";
							info += b;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char bc = bi;
					///assemble them in one color
					axisColor = Color(rc, gc, bc);
				}
				else if (setting == graphColorSet)
				{
					file.unget();
					///red color value
					std::string r;
					int ri;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); (c != -1) && (c != ' '); c = file.get())
						{
							r += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							ri = std::stoi(r);
							if (0 > ri || 255 < ri)
							{
								std::string info = "Bad red graph color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(ri);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad red graph color value in file \"";
							info += name;
							info += "\": ";
							info += r;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char rc = ri;
					///green color value
					std::string g;
					int gi;
					{
						///test for blank space
						char c = file.get();
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						///read out r
						for (c = file.get(); (c != -1) && (c != ' '); c = file.get())
						{
							g += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							gi = std::stoi(g);
							if (0 > gi || 255 < gi)
							{
								std::string info = "Bad green graph color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(gi);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad green graph color value in file \"";
							info += name;
							info += "\": ";
							info += g;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char gc = gi;
					///blue color value
					std::string b;
					int bi;
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
							b += c;
						}
						///if conversion fails, a std::exception is thrown -> catch & throw own exception with more informations
						try
						{
							bi = std::stoi(b);
							if (0 > bi || 255 < bi)
							{
								std::string info = "Bad blue graph color value in file \"";
								info += name;
								info += "\":\n";
								info += toString(bi);
								info += " (not in range from 0 to 255)\n";
								throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
							}
						}
						catch (const std::exception&)
						{
							std::string info = "Bad blue graph color value in file \"";
							info += name;
							info += "\": ";
							info += b;
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
						}
					}
					char bc = bi;
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

	/************************************** LOADING START VARS *********************************************************/
	{
		bool endReached = false;
		int lineNmr = 1;
		do
		{
			///reading a line
			std::string line;
			for (char c = file.get(); !endReached && (c != '\n'); c = file.get())
			{
				if (line == headEnd)
				{
					endReached = true;
				}
				else if (c == -1 || file.eof())
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
			
			if(!endReached) {
				///calculate and save line and advance line number
				Parser::Calculate(line, vars, lineNmr++);
			}
		} while (!endReached);
	}
	/************************************** LOADING PROGRAM CODE *********************************************************/

	/************************************** EXECUTING START VARS *********************************************************/
}
