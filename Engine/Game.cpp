/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "SpriteEffect.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	mouseControl(wnd.mouse),
	font(IDB_BITMAP1)
{
	//Open settings file (list of files)
	std::ifstream settings(settingsFileName);
	if (!settings)
	{
		std::string info = "Can't open file \"";
		info += settingsFileName;
		info += "\": File not found";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	if (settings.eof())
	{
		std::string info = "Empty file \"";
		info += settingsFileName;
		info += "\"";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
	}
	//assembling list of files
	std::vector<std::string> names;
	while (!settings.eof())
	{
		std::string temp;
		settings >> temp;
		names.emplace_back(temp);
	}
	const int right = gfx.GetScreenRect().right + 50;
	const int bottom = gfx.GetScreenRect().bottom + 30;
	const RectI fileRect = {left, right, top, bottom};
	files.reserve(maxFiles);
	for (int i = 0; i < names.size(); i++)
	{
		files.emplace_back(std::make_unique<File>(names.at(i), (float)offset, fileRect, events));
	}

	//top left Iconbar
	graphIconbar.SetPos({ 3,3 });
	for (auto& file : files)
	{
		graphIconbar.AddIcon(std::make_unique<GraphIcon>(file->GetName()), [&file]() {
			file->ToggleVisible();
		});
	}

	//top middle Iconbar
	settingsIconbar.AddIcon(std::make_unique<GraphIcon>("Toggle all"), [this]() {				/* 0 */
		for (auto& file : files)
		{
			file->ToggleVisible();
		}});
	settingsIconbar.AddVoid();																	/* 1 */
	settingsIconbar.AddIcon(std::make_unique<SaveIcon>("Save all"), [this]() {					/* 2 */
		for (const auto& file : files)
		{
			file->Save();
		}});	
	settingsIconbar.AddIcon(std::make_unique<RefreshIcon>("Refresh all"), [this]() {			/* 3 */
		for (auto& file : files)
		{
			file->RefreshAll();
		}});
	settingsIconbar.AddVoid();																	/* 4 */
	settingsIconbar.AddIcon(std::make_unique<PlayIcon>("Continue all"), [this]() {				/* 5 */
		for (auto& file : files)
		{
			file->SetCalculating(true);
		}});
	settingsIconbar.AddIcon(std::make_unique<PauseIcon>("Pause all"), [this]() {				/* 6 */
		for (auto& file : files)
		{
			file->SetCalculating(false);
		}});
	settingsIconbar.AddVoid();																	/* 7 */
	settingsIconbar.AddIcon(std::make_unique<CloseIcon>("Close all"), [this]() {				/* 8 */
		for (auto& file : files)
		{
			file->CloseAll();
		}});
	settingsIconbar.SetAllTextAlignments(Icon::Alignment::centered);
	settingsIconbar.SetPos({ Graphics::ScreenWidth / 2 - settingsIconbar.GetPixelWidth() / 2, 3 });

	//top right close button + reload button
	closeIcon.AddIcon(std::make_unique<ReloadIcon>("Reload 'files.txt'"), [&wnd, this]() {
		//Open settings file (list of files)
		std::ifstream settings(settingsFileName);
		if (!settings)
		{
			std::string info = "Can't open file \"";
			info += settingsFileName;
			info += "\": File not found";
			throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
		}
		//assembling list of all files
		std::vector<std::string> names;
		while (!settings.eof())
		{
			std::string temp;
			settings >> temp;
			names.emplace_back(temp);
		}
		//delete already existing files
		names.erase(std::remove_if(names.begin(), names.end(), [this](std::string s)
		{
			for (int i = 0; i < files.size(); i++)
			{
				if (files.at(i)->GetName() == s)
				{
					return true;
				}
			}
			return false;
		}), names.end());
		const int right = gfx.GetScreenRect().right + 50;
		const int bottom = gfx.GetScreenRect().bottom + 30;
		const RectI fileRect = { left, right, top, bottom };
		const int actFiles = (int)files.size();
		for (int i = 0; i < names.size() && files.size() < maxFiles; i++)
		{
			files.emplace_back(std::make_unique<File>(names.at(i), (float)offset, fileRect, events));
		}
		//add new files to top left Iconbar
		for (int i = actFiles; i < files.size(); i++)
		{
			graphIconbar.AddIcon(std::make_unique<GraphIcon>(files.at(i)->GetName()), [this, i]() {
				files.at(i)->ToggleVisible();
			});
		}
	});
	closeIcon.AddIcon(std::make_unique<CloseIcon>("Close program"), [&wnd, this]() {
		wnd.Kill(); 
	});
	closeIcon.SetAllTextAlignments(Icon::Alignment::right);
	closeIcon.SetNormalColor(1, 0, { 150,0,0 });
	closeIcon.SetHighlightedColor(1, 0, { 255, 0, 0 });
	closeIcon.SetPos({ Graphics::ScreenWidth - closeIcon.GetPixelWidth() - 3, 3 });
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	//Mouse control
	mouseControl.Update();
	float dt = ft.Mark();
	graphIconbar.Update(mouseControl);
	settingsIconbar.Update(mouseControl);
	closeIcon.Update(mouseControl);
	events.Update(dt, mouseControl);
	for (auto& file : files)
	{
		file->Calculate(dt);
		file->Update(mouseControl);
	}

	//Keyboard Input
	while (!wnd.kbd.KeyIsEmpty())
	{
		const Keyboard::Event key = wnd.kbd.ReadKey();
		if (key.IsPress())
		{
			const unsigned char code = key.GetCode();
			bool play = false;
			switch (code)
			{
			case VK_ESCAPE:
				//close program
				wnd.Kill();
				break;
			case VK_SPACE:
				//pause/play all
				for (auto& file : files)
				{
					play = file->GetCalculating() || play;
				}
				settingsIconbar.Activate(play? 6:5);
				break;
			case 'S':
				//save all
				settingsIconbar.Activate(2);
				break;
			case 'R':
				//refresh all
				settingsIconbar.Activate(3);
				break;
			case 0x43:
				//close all if code == 'c'
				for (auto& file : files)
				{
					file->CloseAll();
				}
				break;
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37:
			case 0x38:
			case 0x39:
				//toggle the files on/off
				if ((int)code - 49 < files.size())
				{
					if (files.at((int)code - 49)->AnyVisible())
					{
						files.at((int)code - 49)->CloseAll();
					}
					else
					{
						files.at((int)code - 49)->ToggleVisible();
					}
				}
				break;
			case VK_ADD:
				//speed all open files up
				for (auto& file : files)
				{
					if (file->AnyVisible())
					{
						file->SpeedUp();
					}
				}
				break;
			case VK_SUBTRACT:
				//slow all open files down
				for (auto& file : files)
				{
					if (file->AnyVisible())
					{
						file->SpeedDown();
					}
				}
				break;
			}
		}
	}
}

void Game::ComposeFrame()
{
	graphIconbar.Draw(gfx, font);
	settingsIconbar.Draw(gfx, font);
	closeIcon.Draw(gfx, font);
	for (const auto& file : files)
	{
		file->Draw(gfx);
	}
	events.Draw(gfx);
}
