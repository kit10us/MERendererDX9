// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx9/RendererFactory.h>
#include <medx9/Renderer.h>

using namespace medx9;

me::render::IRenderer * RendererFactory::Produce( me::os::IOS * os, me::render::Display display, size_t index )
{
	return new medx9::Renderer( os, display, index );
}
