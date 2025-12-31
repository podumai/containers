#include <algorithm>
#include <iterator>
#include <lab/containers/forward_list.hpp>
#include <print>
#include <ranges>
#include <vector>

namespace lab::utility {

auto GenerateRange(lab::containers::ForwardList<int>& list) -> void {
  auto current_position = std::ranges::cbegin(list);
  for (const int value : std::views::iota(0, 10)) {
    const auto inserted_position{list.InsertAfter(current_position, value)};
    current_position = inserted_position;
  }
}

auto EraseElements(lab::containers::ForwardList<int>& list, std::initializer_list<int> indexes) -> void {
  using ConstIterator = typename lab::containers::ForwardList<int>::ConstIterator;
  std::vector<ConstIterator> erase_positions(indexes.size());
  for (auto&& [index, erase_position] : std::views::zip(indexes, erase_positions)) {
    erase_position = std::ranges::cbegin(list);
    std::ranges::advance(erase_position, index);
  }
  for (const ConstIterator& erase_position : erase_positions) {
    list.EraseAfter(erase_position);
  }
}

auto PrintRange(std::ranges::range auto&& range) -> void {
  auto first{std::ranges::cbegin(std::forward<decltype(range)>(range))};
  auto last{std::ranges::cend(std::forward<decltype(range)>(range))};
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

auto InsertFirstElement(lab::containers::ForwardList<int>& list, const int value) -> void { list.PushFront(value); }

auto InsertMiddleElement(lab::containers::ForwardList<int>& list, const int value) -> void {
  const auto first{std::ranges::cbegin(list)};
  const auto last{std::ranges::cend(list)};
  auto middle_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(middle_position, (list_size >> 1) - 1);
  list.InsertAfter(middle_position, value);
}

auto InsertLastElement(lab::containers::ForwardList<int>& list, const int value) -> void {
  const auto first{std::ranges::cbegin(list)};
  const auto last{std::ranges::cend(list)};
  auto last_valid_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(last_valid_position, list_size - 1);
  list.InsertAfter(last_valid_position, value);
}

auto PrintRangeSize(std::ranges::range auto&& range) -> void {
  const auto first{std::ranges::cbegin(std::forward<decltype(range)>(range))};
  const auto last{std::ranges::cend(std::forward<decltype(range)>(range))};
  std::println("{}", std::ranges::distance(first, last));
}

}  // namespace lab::utility

auto main() -> int {
  lab::containers::ForwardList<int> list{};
  lab::utility::GenerateRange(list);
  lab::utility::PrintRange(list);
  lab::utility::PrintRangeSize(list);
  lab::utility::EraseElements(list, {1, 3, 5});
  lab::utility::PrintRange(list);
  lab::utility::InsertFirstElement(list, 10);
  lab::utility::PrintRange(list);
  lab::utility::InsertMiddleElement(list, 20);
  lab::utility::PrintRange(list);
  lab::utility::InsertLastElement(list, 30);
  lab::utility::PrintRange(list);
  return 0;
}
