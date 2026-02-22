/*
  rakarrack - aass multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  RingBuffer.hpp - Lock-free single-producer single-consumer ring buffer
  for real-time audio thread to GUI thread communication.

  Usage:
    Producer (RT thread): push(item)
    Consumer (GUI thread): pop(item)

  Both ends are wait-free. If the buffer is full, push() returns false
  (caller should drop the data — never block in the RT thread).
*/

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <new>
#include <type_traits>

/// Lock-free SPSC (Single-Producer Single-Consumer) ring buffer.
/// @tparam T      Element type (must be trivially copyable for RT safety)
/// @tparam Capacity  Number of slots (must be power of 2)
template <typename T, std::size_t Capacity>
class RingBuffer
{
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "RingBuffer capacity must be a power of 2");
    static_assert(std::is_trivially_copyable_v<T>,
                  "RingBuffer element must be trivially copyable");

public:
    RingBuffer() = default;

    /// Push an element into the buffer (producer / RT thread).
    /// @return true if the element was written, false if the buffer was full.
    [[nodiscard]] bool push(const T& item) noexcept
    {
        const auto head = m_head.load(std::memory_order_relaxed);
        const auto next = (head + 1) & kMask;
        if (next == m_tail.load(std::memory_order_acquire))
            return false; // full
        m_data[head] = item;
        m_head.store(next, std::memory_order_release);
        return true;
    }

    /// Pop an element from the buffer (consumer / GUI thread).
    /// @return true if an element was read, false if the buffer was empty.
    [[nodiscard]] bool pop(T& item) noexcept
    {
        const auto tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire))
            return false; // empty
        item = m_data[tail];
        m_tail.store((tail + 1) & kMask, std::memory_order_release);
        return true;
    }

    /// Check if the buffer is empty (consumer side).
    [[nodiscard]] bool empty() const noexcept
    {
        return m_tail.load(std::memory_order_acquire) ==
               m_head.load(std::memory_order_acquire);
    }

    /// Drain all elements, keeping only the most recent one.
    /// @return true if at least one element was available.
    [[nodiscard]] bool pop_latest(T& item) noexcept
    {
        bool got_one = false;
        while (pop(item))
            got_one = true;
        return got_one;
    }

    /// Reset the buffer to empty state.
    /// Only safe when neither producer nor consumer is active.
    void reset() noexcept
    {
        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

private:
    static constexpr std::size_t kMask = Capacity - 1;

    // Separate cache lines to avoid false sharing between producer and consumer.
    alignas(64) std::atomic<std::size_t> m_head{0};
    alignas(64) std::atomic<std::size_t> m_tail{0};
    alignas(64) std::array<T, Capacity>  m_data{};
};
