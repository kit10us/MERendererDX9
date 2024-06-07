// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx9/Renderer.h>
#include <me/render/IConstantBuffer.h>
#include <unify/Path.h>
#include <memory>

namespace medx9
{
	class PixelShader : public me::render::IPixelShader
	{
	public:
		PixelShader( me::render::IRenderer * renderer );
		PixelShader( me::render::IRenderer * renderer, me::render::PixelShaderParameters parameters );

		~PixelShader();

		void Destroy();

		void Create( me::render::PixelShaderParameters parameters );

		/*
		const me::shader::ConstantBuffer * GetConstants() const override;

		void LockConstants( size_t buffer, unify::DataLock & lock ) override;	   

		void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;
		*/
	
	public: // me::render::IPixelShader
		me::render::BlendDesc GetBlendDesc() const override;

	public: // me::render::IShader
		me::render::IConstantBuffer::ptr CreateConstantBuffer(me::render::BufferUsage::TYPE usage) const override;
		const void* GetBytecode() const override;
		size_t GetBytecodeLength() const override;
		void Use() override;
		bool IsTrans() const;

	public: // rm::IResource
		bool Reload() override;
		std::string GetSource() const override;

	protected:
		me::render::PixelShaderParameters m_parameters;
		bool m_assembly;
		std::string m_errorMessage;
		bool m_created;
		// SAS TODO: me::shader::ConstantBuffer::ptr m_constants;

		bool m_isTrans;	// Does this pixel shader turn the render into transparent (in part or entire)

		const Renderer * m_renderer;
		CComPtr< ID3DXBuffer > m_codeBuffer;
		CComPtr< ID3DXConstantTable > m_constantTable;
		CComPtr< IDirect3DPixelShader9 > m_shader;
	};
}