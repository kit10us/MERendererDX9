// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/IndexBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/OutOfBounds.h>
#include <me/exception/NotImplemented.h>
#include <atlbase.h>

using namespace medx9;
using namespace me;
using namespace render;

IndexBuffer::IndexBuffer( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_buffer{ 0 }
	, m_locked( false )
	, m_usage( BufferUsage::Default )
	, m_length( 0 )
	, m_stride( 0 )
{
}

IndexBuffer::IndexBuffer( IRenderer * renderer, IndexBufferParameters parameters )
	: IndexBuffer( renderer )
{
	Create( parameters );
}

IndexBuffer::~IndexBuffer()
{
	Destroy();
}

void IndexBuffer::Create( IndexBufferParameters parameters )
{
	Destroy();

	size_t bufferIndex = 0;

	HRESULT hr = S_OK;

	m_length = parameters.countAndSource[0].count;

	auto dxDevice = m_renderer->GetDxDevice();

	unsigned int createFlags = FLAG00;
	// Some of these are not required.
	if ( unify::CheckFlag( parameters.flags, CreateFlags::DoNotClip ) )	createFlags |= D3DUSAGE_DONOTCLIP;
	if ( unify::CheckFlag( parameters.flags, CreateFlags::NPatches ) )	createFlags |= D3DUSAGE_NPATCHES;
	if ( unify::CheckFlag( parameters.flags, CreateFlags::Points ) )	createFlags |= D3DUSAGE_POINTS;
	if ( unify::CheckFlag( parameters.flags, CreateFlags::RTPatches ) )	createFlags |= D3DUSAGE_RTPATCHES;
	if ( unify::CheckFlag( parameters.flags, CreateFlags::SoftwareProcessing ) )	createFlags |= D3DUSAGE_SOFTWAREPROCESSING;
	if ( unify::CheckFlag( parameters.flags, CreateFlags::WriteOnly ) )	createFlags |= D3DUSAGE_WRITEONLY;

	D3DFORMAT d3dFormat;
	d3dFormat = D3DFMT_INDEX32;
	m_stride = sizeof( Index32 );

	m_createFlags = createFlags;
	m_usage = parameters.usage;

	D3DPOOL pool = D3DPOOL_DEFAULT;
	switch ( m_usage )
	{
	case BufferUsage::Default:
		pool = D3DPOOL_MANAGED;
		break;
	case BufferUsage::Immutable:
		pool = D3DPOOL_DEFAULT;
		createFlags |= D3DUSAGE_WRITEONLY;
		break;
	case BufferUsage::Dynamic:
		pool = D3DPOOL_MANAGED;
		break;
	case BufferUsage::Staging:
		//pool = D3DPOOL_SCRATCH;
		pool = D3DPOOL_SYSTEMMEM;
		break;
	}

	// Create Index Buffer...
	hr = dxDevice->CreateIndexBuffer(
		GetSizeInBytes(bufferIndex),
		createFlags,
		d3dFormat,
		pool,
		&m_buffer,
		NULL
	);
	if ( FAILED( hr ) )
	{
		throw exception::FailedToCreate( "Failed to create index buffer!" );
	}

	if ( parameters.countAndSource[bufferIndex].source )
	{
		// Create the index list...
		unify::DataLock lock;
		Lock( bufferIndex, lock );
		memcpy( lock.GetData<unsigned char >(), parameters.countAndSource[bufferIndex].source, m_length * sizeof( Index32 ) );
		Unlock( bufferIndex, lock );
	}
}

void IndexBuffer::Destroy()
{
	if ( m_buffer )
	{
		m_buffer->Release();
	}
	m_buffer = nullptr;
	m_length = 0;
}

void IndexBuffer::Lock( size_t bufferIndex, unify::DataLock & lock )
{
	HRESULT hr;
	unsigned char * data;
	hr = m_buffer->Lock( 0, 0, (void**)&data, 0 );
	if ( FAILED( hr ) )
	{
		lock.Invalidate();
		throw exception::FailedToLock( "Failed to lock index buffer!" );
	}

	lock.SetLock( data, GetStride(bufferIndex), GetLength(bufferIndex), unify::DataLock::ReadWrite, 0 );
	m_locked = true;
}

void IndexBuffer::LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	assert( m_buffer != 0 );
	if ( m_locked )
	{
		throw exception::FailedToLock( "Index buffer already locked!" );
	}

	void * data = 0;
	if ( FAILED( m_buffer->Lock( 0, 0, &data, D3DLOCK_READONLY ) ) )
	{
		throw exception::FailedToLock( "Failed to lock indices!" );
	}
	lock.SetLock( data, GetStride(bufferIndex), GetLength(bufferIndex), unify::DataLock::Readonly, 0 );
	bool & locked = *const_cast<bool*>(&m_locked); // Break const for locking.
	locked = false;
}

void IndexBuffer::Unlock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( m_locked == false )
	{
		return;
	}
	m_buffer->Unlock();
	m_locked = false;
}

void IndexBuffer::UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( m_locked == false )
	{
		return;
	}
	m_buffer->Unlock();
	bool & locked = *const_cast<bool*>(&m_locked); // Break const for locking.
	locked = false;
}

bool IndexBuffer::Valid() const
{
	return m_buffer != nullptr;
}

void IndexBuffer::Use() const
{
	if ( !m_buffer )
	{
		return;
	}

	auto dxDevice = m_renderer->GetDxDevice();

	HRESULT hr = dxDevice->SetIndices( m_buffer );
	if ( FAILED( hr ) )
	{
		throw unify::Exception( "Failed to use index buffer!" );
	}
}

bool IndexBuffer::Locked( size_t bufferIndex ) const
{
	return m_locked;
}

BufferUsage::TYPE IndexBuffer::GetUsage( size_t bufferIndex ) const
{
	return m_usage;
}

size_t IndexBuffer::GetStride( size_t bufferIndex ) const
{
	return m_stride;
}

size_t IndexBuffer::GetLength( size_t bufferIndex ) const
{
	return m_length;
}

size_t IndexBuffer::GetSizeInBytes( size_t bufferIndex ) const
{
	return m_stride * m_length;
}
