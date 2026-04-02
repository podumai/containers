#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <lab/containers/vector.hpp>
#include <memory_resource>
#include <ranges>

constexpr auto kTestNumbers = {1, 2, 3, 4, 5};

constexpr auto kGetElementByIndex = [] [[nodiscard]] (std::size_t index) consteval noexcept -> int {
  auto begin = kTestNumbers.begin();
  std::advance(begin, index);
  return *begin;
};

TEST(MutableVectorIterator, DereferenceOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.begin();
  EXPECT_EQ(*begin, *kTestNumbers.begin());
}

TEST(MutableVectorIterator, UnaryIncrementOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto begin = filled_vector.begin();
    ++begin;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
  {
    auto begin = filled_vector.begin();
    begin++;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
}

TEST(MutableVectorIterator, UnaryDecrementOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto begin = filled_vector.begin();
    std::advance(begin, 2);
    --begin;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
  {
    auto begin = filled_vector.begin();
    std::advance(begin, 2);
    begin--;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
}

TEST(MutableVectorIterator, SubscriptOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    const auto& second = filled_vector.begin()[1];
    EXPECT_EQ(second, kGetElementByIndex(1));
  }
  {
    auto& second = filled_vector.begin()[1];
    EXPECT_EQ(second, kGetElementByIndex(1));
    second = -1;
    EXPECT_EQ(second, -1) << "Subscript operator in non const overload must provide ability to mutate vector elements";
  }
}

TEST(MutableVectorIterator, PlusEqualsOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.begin();
  begin += 1;
  EXPECT_EQ(*begin, kGetElementByIndex(1));
}

TEST(MutableVectorIterator, PlusOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto begin = filled_vector.begin() + 1;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
  {
    auto begin = 1 + filled_vector.begin();
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
}

TEST(MutableVectorIterator, MinusEqualsOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.begin();
  std::advance(begin, 1);
  begin -= 1;
  EXPECT_EQ(*begin, kGetElementByIndex(0));
}

TEST(MutableVectorIterator, MinusOperator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.begin();
  std::advance(begin, 1);
  begin = begin - 1;
  EXPECT_EQ(*begin, kGetElementByIndex(0));
}

TEST(MutableVectorIterator, MinusOperatorIterator) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto first = filled_vector.begin();
    auto second = filled_vector.begin();
    std::advance(second, 1);
    EXPECT_EQ(second - first, 1);
    EXPECT_EQ(first - second, -1);
  }
  {
    auto first1 = filled_vector.begin();
    auto first2 = filled_vector.begin();
    EXPECT_EQ(first1 - first2, 0);
    EXPECT_EQ(first2 - first1, 0);
  }
}

TEST(MutableVectorIterator, EqualOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_EQ(first1, first2);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_TRUE(!(first == second));
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_EQ(first1, first2);
    }
  }
}

TEST(MutableVectorIterator, NotEqualOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_FALSE(first1 != first2);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_NE(first, second);
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_FALSE(first1 != first2);
    }
  }
}

TEST(MutableVectorIterator, LessOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_FALSE(first1 < first2);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_LT(first, second);
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_FALSE(first1 < first2);
    }
  }
}

TEST(MutableVectorIterator, GreaterOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_FALSE(first1 > first2);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_FALSE(first > second);
      EXPECT_GT(second, first);
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_FALSE(first1 > first2);
      EXPECT_FALSE(first2 > first1);
    }
  }
}

TEST(MutableVectorIterator, LessEqualOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_LE(first1, first2);
    EXPECT_LE(first2, first1);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_LE(first, second);
      EXPECT_FALSE(second <= first);
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_LE(first1, first2);
      EXPECT_LE(first2, first1);
    }
  }
}

