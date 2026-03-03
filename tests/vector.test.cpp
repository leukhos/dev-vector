#include "vector.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Constructors and assignments

TEST_CASE("vector::vector()") {
  WHEN("default-constructing a vector") {
    dev::vector<int> v;

    THEN("the vector is empty") { CHECK(v.empty()); }
  }
}

TEST_CASE("vector::vector(size_type count, const T& value)") {
  WHEN("count > 0") {
    dev::vector<int> v(10, 1);

    THEN("the vector has count copies of value") {
      CHECK(v.size() == 10);
      CHECK(v.capacity() >= v.size());
      CHECK(std::all_of(v.begin(), v.end(), [](int i) { return i == 1; }));
    }
  }

  WHEN("count == 0") {
    dev::vector<int> v(0, 1);

    THEN("the vector is empty") { CHECK(v.empty()); }
  }
}

TEST_CASE("vector::vector(InputIt first, InputIt last)") {
  GIVEN("an integer range") {
    int range[] = {1, 2, 3};

    WHEN("constructing a vector from the range") {
      dev::vector<int> v(std::begin(range), std::end(range));

      THEN("the vector contains the range elements in order") {
        CHECK(v.size() == 3);
        CHECK(v.capacity() >= v.size());
        for (auto i{0uz}; i < v.size(); i++) {
          CHECK(v[i] == i + 1);
        }
      }
    }
  }
}

TEST_CASE("vector::vector(std::initializer_list<T> init)") {
  WHEN("constructing a vector from an initializer list") {
    dev::vector<int> v{1, 2, 3};

    THEN("the vector contains the initializer list elements in order") {
      CHECK(v.size() == 3);
      CHECK(v.capacity() >= v.size());
      for (auto i{0uz}; i < v.size(); i++) {
        CHECK(v[i] == i + 1);
      }
    }
  }
}

TEST_CASE("vector::vector(const vector& other)") {
  GIVEN("a vector v1") {
    dev::vector<int> v1{1, 2, 3};

    WHEN("copy-constructing v2 from v1") {
      dev::vector<int> v2(v1);

      THEN("v2 is equal to v1") { CHECK(v2 == v1); }
    }
  }
}

TEST_CASE("vector::vector(vector&& other)") {
  GIVEN("a vector v1") {
    dev::vector<int> v1{1, 2, 3};

    WHEN("move-constructing v2 from v1") {
      dev::vector<int> v2(std::move(v1));

      THEN("v2 contains v1's original elements") {
        CHECK(v2 == dev::vector<int>{1, 2, 3});

        AND_THEN("v1 is left empty") { CHECK(v1.empty()); }
      }
    }
  }
}

TEST_CASE("vector::operator=(const vector& other)") {
  GIVEN("two vectors v1 and v2") {
    dev::vector<int> v1{1, 2, 3};
    dev::vector<int> v2{4, 5, 6};

    WHEN("copy-assigning v1 to v2") {
      v2 = v1;

      THEN("v2 is equal to v1") {
        CHECK(v2 == v1);

        AND_THEN("the assignment returns an lvalue reference") {
          CHECK(std::is_lvalue_reference_v<decltype(v2 = v1)>);
        }
      }
    }

    WHEN("self-assigning v1") {
      v1 = v1;

      THEN("v1 is unchanged") { CHECK(v1 == dev::vector{1, 2, 3}); }
    }
  }
}

TEST_CASE("vector::operator=(vector&& other)") {
  GIVEN("two vectors v1 and v2") {
    dev::vector<int> v1{1, 2, 3};
    dev::vector<int> v2{4, 5, 6};

    WHEN("move-assigning v1 to v2") {
      v2 = std::move(v1);

      THEN("v2 contains v1's original elements") {
        CHECK(v2 == dev::vector<int>{1, 2, 3});

        AND_THEN("v1 is left empty") {
          CHECK(v1.empty());

          AND_THEN("the assignment returns an lvalue reference") {
            CHECK(std::is_lvalue_reference_v<decltype(v2 = std::move(v1))>);
          }
        }
      }
    }
  }
}

TEST_CASE("vector::assign(size_type count, const T& value)") {
  GIVEN("a vector v") {
    dev::vector<int> v(2, 1);

    WHEN("count > v.capacity()") {
      v.assign(5, 2);

      THEN("the vector holds count copies of value") {
        CHECK(v == dev::vector<int>(5, 2));

        AND_THEN("the capacity is updated") { CHECK(v.capacity() >= 3); }
      }
    }

    WHEN("count <= v.capacity()") {
      v.assign(2, 2);

      THEN("the vector holds count copies of value") {
        CHECK(v == dev::vector<int>(2, 2));
      }
    }
  }
}

TEST_CASE("vector::assign(InputIt first, InputIt last)") {
  GIVEN("a vector v") {
    dev::vector<int> v(2, 1);
    int range[] = {1, 2, 3};

    WHEN("std::distance(first, last) > v.capacity()") {
      v.assign(range, range + 3);

      THEN("the vector holds the range elements") {
        CHECK(v == dev::vector<int>{1, 2, 3});

        AND_THEN("the capacity is updated") { CHECK(v.capacity() >= 3); }
      }
    }

    WHEN("std::distance(first, last) <= v.capacity()") {
      v.assign(range, range + 2);

      THEN("the vector holds the range elements") {
        CHECK(v == dev::vector<int>{1, 2});
      }
    }
  }
}

TEST_CASE("vector::assign(std::initializer_list<T> ilist)") {
  GIVEN("a vector v") {
    dev::vector<int> v(2, 1);

    WHEN("ilist.size() > v.capacity()") {
      v.assign({1, 2, 3});

      THEN("the vector holds the initializer list elements") {
        CHECK(v == dev::vector<int>{1, 2, 3});

        AND_THEN("the capacity is updated") { CHECK(v.capacity() >= 3); }
      }
    }

    WHEN("ilist.size() <= v.capacity()") {
      v.assign({1, 2});

      THEN("the vector holds the initializer list elements") {
        CHECK(v == dev::vector<int>{1, 2});
      }
    }
  }
}

// Add test with std::string

// Element access

TEST_CASE("vector::at(size_type pos)") {
  GIVEN("a non-empty vector v") {
    dev::vector<int> v{1, 2, 3};

    WHEN("pos < v.size()") {
      size_t pos = 1uz;

      THEN("returns the element at pos") {
        CHECK(v.at(pos) == 2);

        AND_THEN("the return type is an lvalue reference") {
          CHECK(std::is_lvalue_reference_v<decltype(v.at(pos))>);
        }
      }
    }

    WHEN("pos >= v.size()") {
      size_t pos = 3uz;

      THEN("throws std::out_of_range") {
        CHECK_THROWS_AS(v.at(pos), std::out_of_range);
      }
    }
  }
}

TEST_CASE("vector::front()") {
  GIVEN("a non-empty vector v") {
    dev::vector<int> v{1, 2, 3};

    THEN("returns the first element") {
      CHECK(v.front() == 1);

      AND_THEN("the return type is an lvalue reference") {
        CHECK(std::is_lvalue_reference_v<decltype(v.front())>);
      }
    }
  }
}

TEST_CASE("vector::back()") {
  GIVEN("a non-empty vector v") {
    dev::vector<int> v{1, 2, 3};

    THEN("returns the last element") {
      CHECK(v.back() == 3);

      AND_THEN("the return type is an lvalue reference") {
        CHECK(std::is_lvalue_reference_v<decltype(v.back())>);
      }
    }
  }
}
