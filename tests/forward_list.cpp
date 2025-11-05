import lab_forward_list;

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <initializer_list>
#include <memory>
#include <memory_resource>
#include <ranges>

constexpr auto kTestNumbers = {1, 2, 3, 4};

TEST_CASE("Default constructor test") {
  lab::containers::ForwardList<int> list;
  REQUIRE(list.Empty());
}

TEST_CASE("Iterator constructor test") {
  lab::containers::ForwardList<int> list{kTestNumbers.begin(), kTestNumbers.end()};
  REQUIRE(!list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, list));
}

TEST_CASE("Copy constructor test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  lab::containers::ForwardList<int> copied_list{list};
  REQUIRE(!copied_list.Empty());
  REQUIRE(std::ranges::equal(list, copied_list));
  REQUIRE(list.GetAllocator() == copied_list.GetAllocator());
}

TEST_CASE("Move constructor test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  lab::containers::ForwardList<int> moved_list{std::move(list)};
  REQUIRE(list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, moved_list));
  REQUIRE(list.GetAllocator() == moved_list.GetAllocator());
}

TEST_CASE("Initializer list constructor test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  REQUIRE(!list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, list));
}

TEST_CASE("Move assignment operator test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  lab::containers::ForwardList<int> another_list;
  another_list = std::move(list);
  REQUIRE(list.Empty());
  REQUIRE(std::ranges::equal(kTestNumbers, another_list));
}

TEST_CASE("Copy assignment operator test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  lab::containers::ForwardList<int> another_list;
  another_list = list;
  REQUIRE(std::ranges::equal(list, another_list));
}

TEST_CASE("Front method test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  REQUIRE(list.Front() == 1);
}

TEST_CASE("PushFront method test") {
  lab::containers::ForwardList<int> list;
  for (int i{}; i < 5; ++i) {
    list.PushFront(i);
  }
  REQUIRE(std::ranges::equal(list, std::initializer_list{4, 3, 2, 1, 0}));
}

TEST_CASE("EmplaceFront method test") {
  lab::containers::ForwardList<int> list;
  for (int i{}; i < 5; ++i) {
    list.EmplaceFront(i);
  }
  REQUIRE(std::ranges::equal(list, std::initializer_list{4, 3, 2, 1, 0}));
}

TEST_CASE("PopFront method test") {
  lab::containers::ForwardList<int> list{kTestNumbers};
  for (auto begin{kTestNumbers.begin() + 1}; begin != kTestNumbers.end(); ++begin) {
    list.PopFront();
    REQUIRE(list.Front() == *begin);
  }
  while (!list.Empty()) {
    list.PopFront();
  }
  REQUIRE(list.Empty());
}

TEST_CASE("Iterator test") {
  lab::containers::ForwardList<int> list;
  REQUIRE(list.begin() == list.end());
  REQUIRE(list.cbegin() == list.cend());
  list.PushFront(120);
  REQUIRE(list.begin() != list.end());
  REQUIRE(list.cbegin() != list.cend());
  list.PopFront();
  REQUIRE(list.begin() == list.end());
  REQUIRE(list.cbegin() == list.cend());
}

TEST_CASE("Clear method test") {
  lab::containers::ForwardList<int> empty_list;
  empty_list.Clear();
  REQUIRE(empty_list.Empty());
  lab::containers::ForwardList<int> filled_list{kTestNumbers};
  filled_list.Clear();
  REQUIRE(filled_list.Empty());
}

TEST_CASE("InsertAfter method test") {
  lab::containers::ForwardList<int> list;
  auto iter{list.cbegin()};
  for (int value : kTestNumbers) {
    auto inserted_value_position{list.InsertAfter(iter, value)};
    REQUIRE(*inserted_value_position == value);
    iter = inserted_value_position;
  }
}

TEST_CASE("Erase method test") {
  lab::containers::ForwardList<int> list{1, 2, 3, 4};
  auto erase_position{std::ranges::find(list, 2)};
  list.Erase(erase_position);
  REQUIRE(std::ranges::equal(list, std::initializer_list{1, 3, 4}));
  list.Erase(list.cbegin());
  REQUIRE(std::ranges::equal(list, std::initializer_list{3, 4}));
  erase_position = std::ranges::find(list, 4);
  list.Erase(erase_position);
  REQUIRE(std::ranges::equal(list, std::initializer_list{3}));
}

TEST_CASE("Swap method test") {
  lab::containers::ForwardList<int> empty_list;
  lab::containers::ForwardList<int> filled_list{kTestNumbers};
  empty_list.Swap(filled_list);
  REQUIRE(!empty_list.Empty());
  REQUIRE(filled_list.Empty());
  REQUIRE(empty_list.GetAllocator() == filled_list.GetAllocator());
}

TEST_CASE("Constexpr test") {
  constexpr int front_value{[] consteval -> int {
    lab::containers::ForwardList<int> list;
    list.PushFront(120);
    int result{list.Front()};
    return result;
  }()};
  REQUIRE(front_value == 120);
}