TEST(MutableVectorIterator, GreaterEqualOperator) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.begin();
    auto first2 = empty_vector.begin();
    EXPECT_GE(first1, first2);
    EXPECT_GE(first2, first1);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.begin();
      auto second = filled_vector.begin();
      std::advance(second, 1);
      EXPECT_FALSE(first >= second);
      EXPECT_GE(second, first);
    }
    {
      auto first1 = filled_vector.begin();
      auto first2 = filled_vector.begin();
      EXPECT_GE(first1, first2);
      EXPECT_GE(first2, first1);
    }
  }
}

TEST(MutableVectorIterator, ImplicitConstIteratorCast) {
  using ConstIterator = lab::containers::Vector<int>::ConstIterator;

  auto empty_vector = lab::containers::Vector<int>{};
  const auto const_begin = ConstIterator(empty_vector.begin());
}

TEST(ConstVectorIterator, DereferenceOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.cbegin();
  EXPECT_EQ(*begin, kGetElementByIndex(0));
}

TEST(ConstVectorIterator, UnaryIncrementOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto begin = filled_vector.cbegin();
    ++begin;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
  {
    auto begin = filled_vector.cbegin();
    begin++;
    EXPECT_EQ(*begin, kGetElementByIndex(1));
  }
}

TEST(ConstVectorIterator, UnaryDecrementOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto begin = filled_vector.cbegin();
    std::advance(begin, 1);
    --begin;
    EXPECT_EQ(*begin, kGetElementByIndex(0));
  }
  {
    auto begin = filled_vector.cbegin();
    std::advance(begin, 1);
    begin--;
    EXPECT_EQ(*begin, kGetElementByIndex(0));
  }
}

TEST(ConstVectorIterator, SubscriptOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  const auto& second = filled_vector.cbegin()[1];
  EXPECT_EQ(second, kGetElementByIndex(1));
}

TEST(ConstVectorIterator, PlusEqualsOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.cbegin();
  begin += 1;
  EXPECT_EQ(*begin, kGetElementByIndex(1));
}

TEST(ConstVectorIterator, PlusOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto first = filled_vector.cbegin();
    const auto second = first + 1;
    EXPECT_EQ(*second, kGetElementByIndex(1));
  }
  {
    auto first = filled_vector.cbegin();
    const auto second = 1 + first;
    EXPECT_EQ(*second, kGetElementByIndex(1));
  }
}

TEST(ConstVectorIterator, MinusEqualsOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.cbegin();
  std::advance(begin, 1);
  begin -= 1;
  EXPECT_EQ(*begin, kGetElementByIndex(0));
}

TEST(ConstVectorIterator, MinusOperator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto begin = filled_vector.cbegin();
  std::advance(begin, 1);
  begin = begin - 1;
  EXPECT_EQ(*begin, kGetElementByIndex(0));
}

TEST(ConstVectorIterator, MinusOperatorIterator) {
  const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    auto first = filled_vector.cbegin();
    auto second = filled_vector.cbegin();
    std::advance(second, 1);
    EXPECT_EQ(second - first, 1);
    EXPECT_EQ(first - second, -1);
  }
  {
    auto first1 = filled_vector.cbegin();
    auto first2 = filled_vector.cbegin();
    EXPECT_EQ(first1 - first2, 0);
    EXPECT_EQ(first2 - first1, 0);
  }
}

TEST(ConstVectorIterator, EqualOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_EQ(first1, first2);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_TRUE(!(first == second));
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_EQ(first1, first2);
    }
  }
}

TEST(ConstVectorIterator, NotEqualOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_FALSE(first1 != first2);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_NE(first, second);
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_FALSE(first1 != first2);
    }
  }
}

TEST(ConstVectorIterator, LessOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_FALSE(first1 < first2);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_LT(first, second);
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_FALSE(first1 < first2);
    }
  }
}

TEST(ConstVectorIterator, GreaterOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_FALSE(first1 > first2);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_FALSE(first > second);
      EXPECT_GT(second, first);
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_FALSE(first1 > first2);
      EXPECT_FALSE(first2 > first1);
    }
  }
}

