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
	mouseControl(wnd.mouse)
{
	std::ifstream settings(settingsFileName);
	if (!settings)
	{
		std::string info = "Can't open file \"";
		info += settingsFileName;
		info += "\": File not found";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
	}
	if (settings.eof())
	{
		std::string info = "Empty file \"";
		info += settingsFileName;
		info += "\"";
		throw Exception(_CRT_WIDE(__FILE__), __LINE__, towstring(info));
	}

	std::vector<std::string> names;
	while (!settings.eof())
	{
		std::string temp;
		settings >> temp;
		names.emplace_back(temp);
	}
	const int right = gfx.GetScreenRect().right - 100;
	const int bottom = gfx.GetScreenRect().bottom - 100;
	const RectI fileRect = {left, right, top, bottom};
	for (int i = 0; i < names.size(); i++)
	{
		files.emplace_back(std::make_unique<File>(names.at(i), (float)offset, fileRect));
	}

	iconbar.SetPos({ 3,3 });
	for (auto& file : files)
	{
		iconbar.AddIcon(std::make_unique<GraphIcon>(file->GetName()), [&file]() {
			file->ToggleVisible();
		});
	}
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
	mouseControl.Update();
	float dt = ft.Mark();
	iconbar.Update(mouseControl);
	for (auto& file : files)
	{
		file->Calculate(dt);
		file->Update(mouseControl);
	}
}

void Game::ComposeFrame()
{
	iconbar.Draw(gfx);
	for (const auto& file : files)
	{
		file->Draw(gfx);
	}
}
