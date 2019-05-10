#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "PhilUtil.h"
#include "Mouse.h"
#include "Vec2.h"

class MouseController
{
public:
	class MousePointer
	{
	public:
		class Exception : public ChiliException
		{
		public:
			using ChiliException::ChiliException;
			virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
			virtual std::wstring GetExceptionType() const override { return L"Cursor Exception"; }
		};
	public:
		enum Pointer
		{
			normal,
			cross,
			hand,
			loading
		};
	public:
		MousePointer(Pointer p = normal)
			:
			cur(p)
		{
			ChangePointer(p);
		}
		void ChangePointer(const Pointer p)
		{
			try
			{
				switch (p)
				{
				case normal:
					SetCursor(LoadCursor(nullptr, IDC_ARROW));
					break;
				case cross:
					SetCursor(LoadCursor(nullptr, IDC_CROSS));
					break;
				case hand:
					SetCursor(LoadCursor(nullptr, IDC_HAND));
					break;
				case loading:
					SetCursor(LoadCursor(nullptr, IDC_IBEAM));
					break;
				}
			}
			catch (const std::exception& e)
			{
				const char* textCStr = e.what();
				const std::string textStr(textCStr);
				std::string info = "Failed to load cursor:\nCaught a std::exception:\n\n";
				info += textStr;
				info += "\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			catch (...)
			{
				std::string info = "Failed to load cursor:\nCaught a non-defined exception.\n";
				throw Exception(_CRT_WIDE(__FILE__), __LINE__, PhilUtil::towstring(info));
			}
			cur = p;
		}
		Pointer GetPointer() const
		{
			return cur;
		}
	private:
		Pointer cur;
	};
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
	void SetPointer(MousePointer::Pointer p)
	{
		cursor.ChangePointer(p);
	}
	MousePointer::Pointer GetPointer() const
	{
		return cursor.GetPointer();
	}
	Vei2 diff = { 0,0 };
	Vei2 pos;
	Mouse& mouse;
private:
	MousePointer cursor;
};
