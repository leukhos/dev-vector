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
  SUBCASE("count > 0") {
    dev::vector<int> v(10, 1);

    CHECK(v.size() == 10);
    CHECK(v.capacity() >= v.size());
    CHECK(std::all_of(v.begin(), v.end(), [](int i) { return i == 1; }));
  }

  SUBCASE("count == 0") {
    dev::vector<int> v(0, 1);

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

  SUBCASE("count > v.capacity()") {
    v.assign(5, 2);

    CHECK(v == dev::vector<int>(5, 2));
    CHECK(v.capacity() >= 3);
  }

  SUBCASE("count <= v.capacity()") {
    v.assign(2, 2);

    CHECK(v == dev::vector<int>(2, 2));
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

    v.reserve(10);

    CHECK(v.capacity() >= 10);
    CHECK(v.empty());
  }

  SUBCASE("non-empty vector") {

    dev::vector<int> v{1, 2, 3};
    REQUIRE(v.capacity() == 3);

    SUBCASE("new_cap <= v.capacity()") {
      v.reserve(2);

      CHECK(v.capacity() == 3); // do nothing
      CHECK(v == dev::vector<int>{1, 2, 3});
    }

    SUBCASE("new_cap > v.capacity()") {
      v.reserve(5);

      CHECK(v.capacity() >= 5);
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