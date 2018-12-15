// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/VertexConstruct.h>
#include <medx9/Renderer.h>
#include <medx9/VertexShader.h>
#include <unify/ColorUnit.h>
#include <algorithm>

using namespace medx9;
using namespace me;
using namespace render;

D3DVERTEXELEMENT9 ToDX( VertexElement element )
{
	D3DVERTEXELEMENT9 out {};
	
	if ( unify::StringIs( "POSITION", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_POSITION;
	}
	else if ( unify::StringIs( "BLENDWEIGHT", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_BLENDWEIGHT;
	}
	else if ( unify::StringIs( "BLENDINDICES", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_BLENDINDICES;
	}
	else if ( unify::StringIs( "NORMAL", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_NORMAL;
	}
	else if ( unify::StringIs( "PSIZE", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_PSIZE;
	}
	else if ( unify::StringIs( "TEXCOORD", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_TEXCOORD;
	}
	else if ( unify::StringIs( "TANGENT", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_TANGENT;
	}
	else if ( unify::StringIs( "BINORMAL", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_BINORMAL;
	}
	else if ( unify::StringIs( "TESSFACTOR", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_TESSFACTOR;
	}
	else if ( unify::StringIs( "POSITIONT", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_POSITIONT;
	}
	else if ( unify::StringIs( "COLOR", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_COLOR;
	}
	else if ( unify::StringIs( "FOG", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_FOG;
	}
	else if ( unify::StringIs( "DEPTH", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_DEPTH;
	}
	else if ( unify::StringIs( "SAMPLE", element.SemanticName ) )
	{
		out.Usage = D3DDECLUSAGE_SAMPLE;
	}
	else
	{
		throw unify::Exception( "Failed to convert string Vertex Declaration usage!" );
	}
			 	
	out.UsageIndex = element.SemanticIndex;
	out.Offset = element.AlignedByteOffset;
	out.Stream = element.InputSlot;

	switch ( element.Format )
	{
	case ElementFormat::Float1: out.Type = D3DDECLTYPE_FLOAT1; break;
	case ElementFormat::Float2: out.Type = D3DDECLTYPE_FLOAT2; break;
	case ElementFormat::Float3: out.Type = D3DDECLTYPE_FLOAT3; break;
	case ElementFormat::Float4: out.Type = D3DDECLTYPE_FLOAT4; break;
	case ElementFormat::ColorUNorm: out.Type = D3DDECLTYPE_D3DCOLOR; break;
	case ElementFormat::Unknown: out.Type = D3DDECLTYPE_UNUSED; break;
	}

	return out;
}


VertexConstruct::VertexConstruct( IRenderer * renderer, const VertexDeclaration & vd  )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
{
	std::vector< D3DVERTEXELEMENT9 > elements;

	for ( auto & e : vd.Elements() )
	{
		elements.push_back( ToDX( e ) );
	}

	elements.push_back( D3DDECL_END() );

	HRESULT hr = m_renderer->GetDxDevice()->CreateVertexDeclaration( &elements[0], &m_layout );
	if ( FAILED( hr ) )
	{
		throw unify::Exception( "Failed to create vertex declaration!" );
	}		   
}

VertexConstruct::~VertexConstruct()
{
	m_layout = nullptr;
}

void VertexConstruct::Use() const
{
	HRESULT result = m_renderer->GetDxDevice()->SetVertexDeclaration( m_layout );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex declaration!" );
	}
}

