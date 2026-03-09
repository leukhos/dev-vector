#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

namespace dev {
template <typename T> class vector {
public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = pointer;
  using const_iterator = const_pointer;

private:
  struct init_capacity_tag {
    explicit init_capacity_tag() = default;
  };
  constexpr static init_capacity_tag init_capacity{};

  vector(init_capacity_tag, size_type cap)
      : m_alloc{}, m_data{m_alloc.allocate(cap)}, m_capacity{cap} {}

public:
  vector() {};

  vector(size_type count, const T& value)
    requires std::copy_constructible<T>
      : vector(init_capacity, count) {
    std::uninitialized_fill_n(m_data, count, value);
    m_size = count;
  }

  template <class InputIt>
    requires std::input_iterator<InputIt> && std::copy_constructible<T>
  vector(InputIt first, InputIt last)
      : vector(init_capacity, std::distance(first, last)) {
    std::uninitialized_copy(first, last, m_data);
    m_size = std::distance(first, last);
  }

  vector(std::initializer_list<T> init)
    requires std::copy_constructible<T>
      : vector(init_capacity, init.size()) {
    std::uninitialized_copy(init.begin(), init.end(), m_data);
    m_size = init.size();
  }

  vector(const vector& other)
    requires std::copy_constructible<T>
      : vector(init_capacity, other.m_capacity) {
    std::uninitialized_copy_n(other.m_data, other.m_size, m_data);
    m_size = other.size();
  }

  vector(vector&& other) noexcept
      : m_data{std::exchange(other.m_data, nullptr)},
        m_size{std::exchange(other.m_size, 0)},
        m_capacity{std::exchange(other.m_capacity, 0)} {}

  ~vector() {
    std::destroy_n(m_data, m_size);
    m_alloc.deallocate(m_data, m_capacity);
  }

  vector& operator=(const vector& other)
    requires std::copy_constructible<T>
  {
    vector(other).swap(*this);
    return *this;
  }

  vector& operator=(vector&& other) noexcept {
    vector(std::move(other)).swap(*this);
    return *this;
  }

  void assign(size_type count, const T& value)
    requires std::copy_constructible<T>
  {
    if (count > m_capacity) {
      vector(count, value).swap(*this);
    } else {
      std::destroy_n(m_data, m_size);
      std::uninitialized_fill_n(m_data, count, value);
      m_size = count;
    }
  }

  template <class InputIt>
    requires std::input_iterator<InputIt> && std::copy_constructible<T>
  void assign(InputIt first, InputIt last) {
    if (static_cast<size_t>(std::distance(first, last)) > m_capacity) {
      vector(first, last).swap(*this);
    } else {
      std::destroy_n(m_data, m_size);
      std::uninitialized_copy(first, last, m_data);
      m_size = std::distance(first, last);
    }
  }

  void assign(std::initializer_list<T> ilist)
    requires std::copy_constructible<T>
  {
    if (ilist.size() > m_capacity) {
      vector(ilist).swap(*this);
    } else {
      std::destroy_n(m_data, m_size);
      std::uninitialized_copy(ilist.begin(), ilist.end(), m_data);
      m_size = ilist.size();
    }
  }

  // Element access
  reference at(size_type pos) {
    if (pos < m_size) {
      return m_data[pos];
    } else {
      throw std::out_of_range("pos >= size()");
    }
  }

  const_reference at(size_type pos) const {
    if (pos < m_size) {
      return m_data[pos];
    } else {
      throw std::out_of_range("pos >= size()");
    }
  }

  reference operator[](size_type pos) { return m_data[pos]; }
  const_reference operator[](size_type pos) const { return m_data[pos]; }
  reference front() { return m_data[0]; }
  const_reference front() const { return m_data[0]; }
  reference back() { return m_data[m_size - 1]; }
  const_reference back() const { return m_data[m_size - 1]; }
  pointer data() { return m_data; }
  const_pointer data() const { return m_data; }

  // Iterators
  iterator begin() { return m_data; }
  const_iterator begin() const { return m_data; }
  const_iterator cbegin() const { return m_data; }
  iterator end() { return m_data + m_size; }
  const_iterator end() const { return m_data + m_size; }
  const_iterator cend() const { return m_data + m_size; }

  // Capacity
  bool empty() const { return m_size == 0uz; }
  size_type size() const { return m_size; }
  size_type capacity() const { return m_capacity; }

  void reserve(size_type new_cap) {
    if (new_cap <= m_capacity) {
      return;
    }

    pointer new_data = m_alloc.allocate(new_cap);
    try {
      move_or_copy(m_data, m_size, new_data);
    } catch (const std::exception& e) {
      m_alloc.deallocate(new_data, new_cap);
      throw e;
    }

    std::destroy_n(m_data, m_size);
    m_alloc.deallocate(m_data, m_capacity);

    m_data = new_data;
    m_capacity = new_cap;
  }

  // Modifiers
  void clear() {
    std::destroy_n(m_data, m_size);
    m_size = 0;
  }

  iterator insert(const_iterator pos, const T& value)
    requires std::copy_constructible<T>
  {
    return m_data;
  }

  iterator insert(const_iterator pos, T&& value)
    requires std::move_constructible<T>
  {
    return m_data;
  }

  iterator insert(const_iterator pos, size_type count, const T& value)
    requires std::copy_constructible<T>
  {
    return m_data;
  }

  template <class InputIt>
    requires std::input_iterator<InputIt> && std::copy_constructible<T>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return m_data;
  }

  iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    requires std::copy_constructible<T>
  {
    return insert(pos, ilist.begin(), ilist.end());
  }

  void push_back(const T& value)
    requires std::copy_constructible<T>
  {
    if (m_size == m_capacity) {
      T temp_value = value;
      reserve(empty() ? 1uz : m_size * growth_factor);

      if constexpr (std::is_nothrow_move_constructible_v<T>) {
        std::construct_at(end(), std::move(temp_value));
      } else {
        std::construct_at(end(), temp_value);
      }
    } else {
      std::construct_at(end(), value);
    }

    m_size++;
  }

  void push_back(T&& value)
    requires std::move_constructible<T>
  {
    if (m_size == m_capacity) {
      reserve(empty() ? 1uz : m_size * growth_factor);
    }

    std::construct_at(end(), std::forward<T>(value));
    m_size++;
  }

  template <class... Args>
    requires std::constructible_from<T, Args...>
  reference emplace_back(Args&&... args) {
    if (m_size == m_capacity) {
      reserve(empty() ? 1uz : m_size * growth_factor);
    }

    std::construct_at(end(), std::forward<Args>(args)...);
    m_size++;

    return back();
  }

  void pop_back() {
    std::destroy_at(end() - 1);
    m_size--;
  }

  void resize(size_type count)
    requires std::default_initializable<T>
  {
    if (count == m_size) {
      return;
    } else if (count < m_size) {
      std::destroy(begin() + count, end());
    } else {
      reserve(count);
      std::uninitialized_value_construct(begin() + m_size, begin() + count);
    }

    m_size = count;
  }

  void resize(size_type count, const T& value)
    requires std::copy_constructible<T>
  {
    if (count == m_size) {
      return;
    } else if (count < m_size) {
      std::destroy(begin() + count, end());
    } else if (count > m_capacity) {
      const T v = value; // handling self-reference
      reserve(count);
      std::uninitialized_fill(begin() + m_size, begin() + count, v);
    } else {
      std::uninitialized_fill(begin() + m_size, begin() + count, value);
    }

    m_size = count;
  }

  void swap(vector& other) noexcept {
    using std::swap;

    swap(this->m_data, other.m_data);
    swap(this->m_size, other.m_size);
    swap(this->m_capacity, other.m_capacity);
  }

  // Non-member functions
  friend bool operator==(const vector& lhs, const vector& rhs)
    requires std::equality_comparable<T>
  {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  friend auto operator<=>(const vector& lhs, const vector& rhs)
    requires std::three_way_comparable<T>
  {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                                                  rhs.begin(), rhs.end());
  }

  friend void swap(const vector& lhs, const vector& rhs) noexcept {
    lhs.swap(rhs);
  }

private:
  bool is_full() const { return m_size == m_capacity; }

  void move_or_copy(pointer source, size_t size, pointer destination) {
    if constexpr (std::is_nothrow_move_constructible_v<T> ||
                  !std::is_copy_constructible_v<T>) {
      std::uninitialized_move_n(source, size, destination);
    } else {
      std::uninitialized_copy_n(source, size, destination);
    }
  }

  std::allocator<T> m_alloc;
  pointer m_data{nullptr};
  size_type m_size{0uz};
  size_type m_capacity{0uz};
  constexpr static unsigned short growth_factor{2};
};

} // namespace dev
