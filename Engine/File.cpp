#include "File.h"

File::File(std::string name, float offset, RectI screenRegion, Eventmanager& e)
	:
	ownName(name),
	offset(offset),
	font("Bitmaps\\Font.bmp"),
	events(e)
{
	std::ifstream file(ownName);
	//test, if file exists																							|-> can lead to EXCEPTION
	if (!file)
	{
		std::string info = "Can't open file \"";	
		info += ownName;
		info += "\"\n";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}

	/************************************** LOADING FILE SETTINGS *********************************************************/
	{
		///adding setting names together && define functions														|-> ADD NEW SETTINGS HERE
		std::unordered_map<std::string, std::pair<std::function<void(std::ifstream&)>, bool>> settingNames;
		settingNames[axisColorSet].first = [this](std::ifstream& file) {
			try
			{
				///red color value
				const unsigned char rc = PhilUtil::toColorChar(file, "red axis", ownName);
				///green color value
				const unsigned char gc = PhilUtil::toColorChar(file, "green axis", ownName);
				///blue color value
				const unsigned char bc = PhilUtil::toColorChar(file, "blue axis", ownName);
				///assemble them in one color
				axisColor = Color(rc, gc, bc);
			}
			catch (const Exception& e)
			{
				throw e;
			}
			catch (const std::exception& e)
			{
				const char* textCStr = e.what();
				const std::string textStr(textCStr);
				std::string info = "Bad axis color in file \"";
				info += ownName;
				info += "\":\nCaught a std::exception:\n\n";
				info += textStr;
				info += "\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			catch (...)
			{
				std::string info = "Bad axis color in file \"";
				info += ownName;
				info += "\":\nCaught a non-defined exception.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		};
		settingNames[graphColorSet].first = [this](std::ifstream& file) {
			try
			{
				for (char c = file.get(); c != '\n'; c = file.get())
				{
					file.unget();
					///red color value
					const char rc = PhilUtil::toColorChar(file, "red graph", ownName);
					///green color value
					const char gc = PhilUtil::toColorChar(file, "green graph", ownName);
					///blue color value
					const char bc = PhilUtil::toColorChar(file, "blue graph", ownName);
					///assemble them in one color
					pixelColors.emplace_back(Color(rc, gc, bc));
					///erase all blank spaces
					c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
				}
			}
			catch (const Exception& e)
			{
				throw e;
			}
			catch (const std::exception& e)
			{
				const char* textCStr = e.what();
				const std::string textStr(textCStr);
				std::string info = "Bad graph colors in file \"";
				info += ownName;
				info += "\":\nCaught a std::exception:\n\n";
				info += textStr;
				info += "\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			catch (...)
			{
				std::string info = "Bad graph colors in file \"";
				info += ownName;
				info += "\":\nCaught a non-defined exception.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		};
		settingNames[yNameSet].first = [this](std::ifstream& file) {
			try
			{
				bool repeat = false;
				///count braces
				int braceNmr = 0;
				yAxisNames.resize(braceNmr + 1);
				do {
					///erase blank spaces
					char c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					///brace
					if (c != '(')
					{
						std::string info = "Bad y axis names in file \"";
						info += ownName;
						info += "\": Missing opening parenthesis\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
					}
					///read out the brace
					for (c = file.get(); c != ')'; c = file.get())
					{
						std::string yName;
						///erase blank spaces
						while (c == ' ')
						{
							c = file.get();
						}
						file.unget();
						for (c = file.get(); (c != ' ') && (c != -1) && (c != '\n') && (c != ')'); c = file.get())
						{
							yName += c;
						}
						file.unget();
						///test, if line ends
						if ((c == '\n') || (c == -1))
						{
							std::string info = "Bad y axis names in file \"";
							info += ownName;
							info += "\": Missing closing parenthesis\n";
							throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
						}
						///c is now definitly == ' '
						yAxisNames.at(braceNmr).emplace_back(yName);
					}
					///erase blank spaces
					c = file.get();
					while (c == ' ')
					{
						c = file.get();
					}
					file.unget();
					if (c == '(')
					{
						repeat = true;
						braceNmr++;
						yAxisNames.resize(braceNmr + 1);
					}
					else if ((c != '\n') && (c != -1))
					{
						std::string info = "Bad y axis names in file \"";
						info += ownName;
						info += "\": Missing parenthesis\n(found characters out of braces)\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
					}
					else
					{
						repeat = false;
					}
				} while (repeat);
				file.get();
			}
			catch (const Exception& e)
			{
				throw e;
			}
			catch (const std::exception& e)
			{
				const char* textCStr = e.what();
				const std::string textStr(textCStr);
				std::string info = "Bad y axis names in file \"";
				info += ownName;
				info += "\":\nCaught a std::exception:\n\n";
				info += textStr;
				info += "\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			catch (...)
			{
				std::string info = "Bad y axis names in file \"";
				info += ownName;
				info += "\":\nCaught a non-defined exception.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		};
		settingNames[timeName].first = [this](std::ifstream& file) {
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
			if (timeVar == "")
			{
				std::string info = "Bad time name in file \"";
				info += ownName;
				info += "\": <No string could be read>\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		};
		settingNames[repeatingName].first = [this](std::ifstream& file) {
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
			///if conversion fails, a std::exception is thrown -> catch & throw own exception with more information
			try
			{
				repeatVal = std::stof(repeatStr);
			}
			catch (const std::exception&)
			{
				if (repeatStr == "")
				{
					repeatStr = "<No string could be read>";
				}
				std::string info = "Bad repeating value in file \"";
				info += ownName;
				info += "\": ";
				info += repeatStr;
				info += "\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
					info += "\": Reached end of file\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
				}
				if (c != '\n')
				{
					setting += c;
				}
				if (setting.size() > std::max(longestSetting, settingsEnd.size()))
				{
					std::string info = "Wrong spelling of setting name in file \"";
					info += ownName;													
					info += "\":\n";
					info += setting;
					info += "\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
					info += "\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
				}
				///now: setting is not already initialized
				///'switch' on setting string																		|-> can lead to EXCEPTION
				settingNames.at(setting).first(file);
				settingNames.at(setting).second = true;
				///erase all blank spaces and returns
				char c = file.get();
				while (c == ' ')
				{
					c = file.get();
				}
				if (c != '\n')
				{
					file.unget();
				}
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
						info += "\n";
						throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
					}
				}
			}
		} while (!endReached);
		///calculate screen regions for graphs
		const int off = 90;
		const auto pair = PhilUtil::squareFactor((int)yAxisNames.size());
		const int yDir = pair.first;
		const int xDir = pair.second;
		const int deltaY = screenRegion.GetHeight() / yDir - off;
		const int deltaX = screenRegion.GetWidth() / xDir - off;
		int colorCount = 0;
		///check, if there are enough colors for all graphs
		{
			const int colorSize = (int)pixelColors.size();
			int graphNumber = 0;
			for (const auto& n : yAxisNames)
			{
				graphNumber += (int)n.size();
			}
			if (colorSize < graphNumber)
			{
				std::string info = "Not enough graph colors found in \"";
				info += ownName;
				info += "\"!\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			else if (colorSize > graphNumber)
			{
				std::string info = "Too many graph colors found in \"";
				info += ownName;
				info += "\"!\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		}


		//initialize graphs
		for (int i = 0; i < yAxisNames.size(); i++)
		{
			std::vector<Color> pixCol;
			int j = 0;
			for (; j < yAxisNames.at(i).size(); j++)
			{
				pixCol.emplace_back(pixelColors.at(colorCount + j));
			}
			colorCount += j;
			const RectI rect = RectI({ screenRegion.left + (i % xDir) * (deltaX + off), screenRegion.top + (i / xDir) * (deltaY+off) }, deltaX, deltaY);
			graphs.emplace_back(std::make_unique<Graph>(Graph(rect, offset, axisColor, pixCol, yAxisNames.at(i), font)));
		}
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
					info += "\": Reached end of file\n";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
				parser.Calculate(line, vars, ownName, lineNmr++);
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
	/************************************** CALCULATING CODE ONCE *********************************************************/
	{
		CalculateOnce();
	}

	/************************************** BUTTON SETUP *********************************************************/

	for (int i = 0; i < graphs.size(); i++)
	{
		iconbars.emplace_back();
		iconbars.at(i).AddIcon(std::make_unique<SaveIcon>(), [this]() {Save(); }, 0);
		iconbars.at(i).AddIcon(std::make_unique<RefreshIcon>(), [i, this]() {RefreshGraph(i); }, 1);
		///DO NOT MODIFY THE PLACE OF THE PLAY && PAUSE BUTTON (else set it up new in the Update() function)
		iconbars.at(i).AddIcon(std::make_unique<PauseIcon>(), [this]() {SetCalculating(false); }, 3);
		iconbars.at(i).AddIcon(std::make_unique<PlayIcon>(), [this]() {SetCalculating(true); }, 3);
		iconbars.at(i).AddIcon(std::make_unique<BackwardIcon>(), [this]() {SpeedDown(); }, 5);
		iconbars.at(i).AddIcon(std::make_unique<ForwardIcon>(), [this]() {SpeedUp(); }, 6);
		iconbars.at(i).AddIcon(std::make_unique<CloseIcon>("Close " + ownName + ": " + graphs.at(i)->GetYAxisName()), [i, this]() {SetVisible(i, false); }, 8);
		iconbars.at(i).SetAllTextAlignments(Icon::Alignment::centered);
	}
	SetUpButtons();
}

void File::Update(MouseController& mouseControl)
{
	for (int i = 0; i < graphs.size(); i++)
	{
		if (graphs.at(i)->IsVisible())
		{
			graphs.at(i)->Update(mouseControl);
			iconbars.at(i).Update(mouseControl);
		}
	}
	SetUpButtons();
	for (auto& bar : iconbars)
	{
		if (calculating)
		{
			bar.SetActive(3, 0);
		}
		else
		{
			bar.SetActive(3, 1);
		}
	}
}

void File::Calculate(float dt)
{
	if (!calculating || !AnyVisible())
	{
		return;
	}
	time += dt;
	const float delta_t = 1.0f / repeatVal;
	for (; delta_t < time; time -= delta_t)
	{
		CalculateOnce();
	}
}

void File::Draw(Graphics & gfx) const
{
	for (int i = 0; i < yAxisNames.size(); i++)
	{
		if (graphs.at(i)->IsVisible())
		{
			graphs.at(i)->Draw(ownName, timeVar, gfx);
			iconbars.at(i).Draw(gfx);
		}
	}
}

void File::SetRepeatValue(float rv)
{
	repeatVal = std::min(std::max(rv, 5.0f), 1200.0f);
	std::stringstream tempSS;
	tempSS << ownName << ":\nNow calculating " << PhilUtil::Round(repeatVal,0) << "/s";
	events.Add(tempSS.str());
}

float File::GetRepeatVal() const
{
	return repeatVal;
}

void File::SpeedUp()
{
	SetRepeatValue(GetRepeatVal() * 1.2f);
}

void File::SpeedDown()
{
	SetRepeatValue(GetRepeatVal() / 1.2f);
}

void File::Save() const
{
	//output file name
	std::string fname = "output - ";
	fname += ownName;
	std::ofstream file(fname);
	//add all data maps of all graphs together in 'data'
	std::vector<std::unique_ptr<std::unordered_map<int, std::pair<float, float>>>> data;
	for (int i = 0; i < graphs.size(); i++)
	{
		const size_t max = graphs.at(i)->GetData().size();
		for (int j = 0; j < max; j++)
		{
			data.emplace_back(std::move(graphs.at(i)->GetData().at(j)));
		}
	}
	//get size of unordered maps
	const int dataSize = (const int)data.at(0)->size();
	//get number of unordered maps
	const int mapCount = (const int)data.size();
	//loop through all data points in the maps
	for (int i = 0; i < dataSize; i++)
	{
		//count current y names
		int yNames = 0;
		//get time
		const float t = data.at(0)->at(i).first;
		//write time
		file << timeVar << ": " << PhilUtil::Crop(t, cropVal) << " ";
		//loop through graphs
		for (int k = 0; k < graphs.size(); k++)
		{
			//loop through y names
			for (int j = 0; j < graphs.at(k)->GetYAxisNames().size(); j++)
			{
				//write y values
				file << graphs.at(k)->GetYAxisNames().at(j) << ": " << PhilUtil::Crop(data.at(yNames)->at(i).second, cropVal) << " ";
				yNames++;
			}
		}
		//put new line character
		file << "\n";
	}
	//create save event
	std::stringstream tempSS;
	tempSS << "Successfully saved " << ownName << "!";
	events.Add(tempSS.str());
}

void File::RefreshAll()
{
	for (auto& graph : graphs)
	{
		graph->Refresh();
	}
	SetUpButtons();
	std::stringstream tempSS;
	tempSS << ownName << ":\nRefreshed all graphs.";
	events.Add(tempSS.str());
}

void File::RefreshGraph(int i)
{
	graphs.at(i)->Refresh();
	SetUpButtons();
	std::stringstream tempSS;
	tempSS << ownName << ":\nRefreshed graph " << graphs.at(i)->GetYAxisName();
	events.Add(tempSS.str());
}

void File::SetCalculating(bool b)
{
	calculating = b;
}

bool File::GetCalculating() const
{
	return calculating;
}

void File::ToggleVisible()
{
	if (AllVisible())
	{
		for (auto& pGraph : graphs)
		{
			pGraph->SetVisible(false);
		}
	}
	else
	{
		for (auto& pGraph : graphs)
		{
			pGraph->SetVisible(true);
		}
	}
}

void File::SetVisible(int graph, bool b)
{
	graphs.at(graph)->SetVisible(b);
}

void File::CloseAll()
{
	for (auto& pGraph : graphs)
	{
		pGraph->SetVisible(false);
	}
	std::stringstream tempSS;
	tempSS << "Closed " << ownName;
	events.Add(tempSS.str());
}

bool File::AllVisible() const
{
	for (auto& pGraph : graphs)
	{
		if (!pGraph->IsVisible())
		{
			return false;
		}
	}
	return true;
}

bool File::AnyVisible() const
{
	for (auto& pGraph : graphs)
	{
		if (pGraph->IsVisible())
		{
			return true;
		}
	}
	return false;
}

std::string File::GetName() const
{
	return ownName;
}

void File::SetUpButtons()
{
	for (int i = 0; i < graphs.size(); i++)
	{
		iconbars.at(i).SetPos({graphs.at(i)->GetScreenRegion().left + graphs.at(i)->GetScreenRegion().GetWidth() / 2 - iconbars.at(i).GetPixelWidth() / 2, graphs.at(i)->GetScreenRegion().bottom + font.GetHeight() + (int)offset });
	}
}

void File::CalculateOnce()
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
			parser.Calculate(line, vars, ownName, lineNmr++);
		}
	} while (!endReached);

	for (int j = 0; j < yAxisNames.size(); j++) ///which graph?
	{
		for (int i = 0; i < yAxisNames.at(j).size(); i++) ///which y variable?
		{
			try 
			{
				graphs.at(j)->PutData(vars.at(timeVar), vars.at(yAxisNames.at(j).at(i)), i);
			}
			catch (const Exception& e)
			{
				throw e;
			}
			catch (const std::exception& e)
			{
				const char* textCStr = e.what();
				const std::string textStr(textCStr);
				std::string info = "Something went wrong calculating \"";
				info += ownName;
				info += "\":\nCaught a std::exception:\n\n";
				info += textStr;
				info += "\n\nProbably uninitialized variable!\nCheck your y axis names.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			catch (...)
			{
				std::string info = "Something went wrong calculating \"";
				info += ownName;
				info += "\":\nCaught a non-defined exception.\nProbably uninitialized variable!\nCheck your y axis names.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
		}
	}
}
