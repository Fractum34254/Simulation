#pragma once
#include "Mouse.h"
#include "Vec2.h"

class MouseController
{
public:
	MouseController(Mouse& m)
		:
		mouse(m),
		pos(m.GetPos())
	{}
	Vei2 GetMousePos() const
	{
		return Vei2(mouse.GetPos());
	}
	void Update()
	{
		const Vei2 now = Vei2(mouse.GetPos());
		diff = now - pos;
		pos = now;
	}
	Vei2 diff = { 0,0 };
	Vei2 pos;
	Mouse& mouse;
};
