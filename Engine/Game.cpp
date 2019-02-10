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
	file("file.txt", 10, RectI(100, 700, 100, 500)),
	mouseControl(wnd.mouse)
{
	buttons.emplace_back(std::make_unique<SaveIcon>(SaveIcon(Surface("Bitmaps\\save-icon.bmp"), 5,5)));
	buttons.emplace_back(std::make_unique<RefreshIcon>(RefreshIcon(Surface("Bitmaps\\refresh-icon.bmp"), 25, 25)));
	buttons.emplace_back(std::make_unique<PlayIcon>(PlayIcon(Surface("Bitmaps\\play-icon.bmp"), 45, 45)));
	buttons.emplace_back(std::make_unique<PauseIcon>(PauseIcon(Surface("Bitmaps\\pause-icon.bmp"), 65, 65)));
	buttons.emplace_back(std::make_unique<ForwardIcon>(ForwardIcon(Surface("Bitmaps\\forward-icon.bmp"), 85, 85)));
	buttons.emplace_back(std::make_unique<BackwardIcon>(BackwardIcon(Surface("Bitmaps\\backward-icon.bmp"), 105, 105)));
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
	float dt = ft.Mark();
	file.Calculate(dt);
	file.Update(mouseControl);
	for (auto& icon : buttons)
	{
		if (icon->IsInside(wnd.mouse.GetPos()))
		{
			icon->SetHighlighted(true);
			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				if (e.GetType() == Mouse::Event::Type::LPress)
				{
					icon->Action(file);
				}
			}
		}
		else
		{
			icon->SetHighlighted(false);
		}
	}
}

void Game::ComposeFrame()
{
	file.Draw(gfx);
	for (auto& icon : buttons)
	{
		icon->Draw(gfx);
	}
}
