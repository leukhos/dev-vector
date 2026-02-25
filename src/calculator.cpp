// First-party headers
#include "calculator.hpp"

// Standard library headers
#include <stdexcept>

int Calculator::add(int first_value, int second_value) {
  return first_value + second_value;
}

int Calculator::subtract(int first_value, int second_value) {
  return first_value - second_value;
}

int Calculator::multiply(int first_value, int second_value) {
  return first_value * second_value;
}

double Calculator::divide(int first_value, int second_value) {
  if (second_value == 0) {
    throw std::invalid_argument("Division by zero");
  }

  return static_cast<double>(first_value) / second_value;
}

// Unit tests (embedded in source file)

#ifdef CALCULATOR_ENABLE_TEST

#include <doctest/doctest.h>
// #include <doctest/trompeloeil.hpp>

TEST_SUITE_BEGIN("unit");

TEST_CASE("Calculator - addition operations") {
  Calculator calculator;

  SUBCASE("addition with positive numbers") {
    // Arrange
    int first_value = 2;
    int second_value = 3;
    int expected_sum = 5;

    // Act
    int result = calculator.add(first_value, second_value);

    // Assert
    CHECK_EQ(result, expected_sum);
  }

  SUBCASE("addition with large positive numbers") {
    // Arrange
    int first_value = 10;
    int second_value = 15;
    int expected_sum = 25;

    // Act
    int result = calculator.add(first_value, second_value);

    // Assert
    CHECK(result == expected_sum);
  }

  SUBCASE("addition with negative numbers") {
    // Arrange
    int first_value = -2;
    int second_value = -3;
    int expected_sum = -5;

    // Act
    int result = calculator.add(first_value, second_value);

    // Assert
    CHECK(result == expected_sum);
  }

  SUBCASE("addition with mixed sign numbers") {
    // Arrange
    int first_value = -10;
    int second_value = 5;
    int expected_sum = -5;

    // Act
    int result = calculator.add(first_value, second_value);

    // Assert
    CHECK(result == expected_sum);
  }
}

TEST_CASE("Calculator - subtraction operations") {
  Calculator calculator;

  SUBCASE("subtraction with positive numbers") {
    // Arrange
    int first_value = 5;
    int second_value = 3;
    int expected_difference = 2;

    // Act
    int result = calculator.subtract(first_value, second_value);

    // Assert
    CHECK(result == expected_difference);
  }

  SUBCASE("subtraction resulting in negative") {
    // Arrange
    int first_value = 10;
    int second_value = 15;
    int expected_difference = -5;

    // Act
    int result = calculator.subtract(first_value, second_value);

    // Assert
    CHECK(result == expected_difference);
  }
}

TEST_CASE("Calculator - multiplication operations") {
  Calculator calculator;

  SUBCASE("multiplication with positive numbers") {
    // Arrange
    int first_value = 3;
    int second_value = 4;
    int expected_product = 12;

    // Act
    int result = calculator.multiply(first_value, second_value);

    // Assert
    CHECK(result == expected_product);
  }

  SUBCASE("multiplication with negative and positive") {
    // Arrange
    int first_value = -2;
    int second_value = 5;
    int expected_product = -10;

    // Act
    int result = calculator.multiply(first_value, second_value);

    // Assert
    CHECK(result == expected_product);
  }

  SUBCASE("multiplication with zero") {
    // Arrange
    int first_value = 0;
    int second_value = 100;
    int expected_product = 0;

    // Act
    int result = calculator.multiply(first_value, second_value);

    // Assert
    CHECK(result == expected_product);
  }
}

TEST_CASE("Calculator - division operations") {
  Calculator calculator;

  SUBCASE("division with even result") {
    // Arrange
    int first_value = 10;
    int second_value = 2;
    double expected_quotient = 5.0;

    // Act
    double result = calculator.divide(first_value, second_value);

    // Assert
    CHECK(result == doctest::Approx(expected_quotient));
  }

  SUBCASE("division with remainder") {
    // Arrange
    int first_value = 7;
    int second_value = 2;
    double expected_quotient = 3.5;

    // Act
    double result = calculator.divide(first_value, second_value);

    // Assert
    CHECK(result == doctest::Approx(expected_quotient));
  }

  SUBCASE("division with negative number") {
    // Arrange
    int first_value = -10;
    int second_value = 2;
    double expected_quotient = -5.0;

    // Act
    double result = calculator.divide(first_value, second_value);

    // Assert
    CHECK(result == doctest::Approx(expected_quotient));
  }

  SUBCASE("division by zero throws exception") {
    // Arrange
    int first_value = 10;
    int second_value = 0;

    // Act & Assert
    CHECK_THROWS_AS(calculator.divide(first_value, second_value),
                    std::invalid_argument);
  }
}

TEST_SUITE_END();

#endif
