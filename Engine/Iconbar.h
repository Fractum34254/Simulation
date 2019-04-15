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
		std::vector<std::unique_ptr<Icon>> next;
		next.emplace_back(std::move(i));
		icons.emplace_back(std::make_pair(std::move(next), 0));
	}
	void AddIcon(std::unique_ptr<Icon> i, std::function<void()> f, int place)
	{
		if (place >= icons.size())
		{
			while (place > icons.size())
			{
				AddVoid();
			}
			AddIcon(std::move(i), f);
		}
		else
		{
			i->SetPos(pos.x + place * (iconWidth + offset), pos.y);
			i->BindAction(f);
			icons.at(place).first.emplace_back(std::move(i));
		}
	}
	void AddVoid()
	{
		std::vector<std::unique_ptr<Icon>> next;
		icons.emplace_back(std::make_pair(std::move(next),0));
	}
	void Draw(Graphics& gfx) const
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons.at(i).first.size() != 0)
			{
				icons.at(i).first.at(icons.at(i).second)->Draw(gfx);
			}
		}
	}
	void Update(const MouseController& mouseControl)
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons.at(i).first.size() != 0)
			{
				if (icons.at(i).first.at(icons.at(i).second)->IsInside(mouseControl.GetMousePos()))
				{
					icons.at(i).first.at(icons.at(i).second)->SetHighlighted(true);
					while (!mouseControl.mouse.IsEmpty())
					{
						const auto e = mouseControl.mouse.Read();
						if (e.GetType() == Mouse::Event::Type::LPress)
						{
							icons.at(i).first.at(icons.at(i).second)->Action();
							if (icons.at(i).second + 1 == icons.at(i).first.size())
							{
								icons.at(i).second = 0;
							}
							else
							{
								icons.at(i).second++;
							}
						}
					}
				}
				else
				{
					icons.at(i).first.at(icons.at(i).second)->SetHighlighted(false);
				}
			}
		}
	}
	void SetActive(int place, int current)
	{
		icons.at(place).second = current % (int)icons.at(place).first.size();
	}
	int GetPixelWidth() const
	{
		return (iconWidth + offset) * GetWidth() - offset;
	}
	int GetWidth() const
	{
		return (int)icons.size();
	}
	void SetPos(Vei2 newPos)
	{
		const Vei2 diff = newPos - pos;
		Translate(diff);
	}
	Vei2 GetPos() const
	{
		return pos;
	}
	void Translate(Vei2 diff)
	{
		pos += diff;
		for (int i = 0; i < icons.size(); i++)
		{
			for (int k = 0; k < icons.at(i).first.size(); k++)
			{
				if (icons.at(i).first.at(k) != nullptr)
				{
					icons.at(i).first.at(k)->Translate(diff);
				}
			}
		}
	}
	void SetTextAlignment(int row, int col, const Icon::Alignment a)
	{
		if (row < icons.size())
		{
			if (col < icons.at(row).first.size())
			{
				icons.at(row).first.at(col)->SetAlignment(a);
			}
		}
	}
	void SetNormalColor(int row, int col, const Color c)
	{
		if (row < icons.size())
		{
			if (col < icons.at(row).first.size())
			{
				icons.at(row).first.at(col)->SetNormalColor(c);
			}
		}
	}
	void SetHighlightedColor(int row, int col, const Color c)
	{
		if (row < icons.size())
		{
			if (col < icons.at(row).first.size())
			{
				icons.at(row).first.at(col)->SetHighlightedColor(c);
			}
		}
	}
	void SetAllTextAlignments(const Icon::Alignment a)
	{
		for (int i = 0; i < icons.size(); i++)
		{
			for (int j = 0; j < icons.at(i).first.size(); j++)
			{
				icons.at(i).first.at(j)->SetAlignment(a);
			}
		}
	}
private:
	Vei2 pos = { 0,0 };
	static constexpr int iconHeight = 20;
	static constexpr int iconWidth = 20;
	static constexpr int offset = 3;
	std::vector<std::pair<std::vector<std::unique_ptr<Icon>>, int>> icons;
};