// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <medx9/DirectX.h>
#include <mewos/WindowsOS.h>
#include <atlbase.h>
#include <memory>

namespace medx9
{
	class Renderer : public me::render::IRenderer
	{
	public:
		Renderer( me::os::IOS * os, me::render::Display display, size_t index );
		virtual ~Renderer();					

		const me::render::Display & GetDisplay() const;

		IDirect3DDevice9 * GetDxDevice() const;

		void BeforeRender() override;
		void BeforeRenderSolids() override;
		void BeforeRenderTrans() override;
		void AfterRender() override;

		me::render::Viewport GetViewport() const override;

		bool IsFullscreen() const override;

		size_t GetIndex() const override;

		void Render(const me::render::RenderInfo& renderInfo, const me::render::RenderMethod& method, me::render::Effect::ptr effect, me::render::IConstantBuffer* vertexCB, me::render::IConstantBuffer* pixelCB, me::render::MatrixFeed& matrixFeed) override;
		void Render(const me::render::RenderInfo& renderInfo, const me::render::RenderMethod& method, me::render::BufferSet* bufferSet, me::render::MatrixFeed& matrixFeed) override;


		me::render::IVertexBuffer::ptr ProduceVB( me::render::VertexBufferParameters parameters ) override;
		me::render::IIndexBuffer::ptr ProduceIB( me::render::IndexBufferParameters parameters ) override;
		me::render::IVertexShader::ptr ProduceVS( me::render::VertexShaderParameters parameters ) override;
		me::render::IPixelShader::ptr ProducePS( me::render::PixelShaderParameters parameters ) override;
		me::render::IVertexConstruct::ptr ProduceVC( const me::render::VertexDeclaration & vd, const me::render::IVertexShader & vs ) override;
		me::render::ITexture::ptr ProduceT( me::render::TextureParameters parameters ) override;

		void UseTextures( std::vector< me::render::ITexture::ptr > textures ) override;

	private:
		mewos::WindowsOS * m_os;
		me::render::Display m_display;
		D3DPRESENT_PARAMETERS m_pp;
		CComPtr< IDirect3DDevice9 > m_dxDevice;
		CComPtr< IDirect3DSwapChain9 > m_swapChain;
		size_t m_index;
	};
}