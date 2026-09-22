#pragma once

#include "types.hpp"

namespace vx
{

class arena_allocator
{
  public:
    explicit arena_allocator(usize capacity);
    ~arena_allocator();

    arena_allocator(const arena_allocator &) = delete;
    arena_allocator &operator=(const arena_allocator &) = delete;

    [[nodiscard]] void *allocate(usize size, usize align = alignof(std::max_align_t));

    void reset() noexcept;
    void clear() noexcept;

  private:
    std::byte *m_buffer{nullptr};
    usize m_capacity{0};
    usize m_offset{0};
};

} // namespace vx
