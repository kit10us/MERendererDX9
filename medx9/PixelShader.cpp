// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/PixelShader.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>

using namespace medx9;
using namespace me;
using namespace render;
using namespace shader;

PixelShader::PixelShader( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
	, m_assembly( false )
	, m_created( false )
	, m_isTrans( false )
{
}

PixelShader::PixelShader( IRenderer * renderer, PixelShaderParameters parameters )
	: PixelShader( renderer )
{
	Create( parameters );
}

PixelShader::~PixelShader()
{
	Destroy();
}

void PixelShader::Destroy()
{
	m_shader = nullptr;
	m_codeBuffer = nullptr;
	m_constantTable = nullptr;
}

void PixelShader::Create( PixelShaderParameters parameters )
{
	Destroy();

	m_parameters = parameters;

	bool debug =
#if defined( DEBUG ) || defined( _DEBUG )
		true;
#else
		false;
#endif

	HRESULT result = S_OK;
	CComPtr< ID3DXBuffer > errorBuffer;
	if ( ! m_parameters.code.empty() )
	{
	}
	else if( ! m_parameters.path.Empty() )
	{
		if( m_assembly )
		{
			DWORD flags = D3DXSHADER_DEBUG;
			result = D3DXAssembleShaderFromFileA( m_parameters.path.ToString().c_str(), 0, 0, D3DXSHADER_DEBUG, &m_codeBuffer, &errorBuffer );
		}
		else
		{
			DWORD flags = 0;
			result = D3DXCompileShaderFromFileA( m_parameters.path.ToString().c_str(), 0, 0, m_parameters.entryPointName.c_str(), m_parameters.profile.c_str(), flags, &m_codeBuffer, &errorBuffer, &m_constantTable );
		}
		if( m_codeBuffer == 0 || FAILED( result ) )
		{
			m_errorMessage = static_cast< char * >(errorBuffer->GetBufferPointer());
			throw unify::Exception( "Failed to create shader \"" + m_parameters.path.ToString() + "\": " + m_errorMessage + "\n" );
		}
	}
	else if( m_parameters.code.empty() )
	{
		if( m_assembly )
		{
			DWORD flags = 0;
			D3DXAssembleShader( m_parameters.code.c_str(), static_cast< unsigned int >(m_parameters.code.length() - 1), 0, NULL, flags, &m_codeBuffer, &errorBuffer );
		}
		else
		{
			DWORD flags = 0;
			D3DXCompileShader( m_parameters.code.c_str(), static_cast< unsigned int >(m_parameters.code.length() - 1), 0, 0, m_parameters.entryPointName.c_str(), m_parameters.profile.c_str(), flags, &m_codeBuffer, &errorBuffer, &m_constantTable );
		}

		if( m_codeBuffer == 0 || FAILED( result ) )
		{
			m_errorMessage = static_cast< char * >(errorBuffer->GetBufferPointer());
			throw unify::Exception( "Failed to create shader from code: " + m_errorMessage + "\n" );
		}
	}
	else
	{
		throw unify::Exception( "Attempted to create shader from unknown source!" );
	}

	/*
	// Find entry handles...
	m_finalMatrix = m_constantTable->GetConstantByName( 0, "finalMatrix" );
	m_colorMultiply = m_constantTable->GetConstantByName( 0, "colorMultiply" );
	m_colorAddition = m_constantTable->GetConstantByName( 0, "colorAddition" );
	m_scalarTime = m_constantTable->GetConstantByName( 0, "scalarTime" );
	m_textureSize = m_constantTable->GetConstantByName( 0, "textureSize" );
	*/
	m_renderer->GetDxDevice()->CreatePixelShader( (unsigned long *)m_codeBuffer->GetBufferPointer(), &m_shader );
}

const ConstantBuffer * PixelShader::GetConstants() const
{
	return m_constants.get();
}

void PixelShader::LockConstants( size_t buffer, unify::DataLock & lock )
{
}

void PixelShader::UnlockConstants( size_t buffer, unify::DataLock & lock )
{
	// Do nothing, we just needed m_lockData updated.
}

void PixelShader::Use()
{
	HRESULT result = m_renderer->GetDxDevice()->SetPixelShader( m_shader );
	if( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set pixel shader!" );
	}
}

std::string PixelShader::GetSource() const
{
	return m_parameters.path.ToXPath();
}

bool PixelShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

bool PixelShader::IsTrans() const
{
	return m_parameters.trans;
}
