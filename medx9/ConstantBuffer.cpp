#include <medx9/ConstantBuffer.h>

using namespace medx9;
using namespace me::render;

// SAS TODO: Whole file.

ConstantBufferParameters ConstantBuffer::GetParameters() const
{
}

const ConstantTable* ConstantBuffer::GetTable() const
{
}

void ConstantBuffer::Create(ConstantBufferParameters parameters)
{
}

void ConstantBuffer::Destroy()
{
}

size_t ConstantBuffer::GetBufferCount() const
{
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
}

BufferUsage::TYPE ConstantBuffer::GetUsage() const
{
	return BufferUsage::TYPE::Default;
}
