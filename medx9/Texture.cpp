// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/Texture.h>
#include <qxml/Document.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToLock.h>

// MS agressive macros.
#ifdef LoadImage
#undef LoadImage
#endif

using namespace medx9;
using namespace me;
using namespace render;

Texture::Texture( IRenderer * renderer, TextureParameters parameters )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
	, m_useColorKey( false )
	, m_created( false )
	, m_parameters( parameters )
{
	Create();
}

Texture::~Texture()
{
}

void Texture::Create()
{
	// Load from a file.
	if ( m_parameters.source.Empty() )
	{
		throw unify::Exception( "Not enough information to load Texture!" );
	}

	LoadHeader();

	LoadImage( m_parameters.source );

	m_created = true;
}

// Destroyes data, keeps header intact - thus no graphical footprint, yet we can still use it's statistics/dimensions in calculations.
void Texture::Destroy()
{
	m_texture = nullptr;
	m_created = false; // TODO: Can solve this from m_texture.
}

const unsigned int Texture::FileWidth() const
{
	return m_fileSize.width;
}

const unsigned int Texture::FileHeight() const
{
	return m_fileSize.height;
}

const unify::Size< unsigned int > & Texture::ImageSize() const
{
	return m_imageSize;
}

bool Texture::GetRenderable() const
{
	return m_parameters.renderable;
}

bool Texture::GetLockable() const
{
	return m_parameters.lockable;
}

void Texture::LockRect( unsigned int level, TextureLock & lock, const unify::Rect< long > * rect, bool readonly )
{
	if ( !m_created )
	{
		Create();
	}

	if( m_parameters.lockable == false )
	{
		throw exception::FailedToLock( "Texture is not lockable!" );
	}
		
	// Allow us to convert from our flag type to DX.
	unsigned int dxFlags = readonly ? D3DLOCK_READONLY : 0;

	D3DLOCKED_RECT d3dLockedRect;
	if( FAILED( m_texture->LockRect( level, &d3dLockedRect, (RECT*)rect, dxFlags ) ) )
	{
		lock.pBits = 0;
		lock.uStride = 0;
		throw unify::Exception( "Failed to lock texture: \"" + m_parameters.source.ToString() + "\"" );
	}

	lock.pBits = (unsigned char*)d3dLockedRect.pBits;
	lock.uStride = d3dLockedRect.Pitch;
	lock.bpp = 4;
	if( rect )
	{
		lock.uWidth = rect->right - rect->left;
		lock.uHeight = rect->bottom - rect->top;
	}
	else
	{
		lock.uWidth = m_imageSize.width;
		lock.uHeight = m_imageSize.height;
	}
}

void Texture::UnlockRect( unsigned int level )
{
	m_texture->UnlockRect( level );
}

// Load all possible info (short of bits) about the texture
void Texture::Preload()
{
	// Load any new image header file if there
	LoadHeader();

	// Attempt to load the image for information
	Create();

	// Destroy our texture.
	Destroy();
}

void Texture::LoadHeader()
{
	unify::Path imageHeaderFilepath{ ChangeExtension( m_parameters.source, ".xml" ) };

	// Check if the file exists...
	if ( !imageHeaderFilepath.Exists() )
	{
		return;	// It's ok to not have an IHF file.
	}

	m_spriteDictionary.LoadDictionary( imageHeaderFilepath );
}

// Load the actual image file...
void Texture::LoadImage( unify::Path filePath )
{
	// Release any previous texture
	Destroy();

	// Verify file exists
	if ( !filePath.Exists() )
	{
		throw unify::Exception( "Failed to load image, file not found! (" + filePath.ToString() + ")" );
	}

	// Set defaults that can be overrided by texture...
	D3DFORMAT textureFormat = D3DFMT_A8R8G8B8;

	// First, load the file to find out it's sizes...	
	D3DXIMAGE_INFO fileDesc;
	unsigned int dwFilter = D3DX_FILTER_NONE;


	D3DPOOL pool = D3DPOOL_DEFAULT;
	if( m_parameters.lockable )
	{
		pool = m_parameters.renderable ? D3DPOOL_MANAGED : D3DPOOL_SCRATCH;
	}

	HRESULT result;
	result = D3DXCreateTextureFromFileExA(
		m_renderer->GetDxDevice(),
		(LPCSTR)filePath.ToString().c_str(),
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
		D3DX_DEFAULT,
		0,
		textureFormat,
		pool,
		dwFilter,
		D3DX_FILTER_NONE,// (mip filter): D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
		m_colorKey,
		&fileDesc,
		NULL,
		&m_texture );

	if( FAILED( result ) )
	{
		Destroy();
		throw unify::Exception( "Failed to load image\"" + filePath.ToString() + "\"!" );
	}

	// Handle the colorkey
	if( m_useColorKey )	// D3DUSAGE_DYNAMIC must be set for D3DPOOL_DEFAULT textures (else D3DPOOL_MANAGED)
	{
		//D3DUtil_SetColorKey( m_texture[0][0], m_colorKey );
	}

	// Get the file dimensions...
	m_fileSize.width = fileDesc.Width;
	m_fileSize.height = fileDesc.Height;

	// Get the ACTUAL description of the Texture...
	D3DSURFACE_DESC textureDesc;
	m_texture->GetLevelDesc( 0, &textureDesc );
	m_imageSize.width = textureDesc.Width;
	m_imageSize.height = textureDesc.Height;
}

SpriteDictionary & Texture::GetSpriteDictionary()
{
	return m_spriteDictionary;
}

const SpriteDictionary & Texture::GetSpriteDictionary() const
{
	return m_spriteDictionary;
}

const TextureParameters * Texture::GetParameters() const
{
	return &m_parameters;
}

bool Texture::Reload()
{
	Destroy();
	Create();
	return true;
}

std::string Texture::GetSource() const
{
	return m_parameters.source.ToXPath();
}