TEST(ConstVectorIterator, LessEqualOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_LE(first1, first2);
    EXPECT_LE(first2, first1);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_LE(first, second);
      EXPECT_FALSE(second <= first);
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_LE(first1, first2);
      EXPECT_LE(first2, first1);
    }
  }
}

TEST(ConstVectorIterator, GreaterEqualOperator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto first1 = empty_vector.cbegin();
    auto first2 = empty_vector.cbegin();
    EXPECT_GE(first1, first2);
    EXPECT_GE(first2, first1);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      auto first = filled_vector.cbegin();
      auto second = filled_vector.cbegin();
      std::advance(second, 1);
      EXPECT_FALSE(first >= second);
      EXPECT_GE(second, first);
    }
    {
      auto first1 = filled_vector.cbegin();
      auto first2 = filled_vector.cbegin();
      EXPECT_GE(first1, first2);
      EXPECT_GE(first2, first1);
    }
  }
}

TEST(ConstVectorIterator, ImplicitMutableIteratorCast) {
  using MutableIterator = lab::containers::Vector<int>::Iterator;

  const auto empty_vector = lab::containers::Vector<int>{};
  const auto begin = MutableIterator(empty_vector.cbegin());
}

TEST(Vector, MutableForRangeLoop) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    for (const auto& value : empty_vector) {
      FAIL() << "Empty vector must not enter for-range loop";
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    for (const auto& [value, test_value] : std::views::zip(filled_vector, kTestNumbers)) {
      EXPECT_EQ(value, test_value);
    }
  }
}

TEST(Vector, ConstForRangeLoop) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    for (auto begin = empty_vector.cbegin(), end = empty_vector.cend(); begin != end; ++begin) {
      FAIL() << "Empty vector must not enter for-range loop";
    }
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    auto current_test_value = kTestNumbers.begin();
    for (auto begin = filled_vector.cbegin(), end = filled_vector.cend(); begin != end; ++begin) {
      EXPECT_EQ(*begin, *current_test_value);
      ++current_test_value;
    }
  }
}

TEST(Vector, ReverseMutableForRangeLoop) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    for (auto begin = empty_vector.rbegin(), end = empty_vector.rend(); begin != end; ++begin) {
      FAIL() << "Empty vector must not enter for-range loop";
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    auto reversed_test_range = std::views::reverse(kTestNumbers);
    auto current_test_value = reversed_test_range.begin();
    for (auto begin = filled_vector.rbegin(), end = filled_vector.rend(); begin != end; ++begin) {
      EXPECT_EQ(*begin, *current_test_value);
      ++current_test_value;
    }
  }
}

TEST(Vector, ReverseConstForRangeLoop) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    for (auto begin = empty_vector.crbegin(), end = empty_vector.crend(); begin != end; ++begin) {
      FAIL() << "Empty vector must not enter for-range loop";
    }
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    auto reversed_test_range = std::views::reverse(kTestNumbers);
    auto current_test_value = reversed_test_range.begin();
    for (auto begin = filled_vector.crbegin(), end = filled_vector.crend(); begin != end; ++begin) {
      EXPECT_EQ(*begin, *current_test_value);
      ++current_test_value;
    }
  }
}

TEST(Vector, Data) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    EXPECT_EQ(empty_vector.Data(), nullptr);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    EXPECT_NE(filled_vector.Data(), nullptr);
  }
}

TEST(Vector, Size) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    EXPECT_EQ(empty_vector.Size(), 0);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    EXPECT_EQ(filled_vector.Size(), kTestNumbers.size());
  }
}

TEST(Vector, Capacity) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    EXPECT_EQ(empty_vector.Capacity(), 0);
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    EXPECT_EQ(filled_vector.Capacity(), kTestNumbers.size());
  }
}

TEST(Vector, MaxSize) {
  const auto vector = lab::containers::Vector<int>{};
  EXPECT_EQ(vector.MaxSize(), std::allocator_traits<std::allocator<int>>::max_size(std::allocator<int>{}));
}

