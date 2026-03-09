#include "vector.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Constructors and assignments

TEST_CASE("vector::vector()") {
  dev::vector<int> v;

  CHECK(v.empty());
}

TEST_CASE("vector::vector(size_type count, const T& value)") {
  int value{1};

  SUBCASE("count > 0") {
    int count{10};

    dev::vector<int> v(count, value);

    CHECK(v.size() == count);
    CHECK(v.capacity() >= v.size());
    CHECK(std::all_of(v.begin(), v.end(),
                      [&value](int i) { return i == value; }));
  }

  SUBCASE("count == 0") {
    int count{0};

    dev::vector<int> v(0, value);

    THEN("the vector is empty") { CHECK(v.empty()); }
  }
}

TEST_CASE("vector::vector(InputIt first, InputIt last)") {
  int range[] = {1, 2, 3};

  dev::vector<int> v(std::begin(range), std::end(range));

  CHECK(v.size() == 3);
  CHECK(v.capacity() >= v.size());
  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

TEST_CASE("vector::vector(std::initializer_list<T> init)") {
  dev::vector<int> v{1, 2, 3};

  CHECK(v.size() == 3);
  CHECK(v.capacity() >= v.size());
  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

TEST_CASE("vector::vector(const vector& other)") {
  dev::vector<int> v1{1, 2, 3};

  dev::vector<int> v2(v1);

  CHECK(v2 == v1);
}

TEST_CASE("vector::vector(vector&& other)") {
  dev::vector<int> v1{1, 2, 3};

  dev::vector<int> v2(std::move(v1));

  CHECK(v2 == dev::vector<int>{1, 2, 3});
  CHECK(v1.empty());
}

TEST_CASE("vector::operator=(const vector& other)") {
  dev::vector<int> v1{1, 2, 3};
  dev::vector<int> v2{4, 5, 6};

  SUBCASE("copy to other") {
    v2 = v1;

    CHECK(v2 == v1);
    CHECK(std::is_lvalue_reference_v<decltype(v2 = v1)>);
  }

  SUBCASE("copy to self") {
    auto& rv1 = v1;
    rv1 = v1;

    CHECK(v1 == dev::vector{1, 2, 3});
  }
}

TEST_CASE("vector::operator=(vector&& other)") {
  dev::vector<int> v1{1, 2, 3};
  dev::vector<int> v2{4, 5, 6};

  v2 = std::move(v1);

  CHECK(v2 == dev::vector<int>{1, 2, 3});
  CHECK(v1.empty());
  CHECK(std::is_lvalue_reference_v<decltype(v2 = std::move(v1))>);
}

TEST_CASE("vector::assign(size_type count, const T& value)") {
  dev::vector<int> v(2, 1);
  int value{2};

  SUBCASE("count > v.capacity()") {
    int count{5};

    v.assign(count, value);

    CHECK(v == dev::vector<int>(count, value));
    CHECK(v.capacity() >= count);
  }

  SUBCASE("count <= v.capacity()") {
    int count{2};
    int capacity = v.capacity();

    v.assign(count, value);

    CHECK(v == dev::vector<int>(count, value));
    CHECK(v.capacity() == capacity);
  }
}

TEST_CASE("vector::assign(InputIt first, InputIt last)") {
  dev::vector<int> v(2, 1);
  int range[] = {1, 2, 3};

  SUBCASE("std::distance(first, last) > v.capacity()") {
    v.assign(range, range + 3);

    CHECK(v == dev::vector<int>{1, 2, 3});
    CHECK(v.capacity() >= 3);
  }

  SUBCASE("std::distance(first, last) <= v.capacity()") {
    v.assign(range, range + 2);

    CHECK(v == dev::vector<int>{1, 2});
    CHECK(v.capacity() == 2);
  }
}

TEST_CASE("vector::assign(std::initializer_list<T> ilist)") {
  dev::vector<int> v(2, 1);

  SUBCASE("ilist.size() > v.capacity()") {
    v.assign({1, 2, 3});

    CHECK(v == dev::vector<int>{1, 2, 3});
    CHECK(v.capacity() >= 3);
  }

  SUBCASE("ilist.size() <= v.capacity()") {
    v.assign({1, 2});

    CHECK(v == dev::vector<int>{1, 2});
    CHECK(v.capacity() == 2);
  }
}

// Element access

TEST_CASE("vector::at(size_type pos)") {
  dev::vector<int> v{1, 2, 3};

  SUBCASE("pos < v.size()") {
    size_t pos = 1uz;

    CHECK(v.at(pos) == 2);
    CHECK(std::is_lvalue_reference_v<decltype(v.at(pos))>);
  }

  SUBCASE("pos >= v.size()") {
    size_t pos = 3uz;

    CHECK_THROWS_AS(v.at(pos), std::out_of_range);
  }
}

TEST_CASE("vector::front()") {
  dev::vector<int> v{1, 2, 3}; // non-empty vector

  CHECK(v.front() == 1);
  CHECK(std::is_lvalue_reference_v<decltype(v.front())>);
}

TEST_CASE("vector::back()") {
  dev::vector<int> v{1, 2, 3}; // non-empty vector

  CHECK(v.back() == 3);
  CHECK(std::is_lvalue_reference_v<decltype(v.back())>);
}

// Capacity

struct ThrowOnCopy {
  inline static bool should_throw = false;
  int value;
  explicit ThrowOnCopy(int v) : value{v} {}
  ThrowOnCopy(const ThrowOnCopy& other) : value{other.value} {
    if (should_throw) {
      throw std::runtime_error("copy failed");
    }
  }
  ThrowOnCopy(ThrowOnCopy&& other)
      : value(other.value) {} // no noexcept → forces copy path
};

TEST_CASE("vector::reserve(size_type new_cap)") {
  SUBCASE("empty vector") {
    dev::vector<int> v;
    REQUIRE(v.empty());
    size_t new_cap{10uz};

    v.reserve(new_cap);

    CHECK(v.capacity() >= new_cap);
    CHECK(v.empty());
  }

  SUBCASE("non-empty vector") {

    dev::vector<int> v{1, 2, 3};
    REQUIRE(v.capacity() == 3);
    size_t old_cap = v.capacity();

    SUBCASE("new_cap <= v.capacity()") {
      size_t new_cap{2uz};

      v.reserve(new_cap);

      CHECK(v.capacity() == old_cap); // do nothing
      CHECK(v == dev::vector<int>{1, 2, 3});
    }

    SUBCASE("new_cap > v.capacity()") {
      size_t new_cap{5uz};

      v.reserve(new_cap);

      CHECK(v.capacity() >= new_cap);
      CHECK(v == dev::vector<int>{1, 2, 3});
    }
  }

  SUBCASE("T throws on copy") {
    REQUIRE_FALSE(std::is_nothrow_move_constructible_v<ThrowOnCopy>);
    dev::vector<ThrowOnCopy> v(3, ThrowOnCopy{42});

    ThrowOnCopy::should_throw = true;
    CHECK_THROWS(v.reserve(10));
    ThrowOnCopy::should_throw = false;
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 3);
  }
}

// Modifiers

TEST_CASE("vector::clear()") {
  dev::vector<int> v{1, 2, 3};
  auto capacity = v.capacity();

  v.clear();

  CHECK(v.empty());
  CHECK(v.size() == 0);
  CHECK(v.capacity() == capacity);
}

TEST_CASE("vector::insert(const_iterator pos, const T& value)") {
  // An invalid pos is undefined behavior
  dev::vector<int> v{1, 2, 3};
  const int value{4};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    auto it = empty.insert(empty.begin(), value);

    CHECK(empty == dev::vector<int>{4});
    CHECK(it == empty.begin());
  }

  SUBCASE("size == capacity") {
    REQUIRE(v.capacity() == 3);
    auto pos = v.begin();

    SUBCASE("external element") {

      auto it = v.insert(pos, value);

      CHECK(v == dev::vector<int>{4, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("self-reference element") {
      auto it = v.insert(v.begin(), v[2]);

      CHECK(v == dev::vector<int>{3, 1, 2, 3});
      CHECK(it == v.begin());
    }
  }

  SUBCASE("size < capacity") {
    v.reserve(10);

    SUBCASE("insert at the front") {
      auto pos = v.cbegin();

      auto it = v.insert(pos, value);

      CHECK(v == dev::vector<int>{4, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("insert in the middle") {
      auto pos = v.cbegin() + 1;

      auto it = v.insert(pos, value);

      CHECK(v == dev::vector<int>{1, 4, 2, 3});
      CHECK(it == pos);
    }

    SUBCASE("insert at the end") {
      auto pos = v.cend();

      auto it = v.insert(pos, value);

      CHECK(v == dev::vector<int>{1, 2, 3, 4});
      CHECK(it == v.begin() + 3);
    }
  }
}

TEST_CASE("vector::insert(const_iterator pos, T&& value)") {
  // An invalid pos is undefined behavior
  dev::vector<int> v{1, 2, 3};
  int value{4};

  SUBCASE("empty vector") {
    dev::vector<int> empty;
    int empty_value{4};

    auto it = empty.insert(empty.begin(), std::move(empty_value));

    CHECK(empty == dev::vector<int>{4});
    CHECK(it == empty.begin());
  }

  SUBCASE("size == capacity") {
    REQUIRE(v.capacity() == 3);

    auto it = v.insert(v.begin(), std::move(value));

    CHECK(v == dev::vector<int>{4, 1, 2, 3});
    CHECK(it == v.begin());
  }

  SUBCASE("size < capacity") {
    v.reserve(10);

    SUBCASE("insert at the front") {
      auto it = v.insert(v.begin(), std::move(value));

      CHECK(v == dev::vector<int>{4, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("insert in the middle") {
      auto it = v.insert(v.begin() + 1, std::move(value));

      CHECK(v == dev::vector<int>{1, 4, 2, 3});
      CHECK(it == v.begin() + 1);
    }

    SUBCASE("insert at the end") {
      auto it = v.insert(v.end(), std::move(value));

      CHECK(v == dev::vector<int>{1, 2, 3, 4});
      CHECK(it == v.begin() + 3);
    }
  }
}

TEST_CASE(
    "vector::insert(const_iterator pos, size_type count, const T& value)") {
  dev::vector<int> v{1, 2, 3};
  size_t count{2uz};
  const int value{4};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    auto it = empty.insert(empty.begin(), count, value);

    CHECK(empty == dev::vector<int>{4, 4});
    CHECK(it == empty.begin());
  }

  SUBCASE("size + count >= capacity") {
    v.reserve(4); // size + count = 5; capacity = 4

    SUBCASE("external element") {
      auto it = v.insert(v.begin(), count, value);

      CHECK(v == dev::vector<int>{4, 4, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("self-reference element") {
      auto it = v.insert(v.begin(), count, v[0]);

      CHECK(v == dev::vector<int>{1, 1, 1, 2, 3});
      CHECK(it == v.begin());
    }
  }

  SUBCASE("size + count < capacity") {
    v.reserve(10);

    SUBCASE("insert at the front") {
      auto it = v.insert(v.begin(), count, value);

      CHECK(v == dev::vector<int>{4, 4, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("insert in the middle") {
      auto it = v.insert(v.begin() + 1, count, value);

      CHECK(v == dev::vector<int>{1, 4, 4, 2, 3});
      CHECK(it == v.begin() + 1);
    }

    SUBCASE("insert at the end") {
      auto it = v.insert(v.end(), count, value);

      CHECK(v == dev::vector<int>{1, 2, 3, 4, 4});
      CHECK(it == v.begin() + 3);
    }
  }
}

TEST_CASE("vector::insert(const_iterator pos, InputIt first, InputIt last)") {
  dev::vector<int> v{1, 2, 3};
  int range[] = {4, 5};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    auto it = empty.insert(empty.begin(), range, range + 2);

    CHECK(empty == dev::vector<int>{4, 5});
    CHECK(it == empty.begin());
  }

  SUBCASE("size + (last - first)  >= capacity") {
    REQUIRE(v.capacity() == 3);

    SUBCASE("external range") {
      auto it = v.insert(v.begin(), range, range + 2);

      CHECK(v == dev::vector<int>{4, 5, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("self-reference range") {
      auto it = v.insert(v.begin(), v.begin() + 1, v.end());

      CHECK(v == dev::vector<int>{2, 3, 1, 2, 3});
      CHECK(it == v.begin());
    }
  }

  SUBCASE("size + (last - first) < capacity") {
    v.reserve(10);

    SUBCASE("insert at the front") {
      auto it = v.insert(v.begin(), range, range + 2);

      CHECK(v == dev::vector<int>{4, 5, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("insert in the middle") {
      auto it = v.insert(v.begin() + 1, range, range + 2);

      CHECK(v == dev::vector<int>{1, 4, 5, 2, 3});
      CHECK(it == v.begin() + 1);
    }

    SUBCASE("insert at the end") {
      auto it = v.insert(v.end(), range, range + 2);

      CHECK(v == dev::vector<int>{1, 2, 3, 4, 5});
      CHECK(it == v.begin() + 3);
    }
  }

  SUBCASE("empty range") {
    auto it = v.insert(v.begin() + 1, range, range);

    CHECK(v == dev::vector<int>{1, 2, 3});
    CHECK(it == v.begin() + 1);
  }
}

TEST_CASE(
    "vector::insert(const_iterator pos, std::initializer_list<T> ilist)") {
  dev::vector<int> v{1, 2, 3};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    auto it = empty.insert(empty.begin(), {4, 5});

    CHECK(empty == dev::vector<int>{4, 5});
    CHECK(it == empty.begin());
  }

  SUBCASE("size + ilist.size() >= capacity") {
    REQUIRE(v.capacity() == 3);

    auto it =
        v.insert(v.begin(), {4, 5}); // initializer_list is always external

    CHECK(v == dev::vector<int>{4, 5, 1, 2, 3});
    CHECK(it == v.begin());
  }

  SUBCASE("size + ilist.size() < capacity") {
    v.reserve(10);

    SUBCASE("insert at the front") {
      auto it = v.insert(v.begin(), {4, 5});

      CHECK(v == dev::vector<int>{4, 5, 1, 2, 3});
      CHECK(it == v.begin());
    }

    SUBCASE("insert in the middle") {
      auto it = v.insert(v.begin() + 1, {4, 5});

      CHECK(v == dev::vector<int>{1, 4, 5, 2, 3});
      CHECK(it == v.begin() + 1);
    }

    SUBCASE("insert at the end") {
      auto it = v.insert(v.end(), {4, 5});

      CHECK(v == dev::vector<int>{1, 2, 3, 4, 5});
      CHECK(it == v.begin() + 3);
    }
  }

  SUBCASE("empty ilist") {
    auto it = v.insert(v.begin() + 1, {});

    CHECK(v == dev::vector<int>{1, 2, 3});
    CHECK(it == v.begin() + 1);
  }
}

TEST_CASE("vector::push_back(const T& value)") {
  dev::vector<int> v{1, 2, 3};
  const int value{4};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    empty.push_back(value);

    CHECK(empty == dev::vector<int>{4});
    CHECK(empty.size() == 1);
    CHECK(empty.capacity() >= 1);
  }

  SUBCASE("size == capacity") {
    REQUIRE(v.capacity() == 3);

    SUBCASE("external element") {
      v.push_back(value);

      CHECK(v == dev::vector<int>{1, 2, 3, 4});
      CHECK(v.size() == 4);
      CHECK(v.capacity() >= 4);
    }

    SUBCASE("self-reference element") {
      v.push_back(v[0]);

      CHECK(v == dev::vector<int>{1, 2, 3, 1});
      CHECK(v.size() == 4);
      CHECK(v.capacity() >= 4);
    }
  }

  SUBCASE("size < capacity") {
    v.reserve(10);

    v.push_back(value);

    CHECK(v == dev::vector<int>{1, 2, 3, 4});
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 10);
  }
}

TEST_CASE("vector::push_back(T&& value)") {
  dev::vector<int> v{1, 2, 3};
  int value{4};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    empty.push_back(std::move(value));

    CHECK(empty == dev::vector<int>{4});
    CHECK(empty.size() == 1);
    CHECK(empty.capacity() >= 1);
  }

  SUBCASE("size == capacity") {
    REQUIRE(v.capacity() == 3);

    v.push_back(std::move(value));

    CHECK(v == dev::vector<int>{1, 2, 3, 4});
    CHECK(v.size() == 4);
    CHECK(v.capacity() >= 4);
  }

  SUBCASE("size < capacity") {
    v.reserve(10);

    v.push_back(std::move(value));

    CHECK(v == dev::vector<int>{1, 2, 3, 4});
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 10);
  }
}

TEST_CASE("vector::emplace_back(Args&&... args)") {
  dev::vector<int> v{1, 2, 3};

  SUBCASE("empty vector") {
    dev::vector<int> empty;

    auto &ref = empty.emplace_back(4);

    CHECK(empty == dev::vector<int>{4});
    CHECK(empty.size() == 1);
    CHECK(empty.capacity() >= 1);
    CHECK(ref == 4);
    CHECK(&ref == &empty.back());
  }

  SUBCASE("size == capacity") {
    REQUIRE(v.capacity() == 3);

    auto &ref = v.emplace_back(4);

    CHECK(v == dev::vector<int>{1, 2, 3, 4});
    CHECK(v.size() == 4);
    CHECK(v.capacity() >= 4);
    CHECK(ref == 4);
    CHECK(&ref == &v.back());
  }

  SUBCASE("size < capacity") {
    v.reserve(10);

    auto &ref = v.emplace_back(4);

    CHECK(v == dev::vector<int>{1, 2, 3, 4});
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 10);
    CHECK(ref == 4);
    CHECK(&ref == &v.back());
  }
}

TEST_CASE("vector::pop_back()") {
  dev::vector<int> v{1, 2, 3};
  auto capacity = v.capacity();

  v.pop_back();

  CHECK(v == dev::vector<int>{1, 2});
  CHECK(v.size() == 2);
  CHECK(v.capacity() == capacity);
}

TEST_CASE("vector::resize(size_type count)") {
  dev::vector<int> v{1, 2, 3};

  SUBCASE("count == size") {
    v.resize(3);

    CHECK(v == dev::vector<int>{1, 2, 3});
  }

  SUBCASE("count < size") {
    auto capacity = v.capacity();

    v.resize(1);

    CHECK(v == dev::vector<int>{1});
    CHECK(v.capacity() == capacity);
  }

  SUBCASE("count > size") {
    SUBCASE("count <= capacity") {
      v.reserve(10);

      v.resize(5);

      CHECK(v == dev::vector<int>{1, 2, 3, int{}, int{}});
    }

    SUBCASE("count > capacity") {
      REQUIRE(v.capacity() == 3);

      v.resize(5);

      CHECK(v == dev::vector<int>{1, 2, 3, int{}, int{}});
    }
  }
}

TEST_CASE("vector::resize(size_type count, const T& value)") {
  dev::vector<int> v{1, 2, 3};
  const int value{9};

  SUBCASE("count == size") {
    v.resize(3, value);

    CHECK(v == dev::vector<int>{1, 2, 3});
  }

  SUBCASE("count < size") {
    auto capacity = v.capacity();

    v.resize(1, value);

    CHECK(v == dev::vector<int>{1});
    CHECK(v.capacity() == capacity);
  }

  SUBCASE("count > size") {
    SUBCASE("count <= capacity") {
      v.reserve(10);

      v.resize(5, value);

      CHECK(v == dev::vector<int>{1, 2, 3, 9, 9});
      CHECK(v.capacity() == 10);
    }

    SUBCASE("count > capacity") {
      REQUIRE(v.capacity() == 3);

      SUBCASE("external element") {
        v.resize(5, value);

        CHECK(v == dev::vector<int>{1, 2, 3, 9, 9});
        CHECK(v.capacity() >= 5);
      }

      SUBCASE("self-reference element") {
        v.resize(5, v[0]);

        CHECK(v == dev::vector<int>{1, 2, 3, 1, 1});
        CHECK(v.capacity() >= 5);
      }
    }
  }
}