// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx9/Renderer.h>
#include <me/render/IVertexConstruct.h>
#include <me/render/VertexDeclaration.h>
#include <unify/unify.h>
#include <unify/String.h>
#include <unify/DataLock.h>
#include <atlbase.h>
#include <map>

namespace medx9
{
	/// <summary> 
	/// Defines the structure of the data in a vertex. 
	/// </summary>
	class VertexConstruct : public me::render::IVertexConstruct
	{
	public:
		VertexConstruct( me::render::IRenderer * renderer, const me::render::VertexDeclaration & vd );
		~VertexConstruct();

		void Use() const override;

	private:
		const Renderer * m_renderer;
		CComPtr< IDirect3DVertexDeclaration9 > m_layout;
	};
}