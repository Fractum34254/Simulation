#pragma once
#include <string>
#include <functional>
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
		Graph
	};
	Icon(Surface s, int x = 0, int y = 0, std::string description = "")
		:
		surf(s),
		pos(x,y),
		description(description),
		font("Bitmaps\\Font.bmp")
	{}
	Icon(Surface s, Vei2 v = { 0,0 }, std::string description = "")
		:
		surf(s),
		pos(v),
		description(description),
		font("Bitmaps\\Font.bmp")
	{}
	void Draw(Graphics& gfx) const
	{
		if (visible)
		{
			if (isHighlighted)
			{
				gfx.DrawSprite(pos.x, pos.y, surf, SpriteEffect::Substitution(chroma, highlightedColor));
				font.DrawText(description, { pos.x, pos.y + GetRect().GetHeight() + 3 }, normalColor, gfx);
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
protected:
	std::function<void()> action;
	bool visible = true;
	static constexpr Color chroma = Colors::White;
	static constexpr Color highlightedColor = Colors::White;
	static constexpr Color normalColor = Colors::Gray;
	bool isHighlighted = false;
	Vei2 pos;
	std::string description;
	Font font;
	Surface surf;
};

class CloseIcon : public Icon
{
public:
	CloseIcon(std::string description = "Close")
		:
		Icon(Surface("Bitmaps\\close-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\graph-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\save-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\refresh-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\pause-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\play-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\forward-icon.bmp"), 0, 0, description)
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
		Icon(Surface("Bitmaps\\backward-icon.bmp"), 0, 0, description)
	{}
	Type GetType() const override
	{
		return Type::Backward;
	}
};
