#include <algorithm>
#include <iterator>
#include <lab/containers/forward_list.hpp>
#include <print>
#include <ranges>
#include <vector>

namespace lab::utility {

auto GenerateRange(lab::containers::ForwardList<int>& list) -> void {
  auto current_position{list.cbegin()};
  for (const int value : std::views::iota(0, 10)) {
    const auto inserted_position{list.InsertAfter(current_position, value)};
    current_position = inserted_position;
  }
}

auto EraseElements(lab::containers::ForwardList<int>& list, std::initializer_list<int> indexes) -> void {
  using ConstIterator = typename lab::containers::ForwardList<int>::ConstIterator;
  std::vector<ConstIterator> erase_positions(indexes.size());
  for (auto&& [index, erase_position] : std::views::zip(indexes, erase_positions)) {
    erase_position = list.cbegin();
    std::advance(erase_position, index);
  }
  for (const auto& erase_position : erase_positions) {
    list.EraseAfter(erase_position);
  }
}

auto PrintRange(const lab::containers::ForwardList<int>& list) -> void {
  auto first{list.cbegin()};
  auto last{list.cend()};
  if (first == last) {
    std::println("<Range is empty>");
    return;
  }
  const auto range_size{std::distance(first, last)};
  auto last_valid_position{first};
  std::advance(last_valid_position, range_size - 1);
  while (first != last_valid_position) {
    std::print("{},", *first);
    ++first;
  }
  std::println("{}", *first);
}

auto InsertFirstElement(lab::containers::ForwardList<int>& list, const int value) -> void { list.PushFront(value); }

auto InsertMiddleElement(lab::containers::ForwardList<int>& list, const int value) -> void {
  const auto first{list.cbegin()};
  const auto last{list.cend()};
  auto middle_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(middle_position, (list_size >> 1) - 1);
  list.InsertAfter(middle_position, value);
}

auto InsertLastElement(lab::containers::ForwardList<int>& list, const int value) -> void {
  const auto first{list.cbegin()};
  const auto last{list.cend()};
  auto last_valid_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(last_valid_position, list_size - 1);
  list.InsertAfter(last_valid_position, value);
}

auto PrintRangeSize(const lab::containers::ForwardList<int>& list) -> void {
  std::println("{}", std::ranges::distance(list.cbegin(), list.cend()));
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
