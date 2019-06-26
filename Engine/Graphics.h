/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include <cassert>
#include "Rect.h"
#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"

class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	Color GetPixel(int x, int y) {
		return pSysBuffer[Graphics::ScreenWidth * y + x];
	}
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
	void DrawLine(Vec2 p0, Vec2 p1, Color c);
	void DrawLine(Ved2 p0, Ved2 p1, Color c)
	{
		DrawLine((Vec2)p0, (Vec2)p1, c);
	}
	void DrawLine(float x0, float y0, float x1, float y1, Color c)
	{
		DrawLine(Vec2(x0, y0), Vec2(x1, y1), c);
	}
	void DrawLine(int x0, int y0, int x1, int y1, Color c)
	{
		DrawLine((float)x0, (float)y0, (float)x1, (float)y1, c);
	}
	void DrawLine(double x0, double y0, double x1, double y1, Color c)
	{
		DrawLine((float)x0, (float)y0, (float)x1, (float)y1, c);
	}
	void DrawCircleFilled(Vec2 pos, float radius, Color c)
	{
		RectF rect = RectF::FromCenter(pos, radius, radius);
		for (float x = rect.left; x <= rect.right; x++)
		{
			for (float y = rect.top; y <= rect.bottom; y++)
			{
				if ((Vec2(x, y) - pos).GetLengthSq() < radius * radius)
				{
					PutPixel((int)x, (int)y, c);
				}
			}
		}
	}
	void DrawRectLine(RectI rect, int thickness, int borderExtension, Color c)
	{
		for (int i = 0; i < thickness; i++)
		{
			//top
			DrawLine(rect.left - borderExtension, rect.top - borderExtension + i, rect.right + borderExtension, rect.top - borderExtension + i, c);
			//bottom
			DrawLine(rect.left - borderExtension, rect.bottom + borderExtension - i, rect.right + borderExtension, rect.bottom + borderExtension - i, c);
			//left
			DrawLine(rect.left - borderExtension + i, rect.top - borderExtension, rect.left - borderExtension + i, rect.bottom + borderExtension, c);
			//right
			DrawLine(rect.right + borderExtension - i, rect.top - borderExtension, rect.right + borderExtension - i, rect.bottom + borderExtension, c);
		}
	}
	void DrawRectArea(RectI rect, Color c)
	{
		for (int x = rect.left; x <= rect.right; x++)
		{
			for (int y = rect.top; y <= rect.bottom; y++)
			{
				PutPixel(x, y, c);
			}
		}
	}
	void DrawRectAreaRound(RectI rect, Color c, int cornerSize)
	{
		RectI topRect = RectI(rect.left + cornerSize, rect.right - cornerSize, rect.top, rect.top + cornerSize);
		RectI midRect = RectI(rect.left, rect.right, rect.top + cornerSize, rect.bottom - cornerSize);
		RectI bottomRect = RectI(rect.left + cornerSize, rect.right - cornerSize, rect.bottom - cornerSize, rect.bottom);
		DrawRectArea(topRect, c);
		DrawRectArea(midRect, c);
		DrawRectArea(bottomRect, c);
		DrawCircleFilled({ (float)(rect.left + cornerSize), (float)(rect.top + cornerSize) }, (float) cornerSize, c);
		DrawCircleFilled({ (float)(rect.left + cornerSize), (float)(rect.bottom - cornerSize) }, (float)cornerSize, c);
		DrawCircleFilled({ (float)(rect.right - cornerSize), (float)(rect.top + cornerSize) }, (float)cornerSize, c);
		DrawCircleFilled({ (float)(rect.right - cornerSize), (float)(rect.bottom - cornerSize) }, (float)cornerSize, c);
	}
	template<typename E> void DrawSprite(int x, int y, const Surface & s, E effect)
	{
		DrawSprite(x, y, s.GetRect(), s, effect);
	}
	template<typename E> void DrawSprite(int x, int y, const RectI & srcRect, const Surface & s, E effect)
	{
		DrawSprite(x, y, srcRect, GetScreenRect(), s, effect);
	}
	template<typename E> void DrawSprite(int x, int y, RectI srcRect, const RectI & clipRect, const Surface & s, E effect)
	{
		assert(srcRect.left >= 0);
		assert(srcRect.right <= s.GetWidth());
		assert(srcRect.top >= 0);
		assert(srcRect.bottom <= s.GetHeight());
		if (x < clipRect.left) {
			srcRect.left += clipRect.left - x;
			x = clipRect.left;
		}
		if (y < clipRect.top) {
			srcRect.top += clipRect.top - y;
			y = clipRect.top;
		}
		if (x + srcRect.GetWidth() > clipRect.right) {
			srcRect.right -= x + srcRect.GetWidth() - clipRect.right;
		}
		if (y + srcRect.GetHeight() > clipRect.bottom) {
			srcRect.bottom -= y + srcRect.GetHeight() - clipRect.bottom;
		}
		for (int sy = srcRect.top; sy < srcRect.bottom; sy++) {
			for (int sx = srcRect.left; sx < srcRect.right; sx++) {
				effect(
					s.GetSurfacePixel(sx, sy),
					x + sx - srcRect.left,
					y + sy - srcRect.top,
					*this
				);
			}
		}
	}
	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 1200;
	static constexpr int ScreenHeight = 800;
	static RectI GetScreenRect()  {
		return Rect_<int>(0, ScreenWidth, 0, ScreenHeight);
	}
};