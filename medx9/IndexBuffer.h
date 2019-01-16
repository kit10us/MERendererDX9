// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx9/IndexBuffer.h>
#include <medx9/Renderer.h>
#include <unify/unify.h>
#include <unify/Flags.h>
#include <memory>

namespace medx9
{
	class IndexBuffer : public me::render::IIndexBuffer
	{
	public:
		struct CreateFlags
		{
			enum TYPE
			{
				DoNotClip = FLAG01,
				Dynamic = FLAG02,
				NPatches = FLAG03,
				Points = FLAG04,
				RTPatches = FLAG05,
				SoftwareProcessing = FLAG06,
				WriteOnly = FLAG07,
			};
		};

		IndexBuffer( me::render::IRenderer * renderer );
		IndexBuffer( me::render::IRenderer * renderer, me::render::IndexBufferParameters parameters );
		~IndexBuffer();

	public: // me::render::IIndexBuffer
		void Create( me::render::IndexBufferParameters parameters );
		size_t GetStride( size_t bufferIndex ) const override;

	public: // me::render::IBuffer
		void Destroy();

		void Lock( size_t bufferIndex, unify::DataLock & lock ) override;
		void LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		void Unlock( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;

		bool Valid() const;
		
		void Use() const override;

		bool Locked( size_t bufferIndex ) const override;
		me::render::BufferUsage::TYPE GetUsage( size_t bufferIndex ) const;
		size_t GetLength( size_t bufferIndex ) const;
		size_t GetSizeInBytes( size_t bufferIndex ) const override;

	protected:
		const Renderer * m_renderer;
		unsigned int m_createFlags;
		IDirect3DIndexBuffer9 * m_buffer;
		bool m_locked;
		me::render::BufferUsage::TYPE m_usage;
		unsigned int m_stride; // Size of each item in the buffer.
		unsigned int m_length; // Number of items we can store in the buffer.
	};
}