TEST(Vector, Empty) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    EXPECT_TRUE(empty_vector.Empty());
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    EXPECT_FALSE(filled_vector.Empty());
  }
}

TEST(Vector, AtAccess) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    const auto result = empty_vector.At(0);
    if (!result) {
      EXPECT_EQ(result.error(), lab::containers::VectorErrors::kOutOfRange);
    } else {
      FAIL() << "Empty vector must not return -> VectorErrors::kEmpty";
    }
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    {
      const auto result = filled_vector.At(0);
      if (result) {
        EXPECT_EQ(result.value(), *kTestNumbers.begin());
      } else {
        FAIL()
          << "Non empty vector at position in range [0, Size() - 1] must not return -> VectorErrors::* (Size() >= 1)";
      }
    }
    {
      const auto result = filled_vector.At(kTestNumbers.size());
      if (!result) {
        EXPECT_EQ(result.error(), lab::containers::VectorErrors::kOutOfRange);
      } else {
        FAIL() << "Non empty vector at position not in range [0, Size() - 1] must return -> VectorErrors::kOutOfRange "
                  "(Size() >= 1)";
      }
    }
  }
}

TEST(Vector, AtModificaiton) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto result = filled_vector.At(0);
  if (result) {
    result.value().get() = -1;
    EXPECT_EQ(
      result.value(), -1
    ) << "Non empty vector in range [0, Size() - 1] in non const overload must modify value (Size() >= 1)";
  } else {
    FAIL() << "Non empty vector at position in range [0, Size() - 1] must not return -> VectorErrors::* (Size() >= 1)";
  }
}

TEST(Vector, Subscript) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  {
    const auto result = filled_vector[0];
    EXPECT_EQ(result, *kTestNumbers.begin());
  }
  {
    auto& result = filled_vector[0];
    result = -1;
    EXPECT_EQ(
      result, -1
    ) << "Non empty vector in range [0, Size() - 1] in non const overload must modify value (Size() >= 1)";
  }
}

TEST(Vector, FrontAccess) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    const auto result = empty_vector.Front();
    if (!result) {
      EXPECT_EQ(result.error(), lab::containers::VectorErrors::kEmpty)
        << "Empty vector must return -> VectorErrors::kEmpty (Size() == 0)";
    } else {
      FAIL() << "Empty vector must return -> VectorErrors::kEmpty (Size() == 0)";
    }
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    auto result = filled_vector.Front();
    if (result) {
      EXPECT_EQ(result, *kTestNumbers.begin());
    } else {
      FAIL() << "Non empty vector must not return -> VectorErrors::*";
    }
  }
}

TEST(Vector, FrontModification) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto result = filled_vector.Front();
  if (result) {
    result.value().get() = -1;
    EXPECT_EQ(
      result, -1
    ) << "Non empty vector in range [0, Size() - 1] in non const overload must modify value (Size() >= 1)";
  } else {
    FAIL() << "Non empty vector must not return -> VectorErrors::*";
  }
}

TEST(Vector, BackAccess) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    const auto result = empty_vector.Back();
    if (!result) {
      EXPECT_EQ(result.error(), lab::containers::VectorErrors::kEmpty)
        << "Empty vector must return -> VectorErrors::kEmpty";
    } else {
      FAIL() << "Empty vector must return -> VectorErrors::kEmpty";
    }
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto result = filled_vector.Back();
    if (result) {
      constexpr auto last_element = [] [[nodiscard]] consteval noexcept {
        auto last_element_position = kTestNumbers.begin();
        std::advance(last_element_position, kTestNumbers.size() - 1);
        return *last_element_position;
      }();
      EXPECT_EQ(result, last_element);
    } else {
      FAIL() << "Non empty vector must not return -> VectorErrors::*";
    }
  }
}

