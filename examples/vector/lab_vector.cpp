#include <algorithm>
#include <iterator>
#include <lab/containers/vector.hpp>
#include <print>
#include <ranges>

namespace lab::utility {

auto GenerateRange(lab::containers::Vector<int>& vector) -> void {
  for (const int value : std::views::iota(0, 10)) {
    vector.Insert(vector.cend(), value);
  }
}

auto EraseElements(lab::containers::Vector<int>& vector, std::initializer_list<int> elements) -> void {
  for (const int value : elements) {
    const auto position{std::ranges::find(vector.cbegin(), vector.cend(), value)};
    if (position == vector.cend()) {
      std::println(stderr, "Missing value {} in ForwardList instance", value);
      continue;
    }
    vector.Erase(position);
  }
}

auto PrintRange(const lab::containers::Vector<int>& vector) -> void {
  auto first{vector.cbegin()};
  const auto last{vector.cend()};
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

auto InsertFirstElement(lab::containers::Vector<int>& vector, const int value) -> void {
  vector.Insert(vector.cbegin(), value);
}

auto InsertMiddleElement(lab::containers::Vector<int>& vector, const int value) -> void {
  auto first{vector.cbegin()};
  const auto last{vector.cend()};
  auto middle_position{first};
  const auto list_size{std::ranges::distance(first, last)};
  std::ranges::advance(middle_position, list_size >> 1);
  vector.Insert(middle_position, value);
}

auto InsertLastElement(lab::containers::Vector<int>& vector, const int value) -> void {
  vector.Insert(vector.cend(), value);
}

auto PrintRangeSize(const lab::containers::Vector<int>& vector) -> void {
  std::println("{}", std::ranges::distance(vector.cbegin(), vector.cend()));
}

}  // namespace lab::utility

auto main() -> int {
  lab::containers::Vector<int> vector{};
  lab::utility::GenerateRange(vector);
  lab::utility::PrintRange(vector);
  lab::utility::PrintRangeSize(vector);
  lab::utility::EraseElements(vector, {2, 4, 6});
  lab::utility::PrintRange(vector);
  lab::utility::InsertFirstElement(vector, 10);
  lab::utility::PrintRange(vector);
  lab::utility::InsertMiddleElement(vector, 20);
  lab::utility::PrintRange(vector);
  lab::utility::InsertLastElement(vector, 30);
  lab::utility::PrintRange(vector);
  return 0;
}
