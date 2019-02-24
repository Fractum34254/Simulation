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
		i->SetPos(pos.x + (int)icons.size() * (iconWidth + offset), pos.y);
		i->BindAction(f);
		icons.emplace_back(std::move(i));
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
	void Update(const MouseController& mouseControl)
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons.at(i) != nullptr)
			{
				if (icons.at(i)->IsInside(mouseControl.GetMousePos()))
				{
					icons.at(i)->SetHighlighted(true);
					while (!mouseControl.mouse.IsEmpty())
					{
						const auto e = mouseControl.mouse.Read();
						if (e.GetType() == Mouse::Event::Type::LPress)
						{
							icons.at(i)->Action();
						}
					}
				}
				else
				{
					icons.at(i)->SetHighlighted(false);
				}
			}
		}
	}
	void SetPos(Vei2 newPos)
	{
		pos = newPos;
	}
	Vei2 GetPos() const
	{
		return pos;
	}
private:
	Vei2 pos = { 0,0 };
	static constexpr int iconHeight = 20;
	static constexpr int iconWidth = 20;
	static constexpr int offset = 3;
	std::vector<std::unique_ptr<Icon>> icons;
};