TEST(Vector, BackModification) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  auto result = filled_vector.Back();
  if (result) {
    result.value().get() = -1;
    EXPECT_EQ(
      result, -1
    ) << "Non empty vector in range [0, Size() - 1] in non const overload must modify the value (Size() >= 1)";
  } else {
    FAIL() << "Non empty vector must not return -> VectorErrors::*";
  }
}

TEST(Vector, GetAllocator) {
  {
    const auto empty_vector = lab::containers::Vector<int>{};
    auto internal_allocator = empty_vector.GetAllocator();
    EXPECT_EQ(internal_allocator, lab::containers::Vector<int>::AllocatorType{});
  }
  {
    const auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    auto internal_allocator = filled_vector.GetAllocator();
    EXPECT_EQ(internal_allocator, lab::containers::Vector<int>::AllocatorType{});
  }
}

TEST(Vector, PushBack) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    empty_vector.PushBack(100);
    EXPECT_EQ(empty_vector.Size(), 1);
    EXPECT_NE(empty_vector.Capacity(), 0);
    EXPECT_EQ(empty_vector[0], 100);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    filled_vector.PushBack(100);
    EXPECT_EQ(previous_size + 1, filled_vector.Size());
    EXPECT_NE(filled_vector.Capacity(), 0);
    EXPECT_EQ(filled_vector[previous_size], 100);
  }
}

TEST(Vector, EmplaceBack) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    empty_vector.EmplaceBack(100);
    EXPECT_EQ(empty_vector.Size(), 1);
    EXPECT_NE(empty_vector.Capacity(), 0);
    EXPECT_EQ(empty_vector[0], 100);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    filled_vector.EmplaceBack(100);
    EXPECT_EQ(previous_size + 1, filled_vector.Size());
    EXPECT_NE(filled_vector.Capacity(), 0);
    EXPECT_EQ(filled_vector[previous_size], 100);
  }
}

TEST(Vector, PopBack) {
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  const auto previous_size = filled_vector.Size();
  const auto previous_capacity = filled_vector.Capacity();
  filled_vector.PopBack();
  EXPECT_EQ(filled_vector.Size(), previous_size - 1);
  EXPECT_EQ(filled_vector.Capacity(), previous_capacity);
}

TEST(Vector, Resize) {
  constexpr auto kResizeElementsLess = kTestNumbers.size() - 1;
  constexpr auto kResizeElementsGreater = kTestNumbers.size() + 1;
  {
    auto empty_vector = lab::containers::Vector<int>{};
    empty_vector.Resize(kTestNumbers.size());
    EXPECT_EQ(empty_vector.Size(), kTestNumbers.size());
    EXPECT_EQ(empty_vector.Capacity(), kTestNumbers.size());
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Resize(kResizeElementsLess);
    EXPECT_EQ(filled_vector.Size(), kResizeElementsLess);
    EXPECT_EQ(filled_vector.Capacity(), previous_capacity);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Resize(kTestNumbers.size());
    EXPECT_EQ(filled_vector.Size(), kTestNumbers.size());
    EXPECT_EQ(filled_vector.Capacity(), previous_capacity);
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Resize(kResizeElementsGreater);
    EXPECT_EQ(filled_vector.Size(), kResizeElementsGreater);
    EXPECT_NE(filled_vector.Capacity(), previous_capacity);
  }
}

TEST(Vector, Reserve) {
  constexpr auto kReservedElements = int{10};
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_size = empty_vector.Size();
    empty_vector.Reserve(kReservedElements);
    EXPECT_EQ(previous_size, empty_vector.Size());
    EXPECT_EQ(kReservedElements, empty_vector.Capacity());
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Reserve(kReservedElements);
    EXPECT_EQ(filled_vector.Size(), previous_size);
    EXPECT_EQ(filled_vector.Capacity(), previous_capacity + kReservedElements);
  }
}

