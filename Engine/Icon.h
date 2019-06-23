#pragma once
#include <string>
#include <functional>
#include "Resource.h"
#include "Rect.h"
#include "Vec2.h"
#include "SpriteEffect.h"
#include "Colors.h"
#include "Graphics.h"
#include "Surface.h"
#include "Font.h"

class Icon
{
public:
	enum Type
	{
		Save,
		Refresh,
		Forward,
		Backward,
		Play,
		Pause,
		Close,
		Graph,
		Reload
	};
	enum Alignment
	{
		left,
		right,
		centered
	};
	Icon(Surface s, int x = 0, int y = 0, std::string description = "")
		:
		surf(s),
		pos(x,y),
		description(description)
	{}
	Icon(Surface s, Vei2 v = { 0,0 }, std::string description = "")
		:
		surf(s),
		pos(v),
		description(description)
	{}
	void Draw(Graphics& gfx, const Font& font) const
	{
		if (visible)
		{
			if (isHighlighted)
			{
				gfx.DrawSprite(pos.x, pos.y, surf, SpriteEffect::Substitution(chroma, highlightedColor));
				Vei2 textPos;
				if (alignment == Alignment::left)
				{
					textPos = { pos.x, pos.y + GetRect().GetHeight() + 3 };
				}
				else if (alignment == Alignment::right)
				{
					textPos = { pos.x + GetRect().GetWidth() - font.GetWidth() * (int)description.size(), pos.y + GetRect().GetHeight() + 3 };
				}
				else ///centered
				{
					textPos = { pos.x + (GetRect().GetWidth() - font.GetWidth() * (int)description.size()) / 2, pos.y + GetRect().GetHeight() + 3 };
				}
				font.DrawText(description, textPos, normalColor, gfx);
			}
			else
			{
				gfx.DrawSprite(pos.x, pos.y, surf, SpriteEffect::Substitution(chroma, normalColor));
			}
		}
	}
	bool IsInside(const Vei2& v) const
	{
		return GetRect().Contains(v);
	}
	void SetHighlighted(bool b)
	{
		isHighlighted = b;
	}
	RectI GetRect() const
	{
		return surf.GetRect().GetTranslated(pos);
	}
	virtual Type GetType() const = 0;
	void SetPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
	}
	void SetPos(Vei2 p)
	{
		pos = p;
	}
	Vei2 GetPos() const
	{
		return pos;
	}
	void Translate(Vei2 dp)
	{
		pos += dp;
	}
	int GetWidth() const
	{
		return surf.GetWidth();
	}
	void SetVisible(bool b)
	{
		visible = b;
	}
	bool GetVisible() const
	{
		return visible;
	}
	void Action() const
	{
		action();
	}
	void BindAction(std::function<void()> newAction)
	{
		action = newAction;
	}
	void SetNormalColor(const Color c)
	{
		normalColor = c;
	}
	void SetHighlightedColor(const Color c)
	{
		highlightedColor = c;
	}
	void SetAlignment(Alignment a)
	{
		alignment = a;
	}
	Alignment GetAlignment() const
	{
		return alignment;
	}
	bool IsHighlighted() const
	{
		return isHighlighted;
	}
protected:
	std::function<void()> action;
	bool visible = true;
	static constexpr Color chroma = Colors::White;
	Color highlightedColor = Colors::White;
	Color normalColor = Colors::Gray;
	bool isHighlighted = false;
	Alignment alignment = Alignment::left;
	Vei2 pos;
	std::string description;
	Surface surf;
};

class CloseIcon : public Icon
{
public:
	CloseIcon(std::string description = "Close")
		:
		Icon(Surface(IDB_BITMAP4), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Close;
	}
};
class GraphIcon : public Icon
{
public:
	GraphIcon(std::string description = "")
		:
		Icon(Surface(IDB_BITMAP5), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Graph;
	}
};
class SaveIcon : public Icon
{
public:
	SaveIcon(std::string description = "Save")
		:
		Icon(Surface(IDB_BITMAP9), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Save;
	}
};

class RefreshIcon : public Icon
{
public:
	RefreshIcon(std::string description = "Refresh")
		:
		Icon(Surface(IDB_BITMAP8), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Refresh;
	}
};

class PauseIcon : public Icon
{
public:
	PauseIcon(std::string description = "Pause")
		:
		Icon(Surface(IDB_BITMAP6), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Pause;
	}
};

class PlayIcon : public Icon
{
public:
	PlayIcon(std::string description = "Play")
		:
		Icon(Surface(IDB_BITMAP7), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Play;
	}
};

class ForwardIcon : public Icon
{
public:
	ForwardIcon(std::string description = "Faster")
		:
		Icon(Surface(IDB_BITMAP2), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Forward;
	}
};

class BackwardIcon : public Icon
{
public:
	BackwardIcon(std::string description = "Slower")
		:
		Icon(Surface(IDB_BITMAP3), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Backward;
	}
};

class ReloadIcon : public Icon
{
public:
	ReloadIcon(std::string description = "Reload")
		:
		Icon(Surface(IDB_BITMAP10), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Reload;
	}
};
