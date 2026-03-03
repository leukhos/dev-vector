#include "vector.hpp"

#include <doctest/doctest.h>

#include <stdexcept>
#include <utility>

// Constructors and assignments

TEST_CASE("vector - Default ctor") {
  dev::vector<int> v;

  CHECK(v.empty());
}

TEST_CASE("vector - Parametrized ctor") {
  SUBCASE("Parameter is positive") {
    dev::vector<int> v(10, 1);

    CHECK(v.size() == 10);
    for (auto i{0uz}; i < v.size(); i++) {
      CHECK(v[i] == 1);
    }
  }

  SUBCASE("Parameter is null") {
    dev::vector<int> v(0, 1);

    CHECK(v.empty());
  }
}

TEST_CASE("vector - Range ctor") {
  int range[] = {1, 2, 3};
  dev::vector<int> v(range, range + 3);

  CHECK(v.size() == 3);
  CHECK(v.capacity() >= v.size());
  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

TEST_CASE("vector - Initializer list ctor") {
  dev::vector<int> v{1, 2, 3};

  CHECK(v.size() == 3);
  CHECK(v.capacity() >= v.size());
  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

TEST_CASE("vector - Copy ctor") {
  dev::vector<int> v1{1, 2, 3};

  dev::vector<int> v2(v1);

  REQUIRE(v1.size() == v2.size());
  for (auto i{0uz}; i < v1.size(); i++) {
    CHECK(v1[i] == i + 1);
    CHECK(v1[i] == v2[i]);
  }
}

TEST_CASE("vector - Move ctor") {
  dev::vector<int> v1{1, 2, 3};
  auto expected_size = v1.size();

  dev::vector<int> v2(std::move(v1));

  CHECK(v1.size() == 0);
  CHECK(v1.capacity() == 0);
  CHECK(v2.size() == expected_size);
  for (auto i{0uz}; i < v2.size(); i++) {
    CHECK(v2[i] == i + 1);
  }
}

TEST_CASE("vector - Copy assignment") {
  dev::vector<int> v1{1, 2, 3};
  dev::vector<int> v2;

  SUBCASE("Copy to other") {
    v2 = v1;

    REQUIRE(v1.size() == v2.size());
    for (auto i{0uz}; i < v1.size(); i++) {
      CHECK(v1[i] == i + 1);
      CHECK(v1[i] == v2[i]);
    }
  }

  SUBCASE("Copy to self") {
    v1 = v1;

    CHECK(v1.size() == 3);
    for (auto i{0uz}; i < v1.size(); i++) {
      CHECK(v1[i] == i + 1);
    }
  }
}

TEST_CASE("vector - Move assignment") {
  dev::vector<int> v1{1, 2, 3};
  dev::vector<int> v2;

  v2 = std::move(v1);

  CHECK(v1.size() == 0);
  CHECK(v1.capacity() == 0);
  CHECK(v2.size() == 3);
  for (auto i{0uz}; i < v2.size(); i++) {
    CHECK(v2[i] == i + 1);
  }
}

TEST_CASE("vector - assign count and value") {
  dev::vector<int> v(2, 1);

  SUBCASE("new_capacity > m_capacity") {
    v.assign(5, 2);

    CHECK(v.size() == 5);
  }

  SUBCASE("new_capacity <= m_capacity") {
    v.assign(2, 2);

    CHECK(v.size() == 2);
  }

  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == 2);
  }
}

TEST_CASE("vector - assign range") {
  dev::vector<int> v(2, 1);
  int range[] = {1, 2, 3};

  SUBCASE("new_capacity > m_capacity") {
    v.assign(range, range + 3);

    CHECK(v.size() == 3);
  }

  SUBCASE("new_capacity <= m_capacity") {
    v.assign(range, range + 2);

    CHECK(v.size() == 2);
  }

  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

TEST_CASE("vector - assign initializer list") {
  dev::vector<int> v(2, 1);

  SUBCASE("new_capacity > m_capacity") {
    v.assign({1, 2, 3});

    CHECK(v.size() == 3);
  }

  SUBCASE("new_capacity <= m_capacity") {
    v.assign({1, 2});

    CHECK(v.size() == 2);
  }

  for (auto i{0uz}; i < v.size(); i++) {
    CHECK(v[i] == i + 1);
  }
}

// Add test with std::string

// Element access

TEST_CASE("vector - at") {
  dev::vector<int> v{1, 2, 3};

  SUBCASE("index is valid") { CHECK(v.at(1) == 2); }

  SUBCASE("index is not valid throw an exeption") {
    CHECK_THROWS_AS(v.at(3), std::out_of_range);
    CHECK_THROWS_AS(v.at(-1), std::out_of_range);
  }
}

TEST_CASE("vector - front and back") {
  dev::vector<int> v{1, 2, 3};

  CHECK(v.front() == 1);
  CHECK(v.back() == 3);

  v.front() = 4;
  CHECK(v.front() == 4);
}
