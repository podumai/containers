import lab_forward_list;

#include <iterator>
#include <print>
#include <ranges>

namespace lab::utility {

auto GenerateRange(lab::containers::ForwardList<int>& list) -> void {
  auto current_position{std::ranges::cbegin(list)};
  for (const int value : std::views::iota(0, 10)) {
    auto inserted_position{list.InsertAfter(current_position, value)};
    current_position = inserted_position;
  }
}

auto EraseElements(lab::containers::ForwardList<int>& list, std::initializer_list<int> elements) -> void {
  for (const int value : elements) {
    auto position{std::ranges::find(list, value)};
    if (position == std::ranges::cend(list)) {
      std::println(stderr, "Missing value {} in ForwardList instance", value);
      continue;
    }
    list.Erase(position);
  }
}

auto PrintRange(std::ranges::range auto&& range) -> void {
  auto first{std::ranges::cbegin(std::forward<decltype(range)>(range))};
  auto last{std::ranges::cend(std::forward<decltype(range)>(range))};
  if (first == last) {
    std::println("<Range is empty>");
    return;
  }
  auto range_size{std::ranges::distance(first, last)};
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
  auto first{std::ranges::cbegin(list)};
  auto last{std::ranges::cend(list)};
  auto middle_position{first};
  auto list_size{std::ranges::distance(first, last)};
  std::advance(middle_position, (list_size >> 1) - 1);
  list.InsertAfter(middle_position, value);
}

auto InsertLastElement(lab::containers::ForwardList<int>& list, const int value) -> void {
  auto first{std::ranges::cbegin(list)};
  auto last{std::ranges::cend(list)};
  auto last_valid_position{first};
  auto list_size{std::ranges::distance(first, last)};
  std::advance(last_valid_position, list_size - 1);
  list.InsertAfter(last_valid_position, value);
}

auto PrintRangeSize(std::ranges::range auto&& range) -> void {
  auto first{std::ranges::cbegin(std::forward<decltype(range)>(range))};
  auto last{std::ranges::cend(std::forward<decltype(range)>(range))};
  std::println("{}", std::ranges::distance(first, last));
}

}  // namespace lab::utility

auto main() -> int {
  lab::containers::ForwardList<int> list;
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
  return 0;
}
