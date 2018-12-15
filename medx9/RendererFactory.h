// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <medx9/DirectX.h>

#pragma warning( push )
#pragma warning( disable:4005 ) // Ignore "warning C4005: 'MAKEFOURCC': macro redefinition" due to mmsyscom.h(153) and dds.h(62).

#include <mewos/WindowsOS.h>
#include <atlbase.h>

#pragma warning( pop )

#include <memory>

namespace medx9
{
	class RendererFactory : public me::render::IRendererFactory
	{
	public:
		me::render::IRenderer * Produce( me::os::IOS * os, me::render::Display display, size_t index ) override;
	};
}