TEST(Vector, ShrinkToFit) {
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_size = empty_vector.Size();
    const auto previous_capacity = empty_vector.Capacity();
    empty_vector.ShrinkToFit();
    EXPECT_EQ(empty_vector.Size(), previous_size);
    EXPECT_EQ(empty_vector.Capacity(), previous_capacity);
  }
  {
    auto filled_vector = lab::containers::Vector{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Reserve(1);
    filled_vector.ShrinkToFit();
    EXPECT_EQ(filled_vector.Size(), previous_size);
    EXPECT_EQ(filled_vector.Capacity(), previous_capacity);
  }
}

TEST(Vector, EmplaceFront) {
  constexpr auto kEmplaceValue = int{-1};
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_capacity = empty_vector.Capacity();
    empty_vector.Emplace(empty_vector.cbegin(), kEmplaceValue);
    EXPECT_EQ(empty_vector.Size(), 1);
    EXPECT_NE(empty_vector.Capacity(), previous_capacity);
    const auto result = empty_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kEmplaceValue);
    } else {
      FAIL();
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Emplace(filled_vector.cbegin(), kEmplaceValue);
    EXPECT_EQ(filled_vector.Size(), previous_size + 1);
    EXPECT_NE(filled_vector.Capacity(), previous_capacity);
    const auto result = filled_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kEmplaceValue);
    } else {
      FAIL();
    }
  }
}

TEST(Vector, EmplaceMiddle) {
  constexpr auto kEmplaceValue = int{50};
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  const auto previous_size = filled_vector.Size();
  const auto previous_capacity = filled_vector.Capacity();
  auto emplace_position = filled_vector.cbegin() + (filled_vector.Size() >> 1);
  filled_vector.Emplace(emplace_position, kEmplaceValue);
  EXPECT_EQ(filled_vector.Size(), previous_size + 1);
  EXPECT_NE(filled_vector.Capacity(), previous_capacity);
  const auto result = filled_vector.At(previous_size >> 1);
  if (result) {
    EXPECT_EQ(result.value().get(), kEmplaceValue);
  } else {
    FAIL();
  }
}

TEST(Vector, EmplaceEnd) {
  constexpr auto kEmplaceValue = int{100};
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_capacity = empty_vector.Capacity();
    empty_vector.Emplace(empty_vector.cend(), kEmplaceValue);
    EXPECT_EQ(empty_vector.Size(), 1);
    EXPECT_NE(empty_vector.Capacity(), previous_capacity);
    const auto result = empty_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kEmplaceValue);
    } else {
      FAIL();
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Emplace(filled_vector.cend(), kEmplaceValue);
    EXPECT_NE(filled_vector.Capacity(), previous_capacity);
    const auto result = filled_vector.Back();
    if (result) {
      EXPECT_EQ(result.value().get(), kEmplaceValue);
    } else {
      FAIL();
    }
  }
}

TEST(Vector, InsertFront) {
  constexpr auto kInsertValue = int{100};
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_capacity = empty_vector.Capacity();
    empty_vector.Insert(empty_vector.cbegin(), kInsertValue);
    EXPECT_EQ(empty_vector.Size(), 1);
    EXPECT_NE(empty_vector.Capacity(), previous_capacity);
    const auto result = empty_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kInsertValue);
    } else {
      FAIL();
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Insert(filled_vector.cbegin(), kInsertValue);
    EXPECT_EQ(filled_vector.Size(), previous_size + 1);
    EXPECT_NE(filled_vector.Capacity(), previous_capacity);
    const auto result = filled_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kInsertValue);
    } else {
      FAIL();
    }
  }
}

TEST(Vector, InsertMiddle) {
  constexpr auto kInsertValue = int{20};
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
  const auto previous_size = filled_vector.Size();
  const auto previous_capacity = filled_vector.Capacity();
  auto insert_position = filled_vector.cbegin() + (filled_vector.Size() >> 1);
  filled_vector.Insert(insert_position, kInsertValue);
  EXPECT_EQ(filled_vector.Size(), previous_size + 1);
  EXPECT_NE(filled_vector.Capacity(), previous_capacity);
  const auto result = filled_vector.At(previous_size >> 1);
  if (result) {
    EXPECT_EQ(result.value().get(), kInsertValue);
  } else {
    FAIL();
  }
}

