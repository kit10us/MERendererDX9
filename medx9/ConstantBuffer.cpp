#include <medx9/ConstantBuffer.h>

using namespace medx9;
using namespace me::render;

// SAS TODO: Whole file.
ConstantBuffer::ConstantBuffer(const me::render::IRenderer* renderer, me::render::ConstantBufferParameters parameters)
	: m_parameters{ parameters }
{
}

ConstantBuffer::~ConstantBuffer()
{
}

ConstantBufferParameters ConstantBuffer::GetParameters() const
{
	return m_parameters;
}

const ConstantTable* ConstantBuffer::GetTable() const
{
	return &m_parameters.constantTable;
}

void ConstantBuffer::Create(ConstantBufferParameters parameters)
{
}

void ConstantBuffer::Destroy()
{
}

size_t ConstantBuffer::GetBufferCount() const
{
	return 0;
}

void ConstantBuffer::Update(const RenderInfo& renderInfo, const unify::Matrix* world, size_t world_size)
{
}

void ConstantBuffer::Use(size_t startSlot, size_t startBuffer)
{
}

void ConstantBuffer::LockConstants(size_t bufferIndex, unify::DataLock& lock)
{
}

void ConstantBuffer::UnlockConstants(size_t buffer, unify::DataLock& lock)
{
}

ResourceType::TYPE ConstantBuffer::GetType() const
{
	return m_parameters.type;
}

BufferUsage::TYPE ConstantBuffer::GetUsage() const
{
	return m_parameters.usage;
}
