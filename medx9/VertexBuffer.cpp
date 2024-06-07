// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx9/VertexBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/NotImplemented.h>

using namespace medx9;
using namespace me;
using namespace render;

VertexBuffer::VertexBuffer( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >( renderer ) )
	, m_locked( false )
	, m_usage( BufferUsage::Default )
{
}

VertexBuffer::VertexBuffer( IRenderer * renderer, VertexBufferParameters parameters )
	: VertexBuffer( renderer )
{
	Create( parameters );
}
			  
VertexBuffer::~VertexBuffer()
{
	Destroy();
}

void VertexBuffer::Create( VertexBufferParameters parameters )
{
	Destroy();

	m_vertexDeclaration = parameters.vertexDeclaration;

	size_t bufferIndex = 0;
	for( auto countAndSource : parameters.countAndSource )
	{
		m_usage.push_back( parameters.usage );
		m_strides.push_back( m_vertexDeclaration->GetSizeInBytes( bufferIndex ) );
		m_lengths.push_back( parameters.countAndSource[bufferIndex].count );
		m_locked.push_back( false );

		// Ensure we have some sort of idea what we need to be...
		if( GetSizeInBytes(bufferIndex) == 0 )
		{
			throw exception::FailedToCreate( "Not a valid vertex buffer size!" );
		}

		auto dxDevice = m_renderer->GetDxDevice();

		unsigned int createFlags = FLAG00;
		D3DPOOL pool = D3DPOOL_DEFAULT;
		switch( m_usage[ bufferIndex ] )
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

		// Create Vertex Buffer...
		HRESULT hr;
		IDirect3DVertexBuffer9 * buffer{};
		hr = dxDevice->CreateVertexBuffer( (UINT)GetSizeInBytes(bufferIndex), createFlags, 0, pool, &buffer, 0 );
		// SAS TODO: OnFailedThrow( hr, "Failed to create vertex buffer!" );

		m_buffers.push_back( buffer );

		if( parameters.countAndSource[0].source )
		{
			unify::DataLock lock;
			Lock( bufferIndex, lock );
			lock.CopyBytesFrom( parameters.countAndSource[bufferIndex].source, 0, GetSizeInBytes(bufferIndex) );
			Unlock( bufferIndex, lock );
		}			 
		bufferIndex++;
	}
		
	m_bbox = parameters.bbox;
}

VertexDeclaration::ptr VertexBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

bool VertexBuffer::Valid() const
{
	return m_buffers.size() == m_strides.size();
}

unify::BBox< float > & VertexBuffer::GetBBox()
{
	return m_bbox;
}

const unify::BBox< float > & VertexBuffer::GetBBox() const
{
	return m_bbox;
}

size_t VertexBuffer::GetStride( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ];
}

size_t VertexBuffer::GetLength( size_t bufferIndex ) const
{
	return m_lengths[ bufferIndex ];
}

size_t VertexBuffer::GetSizeInBytes( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ] * m_lengths[ bufferIndex ];
}



void VertexBuffer::Destroy()
{
	for (auto&& buffer : m_buffers)
	{
		buffer->Release();
	}
	m_buffers.clear();
	m_lengths.clear();
	m_strides.clear();
}

size_t VertexBuffer::GetBufferCount() const
{
	// SAS TODO:
	return m_buffers.size();
}

void VertexBuffer::Use(size_t startBuffer, size_t startSlot) const
{
	auto dxDevice = m_renderer->GetDxDevice();
	for (size_t bufferIndex = 0, buffer_count = m_buffers.size(); bufferIndex < buffer_count; bufferIndex++)
	{
		unsigned int offsetInBytes = 0;
		HRESULT hr = S_OK;
		hr = dxDevice->SetStreamSource((UINT)bufferIndex, m_buffers[bufferIndex], offsetInBytes, (UINT)m_strides[bufferIndex]);
		if (WIN_FAILED(hr))
		{
			throw unify::Exception("VertexBuffer: Failed to SetStreamSource!");
		}
	}
}

void VertexBuffer::Lock(size_t bufferIndex, unify::DataLock& lock)
{
	HRESULT hr;
	unsigned char* data;
	hr = m_buffers[bufferIndex]->Lock(0, 0, (void**)&data, 0);
	if (WIN_FAILED(hr))
	{
		lock.Invalidate();
		throw exception::FailedToLock("Failed to lock vertex buffer!");
	}

	lock.SetLock(data, m_vertexDeclaration->GetSizeInBytes(bufferIndex), GetLength(bufferIndex), unify::DataLockAccess::ReadWrite, 0);
	m_locked[bufferIndex] = true;
}

void VertexBuffer::LockReadOnly(size_t bufferIndex, unify::DataLock& lock) const
{
	HRESULT hr;
	unsigned char* data;
	hr = m_buffers[bufferIndex]->Lock(0, 0, (void**)&data, D3DLOCK_READONLY);
	if (WIN_FAILED(hr))
	{
		lock.Invalidate();
		throw exception::FailedToLock("Failed to lock vertex buffer!");
	}

	lock.SetLock(data, m_vertexDeclaration->GetSizeInBytes(bufferIndex), GetLength(bufferIndex), unify::DataLockAccess::Readonly, 0);
}

void VertexBuffer::Unlock(size_t bufferIndex, unify::DataLock& lock)
{
	if (!m_buffers[bufferIndex] || !m_locked[bufferIndex]) return;
	m_buffers[bufferIndex]->Unlock();
	m_locked[bufferIndex] = FALSE;	// altering data in a constant function
}

void VertexBuffer::UnlockReadOnly(size_t bufferIndex, unify::DataLock& lock) const
{
	if (!m_buffers[bufferIndex] || !m_locked[bufferIndex]) return;
	m_buffers[bufferIndex]->Unlock();
	m_locked[bufferIndex] = false;
}

bool VertexBuffer::Locked(size_t bufferIndex) const
{
	return m_locked[bufferIndex];
}

BufferUsage::TYPE VertexBuffer::GetUsage(size_t bufferIndex) const
{
	return m_usage[bufferIndex];
}