TEST(Vector, InsertEnd) {
  constexpr auto kInsertValue = -1;
  {
    auto empty_vector = lab::containers::Vector<int>{};
    const auto previous_capacity = empty_vector.Capacity();
    empty_vector.Insert(empty_vector.cend(), kInsertValue);
    EXPECT_EQ(empty_vector.Size(), 1); // NOLINT
    EXPECT_NE(empty_vector.Capacity(), previous_capacity); // NOLINT
    const auto result = empty_vector.Front();
    if (result) {
      EXPECT_EQ(result.value().get(), kInsertValue); // NOLINT
    } else {
      FAIL();
    }
  }
  {
    auto filled_vector = lab::containers::Vector<int>{kTestNumbers};
    const auto previous_size = filled_vector.Size();
    const auto previous_capacity = filled_vector.Capacity();
    filled_vector.Insert(filled_vector.cend(), kInsertValue);
    EXPECT_EQ(filled_vector.Size(), previous_size + 1); // NOLINT
    EXPECT_NE(filled_vector.Capacity(), previous_capacity); // NOLINT
    const auto result = filled_vector.Back();
    if (result) {
      EXPECT_EQ(result.value().get(), kInsertValue); // NOLINT
    } else {
      FAIL();
    }
  }
}

TEST(Vector, Swap) {
  auto empty_vector = lab::containers::Vector<int>{};
  auto filled_vector = lab::containers::Vector<int>{kTestNumbers};

  const auto* const previous_empty_data = empty_vector.Data();
  const auto previous_empty_size = empty_vector.Size();
  const auto previous_empty_capacity = empty_vector.Capacity();

  const auto* const previous_filled_data = filled_vector.Data();
  const auto previous_filled_size = filled_vector.Size();
  const auto previous_filled_capacity = filled_vector.Capacity();

  empty_vector.Swap(filled_vector);

  // NOLINTBEGIN
  EXPECT_EQ(empty_vector.Data(), previous_filled_data);
  EXPECT_EQ(empty_vector.Size(), previous_filled_size);
  EXPECT_EQ(empty_vector.Capacity(), previous_filled_capacity);

  EXPECT_EQ(filled_vector.Data(), previous_empty_data);
  EXPECT_EQ(filled_vector.Size(), previous_empty_size);
  EXPECT_EQ(filled_vector.Capacity(), previous_empty_capacity);
  // NOLINTEND

  if constexpr (std::allocator_traits<
                  typename lab::containers::Vector<int>::AllocatorType>::propagate_on_container_swap::value) {
    EXPECT_EQ(empty_vector.GetAllocator(), filled_vector.GetAllocator()); // NOLINT
  }
}

TEST(Vector, CustomAllocator) {
  constexpr auto kTestValue{100};
  constexpr auto kArraySize{1000};

  auto buffer = std::array<unsigned char, kArraySize>{};
  auto monotonic_resource =
    std::pmr::monotonic_buffer_resource{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
  auto allocator = std::pmr::polymorphic_allocator<int>{&monotonic_resource};
  auto test_vector = lab::containers::Vector<int, decltype(allocator)>{allocator};

  const auto previous_capacity = test_vector.Capacity();

  try {
    test_vector.PushBack(kTestValue);
  } catch (...) {
    FAIL();
  }

  EXPECT_EQ(test_vector.Size(), 1); // NOLINT
  EXPECT_NE(test_vector.Capacity(), previous_capacity); // NOLINT
  const auto result = test_vector.Front();
  if (result) {
    EXPECT_EQ(result.value().get(), kTestValue); // NOLINT
  } else {
    FAIL(); // NOLINT
  }
}
