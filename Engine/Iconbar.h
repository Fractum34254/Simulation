#pragma once
#include <vector>
#include <functional>
#include "MouseController.h"
#include "Graphics.h"
#include "Vec2.h"
#include "Icon.h"

class Iconbar
{
public:
	void AddIcon(std::unique_ptr<Icon> i, std::function<void()> f)
	{
		i->SetPos(pos.x + icons.size() * (iconWidth + offset), pos.y);
		i->BindAction(f);
		icons.emplace_back(i);
	}
	void AddVoid()
	{
		icons.emplace_back(nullptr);
	}
	void Draw(Graphics& gfx) const
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons.at(i) != nullptr)
			{
				icons.at(i)->Draw(gfx);
			}
		}
	}
	void Update(const MouseController& mouse)
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons.at(i) != nullptr)
			{
				if (icons.at(i)->IsInside(mouse.GetMousePos()))
				{
					icons.at(i)->SetHighlighted(true);
					if (mouse.mouse.LeftIsPressed())
					{
						icons.at(i)->Action();
					}
				}
				else
				{
					icons.at(i)->SetHighlighted(false);
				}
			}
		}
	}
private:
	Vei2 pos = { 0,0 };
	static constexpr int iconHeight = 20;
	static constexpr int iconWidth = 20;
	static constexpr int offset = 3;
	std::vector<std::unique_ptr<Icon>> icons;
};