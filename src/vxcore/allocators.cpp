#include "allocators.hpp"
#include <new>

namespace vx
{

arena_allocator::arena_allocator(usize capacity) : m_capacity(capacity)
{
    if (capacity > 0)
    {
        m_buffer = static_cast<std::byte *>(::operator new[](capacity));
    }
}

arena_allocator::~arena_allocator()
{
    clear();
}

void *arena_allocator::allocate(usize size, usize align)
{
    if (!m_buffer)
        return nullptr;

    usize current_ptr = reinterpret_cast<usize>(m_buffer + m_offset);
    usize offset = (align - (current_ptr % align)) % align;

    if (m_offset + offset + size > m_capacity)
    {
        return nullptr;
    }

    m_offset += offset;
    void *ptr = m_buffer + m_offset;
    m_offset += size;

    return ptr;
}

void arena_allocator::reset() noexcept
{
    m_offset = 0;
}

void arena_allocator::clear() noexcept
{
    if (m_buffer)
    {
        ::operator delete[](m_buffer);
        m_buffer = nullptr;
    }

    m_capacity = 0;
    m_offset = 0;
}

} // namespace vx
