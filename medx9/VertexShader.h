// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/RenderInfo.h>
#include <medx9/Renderer.h>
#include <me/render/IVertexShader.h>
#include <me/render/VertexDeclaration.h>
#include <unify/Path.h>
#include <atlbase.h>

namespace medx9
{
	class VertexShader : public me::render::IVertexShader
	{
	public:
		VertexShader( me::render::IRenderer * renderer );
		VertexShader( me::render::IRenderer * renderer, me::render::VertexShaderParameters parameters );

		~VertexShader();

		void Destroy();

		void Create( me::render::VertexShaderParameters parameters );

		const me::shader::ConstantBuffer * GetConstants() const override;

		void LockConstants( size_t buffer, unify::DataLock & lock ) override;	  
		void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;	  

		void SetVertexDeclaration( me::render::VertexDeclaration::ptr vertexDeclaration );
		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;

		const void * GetBytecode() const override;

		size_t GetBytecodeLength() const override;

		void Use() override;

		std::string GetSource() const override;

		bool Reload() override;

		bool IsTrans() const override;

	protected:	   
		me::render::VertexShaderParameters m_parameters;

		unify::Path m_filePath;
		std::string m_code;
		bool m_assembly;
		std::string m_entryPointName;
		std::string m_profile;
		std::string m_errorMessage;
		bool m_created;
		me::shader::ConstantBuffer::ptr m_constants;
		me::render::VertexDeclaration::ptr m_vertexDeclaration;

		medx9::Renderer * m_renderer;

		CComPtr< ID3DXBuffer > m_codeBuffer;
		CComPtr< ID3DXConstantTable > m_constantTable;
		CComPtr< IDirect3DVertexShader9 > m_shader;

		std::vector< unify::Matrix > m_lockData;
	
		D3DXHANDLE m_worldMatrixHandle;
		D3DXHANDLE m_viewMatrixHandle;
		D3DXHANDLE m_projectionMatrixHandle;
	};
}