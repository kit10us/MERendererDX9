// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/VertexShader.h>
#include <me/exception/NotImplemented.h>

using namespace medx9;
using namespace me;
using namespace render;
using namespace shader;

VertexShader::VertexShader( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
	, m_worldMatrixHandle( 0 )
	, m_viewMatrixHandle( 0 )
	, m_projectionMatrixHandle( 0 )
	, m_assembly( false )
	, m_created( false )
{
}

VertexShader::VertexShader( IRenderer * renderer, VertexShaderParameters parameters )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
	, m_worldMatrixHandle( 0 )
	, m_viewMatrixHandle( 0 )
	, m_projectionMatrixHandle( 0 )
	, m_assembly( false )
	, m_created( false )
{
	Create( parameters );
}

VertexShader::~VertexShader()
{
	Destroy();
}

void VertexShader::Destroy()
{
	m_codeBuffer = nullptr;
	m_constantTable = nullptr;
	m_shader = nullptr;
}

void VertexShader::Create( VertexShaderParameters parameters )
{
	Destroy();

	// NOTE: For now, we are going to pull this programatiicaly from the shader: m_constants = parameters.constants;
	m_vertexDeclaration = parameters.vertexDeclaration;

	m_parameters = parameters;

	if( parameters.path.Empty() == false )
	{
		m_filePath = parameters.path;
	}
	else
	{
		m_code = parameters.code;
	}

	m_entryPointName = parameters.entryPointName;
	m_profile = parameters.profile;

	bool debug = false;
#if defined( DEBUG ) || defined( _DEBUG )
	debug = true;
#endif

	HRESULT result = S_OK;
	ID3DXBuffer * errorBuffer = 0;
	if( !m_filePath.Empty() )
	{
		if( m_assembly )
		{
			DWORD flags = D3DXSHADER_DEBUG;
			result = D3DXAssembleShaderFromFileA( m_filePath.ToString().c_str(), 0, 0, D3DXSHADER_DEBUG, &m_codeBuffer, &errorBuffer );
		}
		else
		{
			DWORD flags = 0;
			result = D3DXCompileShaderFromFileA( m_filePath.ToString().c_str(), 0, 0, m_entryPointName.c_str(), m_profile.c_str(), flags, &m_codeBuffer, &errorBuffer, &m_constantTable );
		}
		if( m_codeBuffer == 0 || FAILED( result ) )
		{
			m_errorMessage = static_cast< char * >(errorBuffer->GetBufferPointer());
			errorBuffer->Release();
			errorBuffer = 0;
			throw unify::Exception( "Failed to create shader \"" + m_filePath.ToString() + "\": " + m_errorMessage + "\n" );
		}
	}
	else if( m_code.empty() )
	{
		if( m_assembly )
		{
			DWORD flags = 0;
			D3DXAssembleShader( m_code.c_str(), static_cast< unsigned int >(m_code.length() - 1), 0, NULL, flags, &m_codeBuffer, &errorBuffer );
		}
		else
		{
			DWORD flags = 0;
			D3DXCompileShader( m_code.c_str(), static_cast< unsigned int >(m_code.length() - 1), 0, 0, m_entryPointName.c_str(), m_profile.c_str(), flags, &m_codeBuffer, &errorBuffer, &m_constantTable );
		}

		if( m_codeBuffer == 0 || FAILED( result ) )
		{
			m_errorMessage = static_cast< char * >(errorBuffer->GetBufferPointer());
			errorBuffer->Release();
			errorBuffer = 0;
			throw unify::Exception( "Failed to create shader from code: " + m_errorMessage + "\n" );
		}
	}
	else
	{
		throw unify::Exception( "Attempted to create shader from unknown source!" );
	}

	m_constants.reset( new shader::ConstantBuffer );
	m_constants->AddBuffer();

	m_renderer->GetDxDevice()->CreateVertexShader( (unsigned long *)m_codeBuffer->GetBufferPointer(), &m_shader );


	m_worldMatrixHandle = m_constantTable->GetConstantByName( 0, "worldMatrix" );
	if ( m_worldMatrixHandle == 0 )
	{
		m_worldMatrixHandle = m_constantTable->GetConstantByName( 0, "world" );
	}

	if ( m_worldMatrixHandle != 0 )
	{
		m_constants->AddVariable( 0, { "world", ElementFormat::Matrix4x4, 1 } );
	}
											
	m_viewMatrixHandle = m_constantTable->GetConstantByName( 0, "viewMatrix" );
	if ( m_viewMatrixHandle == 0 )
	{
		m_viewMatrixHandle = m_constantTable->GetConstantByName( 0, "view" );
	}

	if ( m_viewMatrixHandle != 0 )
	{
		m_constants->AddVariable( 0, { "view", ElementFormat::Matrix4x4, 1 } );
	}


	m_projectionMatrixHandle = m_constantTable->GetConstantByName( 0, "projectionMatrix" );
	if ( m_projectionMatrixHandle == 0 )
	{
		m_projectionMatrixHandle = m_constantTable->GetConstantByName( 0, "projection" );
	}

	if ( m_projectionMatrixHandle != 0 )
	{
		m_constants->AddVariable( 0, { "projection", ElementFormat::Matrix4x4, 1 } );
	}

	m_lockData.resize( m_constants->GetVariables( 0 ).size() );
	
	m_vertexDeclaration->Build( m_renderer, *this );
	m_created = true;
}

const ConstantBuffer * VertexShader::GetConstants() const
{
	return m_constants.get();
}

void VertexShader::LockConstants( size_t buffer, unify::DataLock & lock )
{
	lock.SetLock( &m_lockData[0], sizeof( unify::Matrix ) * m_lockData.size(), false, 0 );
}

void VertexShader::UnlockConstants( size_t buffer, unify::DataLock & lock )
{
	// Do nothing, we just needed m_lockData updated.
}

void VertexShader::SetVertexDeclaration( VertexDeclaration::ptr vertexDeclaration )
{
	m_vertexDeclaration = vertexDeclaration;
}

VertexDeclaration::ptr VertexShader::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

const void * VertexShader::GetBytecode() const
{
	return nullptr;
}

size_t VertexShader::GetBytecodeLength() const
{
	return 0;
}

void VertexShader::Use()
{
	m_vertexDeclaration->Use();

	HRESULT result = S_OK;

	size_t i = 0;

	if( m_worldMatrixHandle != 0 )
	{
		result = m_constantTable->SetMatrix( m_renderer->GetDxDevice(), m_worldMatrixHandle, (D3DXMATRIX*)&m_lockData[ i++ ] );
	}
	if( m_viewMatrixHandle != 0 )
	{
		result = m_constantTable->SetMatrix( m_renderer->GetDxDevice(), m_viewMatrixHandle, (D3DXMATRIX*)&m_lockData[ i++ ] );
	}
	if( m_projectionMatrixHandle != 0 )
	{
		result = m_constantTable->SetMatrix( m_renderer->GetDxDevice(), m_projectionMatrixHandle, (D3DXMATRIX*)&m_lockData[ i++ ] );
	}

	result = m_renderer->GetDxDevice()->SetVertexShader( m_shader );
	if( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}
}

std::string VertexShader::GetSource() const
{
	return m_filePath.ToXPath();
}

bool VertexShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

bool VertexShader::IsTrans() const
{
	return m_parameters.trans;
}
