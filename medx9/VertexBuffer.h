// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/render/IVertexBuffer.h>
#include <me/render/IRenderer.h>
#include <me/render/BufferUsage.h>
#include <me/render/VertexDeclaration.h>
#include <medx9/Renderer.h>
#include <unify/BBox.h>

namespace medx9
{
	class VertexBuffer : public me::render::IVertexBuffer
	{
	public:
		VertexBuffer( me::render::IRenderer * renderer );
		VertexBuffer( me::render::IRenderer * renderer, me::render::VertexBufferParameters parameters );
		~VertexBuffer();

		void Create( me::render::VertexBufferParameters parameters ) override;

		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;
		
		bool Valid() const;

		unify::BBox< float > & GetBBox() override;
		const unify::BBox< float > & GetBBox() const override;

		size_t GetStride( size_t bufferIndex ) const;
		size_t GetLength( size_t bufferIndex ) const;
		size_t GetSizeInBytes( size_t bufferIndex ) const;

	public: // IBuffer
		void Destroy() override;

		size_t GetBufferCount() const override;

		void Use(size_t startBuffer, size_t startSlot) const override;

		void Lock(size_t bufferIndex, unify::DataLock& lock) override;

		void LockReadOnly(size_t bufferIndex, unify::DataLock& lock) const override;

		void Unlock(size_t bufferIndex, unify::DataLock& lock) override;

		void UnlockReadOnly(size_t bufferIndex, unify::DataLock& lock) const override;

		bool Locked(size_t bufferIndex) const override;

		me::render::BufferUsage::TYPE GetUsage(size_t bufferIndex) const override;

	protected:
		const Renderer * m_renderer;
		std::vector< IDirect3DVertexBuffer9 * > m_buffers;
		me::render::VertexDeclaration::ptr m_vertexDeclaration;
		unify::BBox< float > m_bbox;
		std::vector< me::render::BufferUsage::TYPE > m_usage;
		std::vector< size_t > m_strides; // Size of each item in the buffer.
		std::vector< size_t > m_lengths; // Number of items we can store in the buffer.
		mutable std::vector< bool > m_locked;
	};
}