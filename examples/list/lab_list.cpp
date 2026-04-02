#include <algorithm>
#include <iterator>
#include <lab/containers/list.hpp>
#include <print>
#include <ranges>

namespace lab::utility {

auto GenerateRange(lab::containers::List<int>& list) -> void {
  for (const int value : std::views::iota(0, 10)) {
    list.PushBack(value);
  }
}

auto EraseElements(lab::containers::List<int>& list, std::initializer_list<int> elements) -> void {
  for (const int value : elements) {
    const auto position{std::ranges::find(list.cbegin(), list.cend(), value)};
    if (position == list.cend()) {
      std::println(stderr, "Missing value {} in ForwardList instance", value);
      continue;
    }
    list.Erase(position);
  }
}

auto PrintRange(const lab::containers::List<int>& list) -> void {
  auto first{list.cbegin()};
  const auto last{list.cend()};
  if (first == last) {
    std::println("<Range is empty>");
    return;
  }
  const auto range_size{std::ranges::distance(first, last)};
  auto last_valid_position{first};
  std::ranges::advance(last_valid_position, range_size - 1);
  while (first != last_valid_position) {
    std::print("{},", *first);
    ++first;
  }
  std::println("{}", *first);
}

auto InsertFirstElement(lab::containers::List<int>& list, const int value) -> void { list.PushFront(value); }

auto InsertMiddleElement(lab::containers::List<int>& list, const int value) -> void {
  const auto first{list.cbegin()};
  const auto last{list.cend()};
  auto middle_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(middle_position, list_size >> 1);
  list.Insert(middle_position, value);
}

auto InsertLastElement(lab::containers::List<int>& list, const int value) -> void { list.PushBack(value); }

auto PrintRangeSize(const lab::containers::List<int>& list) -> void {
  std::println("{}", std::ranges::distance(list.cbegin(), list.cend()));
}

}  // namespace lab::utility

auto main() -> int {
  try {
    lab::containers::List<int> list{};
    lab::utility::GenerateRange(list);
    lab::utility::PrintRange(list);
    lab::utility::PrintRangeSize(list);
    lab::utility::EraseElements(list, {2, 4, 6});
    lab::utility::PrintRange(list);
    lab::utility::InsertFirstElement(list, 10);
    lab::utility::PrintRange(list);
    lab::utility::InsertMiddleElement(list, 20);
    lab::utility::PrintRange(list);
    lab::utility::InsertLastElement(list, 30);
    lab::utility::PrintRange(list);
  } catch (std::exception const& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
  return 0;
}
