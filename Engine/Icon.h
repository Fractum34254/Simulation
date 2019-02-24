#pragma once
#include "Rect.h"
#include "Vec2.h"
#include "SpriteEffect.h"
#include "Colors.h"
#include "Graphics.h"
#include "Surface.h"

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
		Graph
	};
	Icon(Surface s, int x = 0, int y = 0)
		:
		surf(s),
		pos(x,y)
	{}
	Icon(Surface s, Vei2 v = { 0,0 })
		:
		surf(s),
		pos(v)
	{}
	void Draw(Graphics& gfx) const
	{
		if (visible)
		{
			if (isHighlighted)
			{
				gfx.DrawSprite(pos.x, pos.y, surf, SpriteEffect::Substitution(chroma, highlightedColor));
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
protected:
	bool visible = true;
	static constexpr Color chroma = Colors::White;
	static constexpr Color highlightedColor = Colors::White;
	static constexpr Color normalColor = Colors::Gray;
	bool isHighlighted = false;
	Vei2 pos;
	Surface surf;
};

class GraphIcon : public Icon
{
	GraphIcon()
		:
		Icon(Surface("Bitmaps\\graph-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Graph;
	}
};
class SaveIcon : public Icon
{
public:
	SaveIcon()
		:
		Icon(Surface("Bitmaps\\save-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Save;
	}
};

class RefreshIcon : public Icon
{
public:
	RefreshIcon()
		:
		Icon(Surface("Bitmaps\\refresh-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Refresh;
	}
};

class PauseIcon : public Icon
{
public:
	PauseIcon()
		:
		Icon(Surface("Bitmaps\\pause-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Pause;
	}
};

class PlayIcon : public Icon
{
public:
	PlayIcon()
		:
		Icon(Surface("Bitmaps\\play-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Play;
	}
};

class ForwardIcon : public Icon
{
public:
	ForwardIcon()
		:
		Icon(Surface("Bitmaps\\forward-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Forward;
	}
};

class BackwardIcon : public Icon
{
public:
	BackwardIcon()
		:
		Icon(Surface("Bitmaps\\backward-icon.bmp"), 0, 0)
	{}
	Type GetType() const override
	{
		return Type::Backward;
	}
};
