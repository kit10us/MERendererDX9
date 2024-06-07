// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <medx9/DirectX.h>
#include <mewos/WindowsOS.h>

#include <memory>

namespace medx9
{
	class RendererFactory : public me::render::IRendererFactory
	{
	public:
		RendererFactory(mewos::IWindowsOS* os);
		virtual ~RendererFactory();

	public: // me::render::IRendererFactory
		me::render::IRenderer * Produce(me::render::Display display, size_t index) override;

	private:
		mewos::IWindowsOS* m_os;
	};
}