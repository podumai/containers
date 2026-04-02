#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <initializer_list>
#include <lab/containers/forward_list.hpp>
#include <memory>
#include <memory_resource>
#include <ranges>

constexpr auto kTestNumbers = {1, 2, 3, 4};

TEST_CASE("ForwardList: Default constructor test") {
  auto list = lab::containers::ForwardList<int>{};
  REQUIRE(list.Empty());
}

TEST_CASE("ForwardList: Iterator constructor test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers.begin(), kTestNumbers.end()};
  REQUIRE(std::ranges::equal(kTestNumbers, list));
}

TEST_CASE("ForwardList: Copy constructor test") {
  {
    INFO("Empty forward list check");
    auto empty_list = lab::containers::ForwardList<int>{};
    auto copied_list = empty_list;
    CHECK(std::ranges::equal(empty_list, copied_list));
    CHECK(empty_list.GetAllocator() == copied_list.GetAllocator());
  }
  {
    INFO("Filled forward list check");
    auto list = lab::containers::ForwardList<int>{kTestNumbers};
    auto copied_list = list;
    CHECK(std::ranges::equal(list, copied_list));
    CHECK(list.GetAllocator() == copied_list.GetAllocator());
  }
}

TEST_CASE("ForwardList: Move constructor test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers};
  auto moved_list = std::move(list);
  REQUIRE(list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, moved_list));
  REQUIRE(list.GetAllocator() == moved_list.GetAllocator());
}

TEST_CASE("ForwardList: Initializer list constructor test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers};
  REQUIRE(!list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, list));
}

TEST_CASE("ForwardList: Move assignment operator test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers};
  auto another_list = lab::containers::ForwardList<int>{};
  another_list = std::move(list);
  REQUIRE(list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, another_list));
}

TEST_CASE("ForwardList: Copy assignment operator test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers};
  auto another_list = lab::containers::ForwardList<int>{};
  another_list = list;
  CHECK(std::ranges::equal(list, another_list));
  CHECK(another_list.GetAllocator() == list.GetAllocator());
}

TEST_CASE("ForwardList: Front method test") {
  {
    auto empty_list = lab::containers::ForwardList<int>{};
    auto result = empty_list.Front();
    CHECK(!result.has_value());
  }
  {
    auto list = lab::containers::ForwardList<int>{kTestNumbers};
    auto result = list.Front();
    REQUIRE(result.has_value());
    CHECK(result == 1);
  }
}

TEST_CASE("ForwardList: PushFront method test") {
  auto list = lab::containers::ForwardList<int>{};
  for (auto i = int{}; i < 5; ++i) {
    list.PushFront(i);
  }
  REQUIRE(std::ranges::equal(list, std::initializer_list{4, 3, 2, 1, 0}));
}

TEST_CASE("ForwardList: EmplaceFront method test") {
  auto list = lab::containers::ForwardList<int>{};
  for (auto i = int{}; i < 5; ++i) {
    list.EmplaceFront(i);
  }
  REQUIRE(std::ranges::equal(list, std::initializer_list{4, 3, 2, 1, 0}));
}

TEST_CASE("ForwardList: PopFront method test") {
  auto list = lab::containers::ForwardList<int>{kTestNumbers};
  for (auto begin = kTestNumbers.begin(); begin != kTestNumbers.end(); ++begin) {
    CHECK(list.Front() == *begin);
    list.PopFront();
  }
  REQUIRE(list.Empty());
}

TEST_CASE("ForwardList: Iterator test") {
  auto list = lab::containers::ForwardList<int>{};
  INFO("Empty forward list check");
  REQUIRE(list.begin() == list.end());
  REQUIRE(list.cbegin() == list.cend());
  list.PushFront(120);
  INFO("One element forward list check");
  REQUIRE(list.begin() != list.end());
  REQUIRE(list.cbegin() != list.cend());
  list.PopFront();
  INFO("Empty forward list check after PopFront");
  REQUIRE(list.begin() == list.end());
  REQUIRE(list.cbegin() == list.cend());
}

TEST_CASE("ForwardList: Clear method test") {
  {
    INFO("Empty forward list check");
    auto empty_list = lab::containers::ForwardList<int>{};
    empty_list.Clear();
    CHECK(empty_list.Empty());
  }
  {
    INFO("Filled forward list check");
    auto filled_list = lab::containers::ForwardList<int>{kTestNumbers};
    filled_list.Clear();
    CHECK(filled_list.Empty());
  }
}

TEST_CASE("ForwardList: InsertAfter method test") {
  auto list = lab::containers::ForwardList<int>{};
  auto iter = list.cbegin();
  for (auto value : kTestNumbers) {
    auto inserted_value_position = list.InsertAfter(iter, value);
    REQUIRE(*inserted_value_position == value);
    iter = inserted_value_position;
  }
}

TEST_CASE("ForwardList: Erase method test") {
  auto list = lab::containers::ForwardList{1, 2, 3, 4};
  auto erase_position = std::ranges::find(list, 2);
  list.EraseAfter(erase_position);
  REQUIRE(std::ranges::equal(list, std::initializer_list{1, 2, 4}));
  list.EraseAfter(list.BeforeBegin());
  REQUIRE(std::ranges::equal(list, std::initializer_list{2, 4}));
  erase_position = std::ranges::find(list, 2);
  list.EraseAfter(erase_position);
  REQUIRE(std::ranges::equal(list, std::initializer_list{2}));
  for (const auto value : list) {
    INFO(value);
  }
}

TEST_CASE("ForwardList: Swap method test") {
  auto empty_list = lab::containers::ForwardList<int>{};
  auto filled_list = lab::containers::ForwardList<int>{kTestNumbers};
  empty_list.Swap(filled_list);
  REQUIRE(!empty_list.Empty());
  REQUIRE(filled_list.Empty());
  REQUIRE(empty_list.GetAllocator() == filled_list.GetAllocator());
}

TEST_CASE("ForwardList: Swap function test") {
  auto empty_list = lab::containers::ForwardList<int>{};
  auto filled_list = lab::containers::ForwardList<int>{kTestNumbers};
  using std::swap;
  swap(empty_list, filled_list);
  REQUIRE(!empty_list.Empty());
  REQUIRE(filled_list.Empty());
  REQUIRE(empty_list.GetAllocator() == filled_list.GetAllocator());
}

TEST_CASE("ForwardList: Constexpr test") {
  constexpr auto kExpectedFrontValue = 120;
  constexpr auto kFrontValue = [kExpectedFrontValue] consteval -> int {
    auto list = lab::containers::ForwardList<int>{};
    list.PushFront(kExpectedFrontValue);
    auto result = list.Front();
    return result.value();
  }();
  REQUIRE(kFrontValue == kExpectedFrontValue);
}

TEST_CASE("ForwardList: Empty method test") {
  {
    INFO("Empty forward list test");
    auto empty_list = lab::containers::ForwardList<int>{};
    CHECK(empty_list.Empty());
  }
  {
    INFO("Filled forward list test");
    auto filled_list = lab::containers::ForwardList<int>{kTestNumbers};
    CHECK(!filled_list.Empty());
  }
}

TEST_CASE("ForwardList: Size method test") {
  {
    INFO("Empty forward list test");
    auto empty_list = lab::containers::ForwardList<int>{};
    CHECK(empty_list.Size() == 0);
  }
  {
    INFO("Filled forward list test");
    auto filled_list = lab::containers::ForwardList<int>{kTestNumbers};
    CHECK(filled_list.Size() == kTestNumbers.size());
  }
}
