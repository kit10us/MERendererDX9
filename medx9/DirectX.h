// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved
#pragma once

// A common header to include Direct-X and Windows in a clean manor.
#include <port/win/Windows.h>

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#else // NDEBUG
#endif

#pragma warning ( disable: 4221 ) // unused lib
#pragma warning ( disable: 4006 ) // duplicate definition

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9d" )
#else // NDEBUG
#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )
#endif


#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9shader.h>
