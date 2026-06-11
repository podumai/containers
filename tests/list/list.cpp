#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <lab/containers/list.hpp>
#include <ranges>

constexpr auto kTestNumbers = {1, 2, 3, 4};

TEST_CASE("List: Default constructor test") {
  auto list = lab::containers::List<int>{};
  REQUIRE(list.Empty());
}

TEST_CASE("List: Iterator constructor test") {
  auto list = lab::containers::List<int>{kTestNumbers.begin(), kTestNumbers.end()};
  CHECK(!list.Empty());
  REQUIRE(std::ranges::equal(list, kTestNumbers));
}

TEST_CASE("List: Copy constructor test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  auto copied_list = list;
  CHECK(!copied_list.Empty());
  REQUIRE(std::ranges::equal(list, copied_list));
}

TEST_CASE("List: Move constructor test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  auto moved_list = std::move(list);
  CHECK(list.Empty());
  REQUIRE(std::ranges::equal(moved_list, kTestNumbers));
}

TEST_CASE("List: Initializer list constructor test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  CHECK(!list.Empty());
  REQUIRE(std::ranges::equal(list, kTestNumbers));
}

TEST_CASE("List: Move assignment operator test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  auto another_list = lab::containers::List<int>{};
  another_list = std::move(list);
  CHECK(list.Empty());
  REQUIRE(std::ranges::equal(another_list, kTestNumbers));
}

TEST_CASE("List: Copy assignment operator test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  auto another_list = lab::containers::List<int>{};
  another_list = list;
  REQUIRE(std::ranges::equal(list, another_list));
}

TEST_CASE("List: Front method test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  constexpr auto kFirstElement = *kTestNumbers.begin();
  CHECK(list.Front() == kFirstElement);
  CHECK(static_cast<const decltype(list)&>(list).Front() == kFirstElement);
}

TEST_CASE("List: Back method test") {
  auto list = lab::containers::List<int>{kTestNumbers};
  constexpr auto kLastElement = [] [[nodiscard]] consteval noexcept {
    auto iterator = kTestNumbers.begin();
    std::advance(iterator, kTestNumbers.size() - 1);
    return *iterator;
  }();
  CHECK(list.Back() == kLastElement);
  CHECK(static_cast<const decltype(list)&>(list).Back() == kLastElement);
}

TEST_CASE("List: PushFront method test") {
  auto list = lab::containers::List<int>{};
  for (const auto value : kTestNumbers) {
    list.PushFront(value);
  }
  CHECK(list.Size() == kTestNumbers.size());
  CHECK(std::ranges::equal(list, std::views::reverse(kTestNumbers)));
}

TEST_CASE("List: EmplaceFront method test") {
  auto list = lab::containers::List<int>{};
  for (const auto value : kTestNumbers) {
    list.EmplaceFront(value);
  }
  CHECK(list.Size() == kTestNumbers.size());
  CHECK(std::ranges::equal(list, std::views::reverse(kTestNumbers)));
}

TEST_CASE("List: PushBack method test") {
  auto list = lab::containers::List<int>{};
  for (const auto value : kTestNumbers) {
    list.PushBack(value);
  }
  CHECK(list.Size() == kTestNumbers.size());
  CHECK(std::ranges::equal(list, kTestNumbers));
}

TEST_CASE("List: EmplaceBack method test") {
  auto list = lab::containers::List<int>{};
  for (const auto value : kTestNumbers) {
    list.EmplaceBack(value);
  }
  CHECK(list.Size() == kTestNumbers.size());
  CHECK(std::ranges::equal(list, kTestNumbers));
}

TEST_CASE("List: Size method test") {
  {
    INFO("Empty list test");
    const auto list = lab::containers::List<int>{};
    CHECK(list.Size() == 0);
  }
  {
    INFO("Filled list test");
    const auto filled_list = lab::containers::List<int>{kTestNumbers};
    CHECK(filled_list.Size() == kTestNumbers.size());
  }
}

TEST_CASE("List: Empty method test") {
  {
    INFO("Empty list test");
    const auto list = lab::containers::List<int>{};
    CHECK(list.Empty());
  }
  {
    INFO("Filled list test");
    const auto filled_list = lab::containers::List<int>{kTestNumbers};
    CHECK(!filled_list.Empty());
  }
}

TEST_CASE("List: Operator subscript test") {
  auto subscript_check = [] [[nodiscard]] (auto&& list) constexpr noexcept -> bool {
    using SizeType = std::remove_cvref_t<decltype(list)>::SizeType;

    const auto list_size = list.Size();
    auto test_number = kTestNumbers.begin();
    for (auto i = SizeType{}; i < list_size; ++i) {
      if (list[i] != *test_number) {
        return false;
      }
      std::advance(test_number, 1);
    }
    return true;
  };
  {
    auto list = lab::containers::List<int>{kTestNumbers};
    CHECK(subscript_check(list));
  }
  {
    const auto list = lab::containers::List<int>{kTestNumbers};
    CHECK(subscript_check(list));
  }
}
