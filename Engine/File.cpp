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
		info += "\"";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}

	/************************************** LOADING FILE SETTINGS *********************************************************/
	{
		///adding setting names together && define functions														|-> ADD NEW SETTINGS HERE
		std::unordered_map<std::string, std::pair<std::function<void(std::ifstream&)>, bool>> settingNames;
		settingNames[axisColorSet].first = [this](std::ifstream& file) {
			file.unget();
			///red color value
			const unsigned char rc = PhilUtil::toColorChar(file, "red axis", ownName);
			///green color value
			const unsigned char gc = PhilUtil::toColorChar(file, "green axis", ownName);
			///blue color value
			const unsigned char bc = PhilUtil::toColorChar(file, "blue axis", ownName);
			///assemble them in one color
			axisColor = Color(rc, gc, bc);
		};
		settingNames[graphColorSet].first = [this](std::ifstream& file) {
			file.unget();
			///red color value
			const char rc = PhilUtil::toColorChar(file, "red graph", ownName);
			///green color value
			const char gc = PhilUtil::toColorChar(file, "green graph", ownName);
			///blue color value
			const char bc = PhilUtil::toColorChar(file, "blue graph", ownName);
			///assemble them in one color
			pixelColor = Color(rc, gc, bc);
		};
		settingNames[yNameSet].first = [this](std::ifstream& file) {
			file.unget();
			bool repeat = false;
			do {
				///test for blank space
				std::string yName;
				char c = file.get();
				while (c == ' ')
				{
					c = file.get();
				}
				file.unget();
				for (c = file.get(); (c != ' ') && (c != -1) && (c != '\n'); c = file.get())
				{
					yName += c;
				}
				yAxisNames.emplace_back(yName);
				if (c == ' ')
				{
					repeat = true;
				}
				else
				{
					repeat = false;
				}
			} while (repeat);
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
			}
			catch (const std::exception&)
			{
				std::string info = "Bad repeating value in file \"";
				info += ownName;
				info += "\": ";
				info += repeatStr;
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
					info += "\": Reached end of file";
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
				}
				setting += c;
				if (setting.size() > std::max(longestSetting, settingsEnd.size()))
				{
					std::string info = "Wrong spelling of setting name in file \"";
					info += ownName;													
					info += "\":\n";
					info += setting;
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
					throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
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
		for (int i = 0; i < yAxisNames.size(); i++)
		{
			const RectI rect = RectI({ screenRegion.left + (i % xDir) * (deltaX + off), screenRegion.top + (i / xDir) * (deltaY+off) }, deltaX, deltaY);
			graphs.emplace_back(std::make_unique<Graph>(Graph(rect, offset, axisColor, pixelColor, yAxisNames.at(i), font)));
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
					info += "\": Reached end of file";
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
				parser.Calculate(line, vars, lineNmr++);
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
	/************************************** ASSIGNING VARIABLES TO GRAPH *********************************************************/
	{
		for (int i = 0; i < yAxisNames.size(); i++)
		{
			graphs.at(i)->PutData(vars.at(timeVar), vars.at(yAxisNames.at(i)));
		}
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
		iconbars.at(i).AddIcon(std::make_unique<BackwardIcon>(), [this]() {SetRepeatValue((int)((float)GetRepeatVal() * 0.8f)); }, 5);
		iconbars.at(i).AddIcon(std::make_unique<ForwardIcon>(), [this]() {SetRepeatValue((int)((float)GetRepeatVal() * 1.2f)); }, 6);
		iconbars.at(i).AddIcon(std::make_unique<CloseIcon>("Close " + ownName + ": " + graphs.at(i)->GetYAxisName()), [i, this]() {SetVisible(i, false); }, 8);
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
	const float delta_t = 1.0f / (float)repeatVal;
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
			graphs.at(i)->Draw(ownName, gfx);
			font.DrawText(yAxisNames.at(i), Vei2(graphs.at(i)->GetScreenRegion().left - (int)offset, graphs.at(i)->GetScreenRegion().top - font.GetHeight() - (int)offset), axisColor, gfx);
			font.DrawText(timeVar, Vei2(graphs.at(i)->GetScreenRegion().right - 2 * (int)offset, graphs.at(i)->GetScreenRegion().bottom - (graphs.at(i)->IsNegative() ? graphs.at(i)->GetScreenRegion().GetHeight() / 2 - (int)offset : 0)), axisColor, gfx);
			iconbars.at(i).Draw(gfx);
		}
	}
}

void File::SetRepeatValue(int rv)
{
	repeatVal = std::min(std::max(rv, 5), 1200);
	std::stringstream tempSS;
	tempSS << ownName << ":\nNow calculating " << repeatVal << "/s";
	events.Add(tempSS.str());
}

int File::GetRepeatVal() const
{
	return repeatVal;
}

void File::Save() const
{
	std::string fname = "output - ";
	fname += ownName;
	std::ofstream file(fname);
	std::vector<std::unique_ptr<std::unordered_map<int, std::pair<float, float>>>> data;
	for (int i = 0; i < graphs.size(); i++)
	{
		data.emplace_back(std::move(graphs.at(i)->GetData()));
	}
	const int cur = (const int)data.at(0)->size();
	for (int i = 0; i < cur; i++)
	{
		const float t = data.at(0)->at(i).first;
		std::vector<float> y;
		for (int k = 0; k < graphs.size(); k++)
		{
			y.emplace_back(data.at(k)->at(i).second);
		}
		file << "t: " << PhilUtil::Crop(t, cropVal) << " ";
		for (int k = 0; k < graphs.size(); k++)
		{
			file << graphs.at(k)->GetYAxisName() << ": " << PhilUtil::Crop(y.at(k), cropVal) << " ";
		}
		file << "\n";
	}
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
			parser.Calculate(line, vars, lineNmr++);
		}
	} while (!endReached);
	for (int i = 0; i < yAxisNames.size(); i++)
	{
		graphs.at(i)->PutData(vars.at(timeVar), vars.at(yAxisNames.at(i)));
	}
}
