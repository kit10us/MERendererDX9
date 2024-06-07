// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/RenderInfo.h>
#include <medx9/Renderer.h>
#include <me/render/IVertexShader.h>
#include <me/render/VertexDeclaration.h>
#include <me/render/IConstantBuffer.h>
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

		// SAS TODO: const me::shader::ConstantBuffer * GetConstants() const override;

		// SAS TODO: void LockConstants( size_t buffer, unify::DataLock & lock ) override;	  
		// SAS TODO: void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;	  

		void SetVertexDeclaration( me::render::VertexDeclaration::ptr vertexDeclaration );
		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;

	public: // me::render::IShader
		me::render::IConstantBuffer::ptr CreateConstantBuffer(me::render::BufferUsage::TYPE usage) const override;
		const void * GetBytecode() const override;
		size_t GetBytecodeLength() const override;
		void Use() override;
		bool IsTrans() const override;

	public: // rm::IResource
		bool Reload() override;
		std::string GetSource() const override;

	protected:	   
		me::render::VertexShaderParameters m_parameters;

		unify::Path m_filePath;
		std::string m_code;
		bool m_assembly;
		std::string m_entryPointName;
		std::string m_profile;
		std::string m_errorMessage;
		bool m_created;
		//me::shader::ConstantBuffer::ptr m_constants;
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