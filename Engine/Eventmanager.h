#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include "Colors.h"
#include "Font.h"
#include "Vec2.h"
#include "Rect.h"

class Eventmanager
{
public:
	class Event
	{
	public:
		Event(std::string text = "Event", Vec2 pos = { 0.0f,0.0f })
			:
			text(text),
			pos(pos),
			font("Bitmaps\\Font.bmp")
		{}
		std::string GetInfo() const
		{
			return text;
		}
		int GetWidth() const
		{
			return GetTextWidth() + 2 * offset;
		}
		int GetHeight() const
		{
			return GetTextHeight() + 2 * offset;
		}
		void Draw(Graphics& gfx) const
		{
			RectI rect = RectI(Vei2(pos), GetWidth(), GetHeight());
			gfx.DrawRectArea(rect, rectColor.GetFaded((delay - timePassed) / delay));
			font.DrawText(text, Vei2(pos) + Vei2(offset, offset), textColor.GetFaded((delay - timePassed) / delay), gfx);
		}
		void Fade(float dt)
		{
			timePassed += dt;
			timePassed = std::min(timePassed, delay);
		}
		bool IsFadedOut() const
		{
			return timePassed == delay;
		}
		Vec2 GetPos() const
		{
			return pos;
		}
		void TranslatePos(Vec2 t)
		{
			pos += t;
		}
	private:
		int GetTextWidth() const
		{
			std::vector<int> max;
			max.emplace_back(0);
			for (const char& c : text)
			{
				if (c == '\n')
				{
					max.emplace_back(0);
				}
				else
				{
					max.back() += 1;
				}
			}
			int Max = 0;
			for (const auto& i : max)
			{
				Max = std::max(Max, i);
			}
			return Max * font.GetWidth();
		}
		int GetTextHeight() const
		{
			int ret = 1;
			for (const char& c : text)
			{
				if (c == '\n')
				{
					ret++;
				}
			}
			return font.GetHeight() * ret;
		}
	private:
		std::string text;
		static constexpr int offset = 7;
		static constexpr Color rectColor = Colors::Blue;
		static constexpr Color textColor = Colors::White;
		static constexpr float delay = 1.5f;
		float timePassed = 0.0f;
		Font font;
		Vec2 pos;
	};
public:
	void Update(float dt)
	{
		if (!events.empty() && !(events.at(0).GetPos().y > (float)minY))
		{
			events.at(0).Fade(dt);
		}
		for (int i = 0; i < events.size(); i++)
		{
			if (events.at(i).IsFadedOut())
			{
				events.erase(events.begin() + i);
				i--;
			}
		}
		if (!events.empty() && (events.at(0).GetPos().y > (float)minY))
		{
			for (auto& e : events)
			{
				e.TranslatePos({ 0.0f, -vel * dt});
			}
		}
	}
	void Draw(Graphics& gfx) const
	{
		for (const auto& e : events)
		{
			e.Draw(gfx);
		}
	}
	void Add(std::string text)
	{
		if (events.empty())
		{
			events.emplace_back(Event(text, { (float)offset, (float)minY }));
		}
		else
		{
			Vec2 nextPos = events.back().GetPos();
			nextPos.y += (float)events.back().GetHeight();
			nextPos.y += offset;
			events.emplace_back(Event(text, nextPos));
		}
	}
private:
	std::vector<Event> events;
	static constexpr int offset = 3;
	static constexpr int minY = 20 + 2 * offset;
	static constexpr float vel = 50.0f;
};
