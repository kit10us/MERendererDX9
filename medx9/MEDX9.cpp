// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <mewos/WindowsOS.h>
#include <medx9/RendererFactory.h>
#include <memory.h>
#include <me/game/IGame.h>

void Deleter( mewos::WindowsOS * factory )
{
	delete factory;
}

extern "C" __declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element );

__declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element )
{
	using namespace me;

	auto os = dynamic_cast<mewos::IWindowsOS*>(gameInstance->GetOS());

	os->SetRenderFactory(me::render::IRendererFactory::ptr{ new medx9::RendererFactory(os) });

	// Load display setup...
	for (auto&& node : element->Children("display"))
	{
		bool fullscreen = node.GetAttributeElse< bool >("fullscreen", false);
		int width = node.GetAttribute< int >("width");
		int height = node.GetAttribute< int >("height");
		int x = node.GetAttributeElse< int >("x", 0);
		int y = node.GetAttributeElse< int >("y", 0);
		float nearZ = node.GetAttributeElse< float >("nearz", 0.0f);
		float farZ = node.GetAttributeElse< float >("farz", 1000.0f);

		render::Display display{};
		if (fullscreen)
		{
			display = render::Display::CreateFullscreenDisplay(unify::Size< float >((float)width, (float)height));
		}
		else
		{
			display = render::Display::CreateWindowedDisplay(unify::Size< float >((float)width, (float)height), unify::V2< float >((float)x, (float)y));
		}

		display.SetNearZ(nearZ);
		display.SetFarZ(farZ);

		os->AddDisplay(display);
	}

	return true;
}

