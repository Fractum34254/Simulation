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
		Pause
	};
	Icon(Surface s, int x, int y)
		:
		surf(s),
		pos(x,y)
	{}
	Icon(Surface s, Vei2 v)
		:
		surf(s),
		pos(v)
	{}
	void Draw(Graphics& gfx) const
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
private:
	static constexpr Color chroma = Colors::White;
	static constexpr Color highlightedColor = Colors::White;
	static constexpr Color normalColor = Colors::Gray;
	bool isHighlighted = false;
	Vei2 pos;
	Surface surf;
};

class SaveIcon : public Icon
{
public:
	SaveIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	SaveIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Save;
	}
};

class RefreshIcon : public Icon
{
public:
	RefreshIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	RefreshIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Refresh;
	}
};

class PauseIcon : public Icon
{
public:
	PauseIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	PauseIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Pause;
	}
};

class PlayIcon : public Icon
{
public:
	PlayIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	PlayIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Play;
	}
};

class ForwardIcon : public Icon
{
public:
	ForwardIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	ForwardIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Forward;
	}
};

class BackwardIcon : public Icon
{
public:
	BackwardIcon(Surface s, int x, int y)
		:
		Icon(s, x, y)
	{}
	BackwardIcon(Surface s, Vei2 v)
		:
		Icon(s, v)
	{}
	Type GetType() const override
	{
		return Type::Backward;
	}
};
