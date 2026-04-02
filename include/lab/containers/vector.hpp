#pragma once

#include <algorithm>
#include <compare>
#include <concepts>
#include <expected>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <lab/containers/assert.hpp>
#include <memory>
#include <type_traits>
#include <utility>

namespace lab::containers {

namespace details {

template<typename T>
concept IsValidVectorType = (std::movable<T> || std::copyable<T>) && std::is_nothrow_destructible_v<T>;

}  // namespace details

template<bool IsConst, typename VectorType>
class VectorIteratorBase {
  template<details::IsValidVectorType, typename>
  friend class Vector;

  friend class VectorIteratorBase<!IsConst, VectorType>;

  using InternalPointer = VectorType::ValueType*;

 public:
  // NOLINTBEGIN
  using value_type = VectorType::value_type;
  using ValueType = VectorType::ValueType;
  using reference = std::conditional_t<IsConst, typename VectorType::const_reference, typename VectorType::reference>;
  using Reference = reference;
  using pointer = std::conditional_t<IsConst, typename VectorType::const_pointer, typename VectorType::pointer>;
  using Pointer = pointer;
  using difference_type = VectorType::difference_type;
  using DifferenceType = VectorType::DifferenceType;
  using iterator_category = std::contiguous_iterator_tag;
  using IteratorCategory = iterator_category;
  // NOLINTEND

  constexpr VectorIteratorBase() noexcept = default;

 protected:
  constexpr VectorIteratorBase(InternalPointer current) noexcept : current_{current} { }

 public:
  constexpr ~VectorIteratorBase() = default;

  constexpr auto operator->() const noexcept -> Pointer { return current_; }

  constexpr auto operator*() const noexcept -> Reference {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator*(): nullptr dereference");
    return *current_;
  }

  constexpr auto operator++() noexcept -> VectorIteratorBase& {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator++(): nullptr dereference");
    ++current_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> VectorIteratorBase {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator++(int): nullptr dereference");
    VectorIteratorBase previous{*this};
    ++current_;
    return previous;
  }

  constexpr auto operator--() noexcept -> VectorIteratorBase& {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator--(): nullptr dereference");
    --current_;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> VectorIteratorBase {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator--(int): nullptr dereference)");
    VectorIteratorBase previous{*this};
    --current_;
    return previous;
  }

  constexpr auto operator+=(const DifferenceType offset) noexcept -> VectorIteratorBase& {
    utility::LabContainersAssert(current_, "Undefined behaviour: binary operator+= called on invalid instance");
    current_ += offset;
    return *this;
  }

  [[nodiscard]] friend constexpr auto operator+(const VectorIteratorBase iterator, const DifferenceType offset) noexcept
    -> VectorIteratorBase {
   utility::LabContainersAssert(iterator.current_, "Undefined behaviour: binary operator+ called on invalid instance");
    return iterator.current_ + offset;
  }

  [[nodiscard]] friend constexpr auto operator+(const DifferenceType offset, const VectorIteratorBase iterator) noexcept
    -> VectorIteratorBase {
    utility::LabContainersAssert(iterator.current_, "Undefined behaviour: binary operator+ called on invalid instance");
    return iterator.current_ + offset;
  }

  friend constexpr auto operator-=(VectorIteratorBase& iterator, const DifferenceType offset) noexcept
    -> VectorIteratorBase& {
    utility::LabContainersAssert(iterator.current_, "Undefined behaviour: binary operator-= called on invalid instance");
    iterator.current_ -= offset;
    return iterator;
  }

  [[nodiscard]] friend constexpr auto operator-(const VectorIteratorBase iterator, const DifferenceType offset) noexcept
    -> VectorIteratorBase {
    utility::LabContainersAssert(iterator.current_, "Undefined behaviour: binary operator- called on invalid instance");
    return iterator.current_ - offset;
  }

  [[nodiscard]] friend constexpr auto operator-(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> DifferenceType {
    return lhs.current_ && rhs.current_ ? lhs.current_ - rhs.current_ : 0;
  }

  [[nodiscard]] constexpr auto operator[](const DifferenceType index) const noexcept -> Reference {
    utility::LabContainersAssert(current_, "Undefined behaviour: VectorIteratorBase::operator[] const: nullptr dereference");
    return current_[index];
  }

  [[nodiscard]] friend constexpr auto operator==(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ == rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator!=(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ != rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator<(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ < rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator<=(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ <= rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator>(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ > rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator>=(const VectorIteratorBase lhs, const VectorIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ >= rhs.current_;
  }

  [[nodiscard]] constexpr operator VectorIteratorBase<!IsConst, VectorType>() const noexcept { return current_; }

 private:
  InternalPointer current_{nullptr};
};

enum struct VectorErrors : unsigned char { kUnknown, kEmpty, kOutOfRange };

template<details::IsValidVectorType T, typename Allocator = std::allocator<T>>
class [[nodiscard]] Vector {
  using AllocatorTraits = std::allocator_traits<Allocator>;
  friend class VectorIteratorBase<false, Vector>;
  friend class VectorIteratorBase<true, Vector>;

 public:
  // NOLINTBEGIN
  using value_type = T;
  using ValueType = value_type;
  using reference = T&;
  using Reference = reference;
  using const_reference = const value_type&;
  using ConstReference = const_reference;
  using pointer = AllocatorTraits::pointer;
  using Pointer = pointer;
  using const_pointer = AllocatorTraits::const_pointer;
  using ConstPointer = const_pointer;
  using size_type = AllocatorTraits::size_type;
  using SizeType = size_type;
  using difference_type = AllocatorTraits::difference_type;
  using DifferenceType = difference_type;
  using iterator = VectorIteratorBase<false, Vector>;
  using Iterator = iterator;
  using const_iterator = VectorIteratorBase<true, Vector>;
  using ConstIterator = const_iterator;
  using reverse_iterator = std::reverse_iterator<Iterator>;
  using ReverseIterator = reverse_iterator;
  using const_reverse_iterator = std::reverse_iterator<ConstIterator>;
  using ConstReverseIterator = const_reverse_iterator;
  using allocator_type = Allocator;
  using AllocatorType = allocator_type;
  // NOLINTEND

  constexpr Vector() noexcept = default;

  constexpr explicit Vector(const AllocatorType& allocator) noexcept : allocator_{allocator} { }

  constexpr Vector(const Vector& other)
    : Vector{other.cbegin(), other.cend(), AllocatorTraits::select_on_container_copy_construction(other.allocator_)} { }

  constexpr Vector(Vector&& other) noexcept
    : first_{std::exchange(other.first_, nullptr)}
    , current_{std::exchange(other.current_, nullptr)}
    , last_{std::exchange(other.last_, nullptr)}
    , allocator_{std::move(other.allocator_)} { }

  template<std::input_iterator InputIterator, std::sentinel_for<InputIterator> Sentinel>
  constexpr Vector(InputIterator first, Sentinel last, const AllocatorType& allocator = AllocatorType{})
    : Vector{allocator} {
    if (first == last) [[unlikely]] {
      return;
    }
    const SizeType size{static_cast<SizeType>(std::ranges::distance(first, last))};
    first_ = AllocatorTraits::allocate(allocator_, size);
    current_ = first_;
    last_ = first_ + size;
    try {
      while (first != last) {
        AllocatorTraits::construct(allocator_, current_, *first);
        ++first;
        ++current_;
      }
    } catch (...) {
      for (Pointer traverser{first_}; traverser != current_; ++traverser) {
        AllocatorTraits::destroy(allocator_, traverser);
      }
      AllocatorTraits::deallocate(allocator_, first_, size);
      throw;
    }
  }

  constexpr Vector(std::initializer_list<ValueType> ilist, const AllocatorType& allocator = AllocatorType{})
    : Vector{ilist.begin(), ilist.end(), allocator} { }

  constexpr Vector(const SizeType count, const ValueType& value, const AllocatorType& allocator = AllocatorType{})
    : Vector{allocator} {
    if (!count) [[unlikely]] {
      return;
    }
    first_ = AllocatorTraits::allocate(allocator_, count);
    current_ = first_;
    last_ = first_ + count;
    try {
      while (current_ != last_) {
        AllocatorTraits::construct(allocator_, current_, value);
        ++current_;
      }
    } catch (...) {
      for (auto traverser = first_; traverser != current_; ++traverser) {
        AllocatorTraits::destroy(allocator_, traverser);
      }
      AllocatorTraits::deallocate(allocator_, first_, count);
      throw;
    }
  }

  constexpr Vector(SizeType count, const AllocatorType& allocator = AllocatorType{})
    : Vector{count, ValueType{}, allocator} { }

  constexpr ~Vector() { Clear(); }

  constexpr auto operator=(const Vector& other) -> Vector& {
    LAB_CONTAINERS_ASSERT(this != &other, "Undefined behaviour: Vector::operator=(const Vector&): self copy detected");
    Vector{other}.Swap(*this);
    return *this;
  }

  constexpr auto operator=(Vector&& other) noexcept -> Vector& {
    LAB_CONTAINERS_ASSERT(this != &other, "Undefined behaviour: Vector::operator=(Vector&&): self move detected");
    Clear();
    std::swap(first_, other.first_);
    std::swap(current_, other.current_);
    std::swap(last_, other.last_);
    if constexpr (AllocatorTraits::propagate_on_container_move_assigment::value) {
      allocator_ = std::move(other.allocator_);
    }
    return *this;
  }

  [[nodiscard]] friend constexpr auto operator==(const Vector& lhs, const Vector& rhs) noexcept -> bool {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
  }

  [[nodiscard]] friend constexpr auto operator<=>(const Vector& lhs, const Vector& rhs) noexcept {
    return std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
  }

  [[nodiscard]] constexpr auto begin() noexcept -> Iterator { return first_; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto end() noexcept -> Iterator { return current_; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto cbegin() const noexcept -> ConstIterator { return first_; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto cend() const noexcept -> ConstIterator { return current_; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto rbegin() noexcept -> ReverseIterator { return ReverseIterator{current_}; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto rend() noexcept -> ReverseIterator { return ReverseIterator{first_}; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto crbegin() const noexcept -> ConstReverseIterator { // NOLINT: std compatible interface
    return ConstReverseIterator{current_};
  }

  [[nodiscard]] constexpr auto crend() const noexcept -> ConstReverseIterator { return ConstReverseIterator{first_}; } // NOLINT: std compatible interface

  [[nodiscard]] constexpr auto Data() const noexcept -> ConstPointer { return first_; }

  [[nodiscard]] constexpr auto Size() const noexcept -> SizeType { return first_ ? current_ - first_ : 0; }

  [[nodiscard]] constexpr auto Capacity() const noexcept -> SizeType { return first_ ? last_ - first_ : 0; }

  [[nodiscard]] constexpr auto MaxSize() const noexcept -> SizeType { return AllocatorTraits::max_size(allocator_); }

  [[nodiscard]] constexpr auto Empty() const noexcept -> bool { return first_ == current_; }

  [[nodiscard]] constexpr auto At(const SizeType index) noexcept
    -> std::expected<std::reference_wrapper<ValueType>, VectorErrors> {
    Pointer indexed_position{first_ ? first_ + index : current_};
    if (indexed_position < current_) [[likely]] {
      return *indexed_position;
    }
    return std::unexpected{VectorErrors::kOutOfRange};
  }

  [[nodiscard]] constexpr auto At(const SizeType index) const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, VectorErrors> {
    const Pointer indexed_position{first_ ? first_ + index : current_};
    if (indexed_position < current_) [[likely]] {
      return *indexed_position;
    }
    return std::unexpected{VectorErrors::kOutOfRange};
  }

  [[nodiscard]] constexpr auto operator[](const SizeType index) noexcept -> Reference {
    utility::LabContainersAssert(!Empty(), "Undefined behaviour: Vector::operator[]: called on empty vector");
    return first_[index];
  }

  [[nodiscard]] constexpr auto operator[](const SizeType index) const noexcept -> ConstReference {
    utility::LabContainersAssert(!Empty(), "Undefined behaviour: Vector::operator[] const: called on empty vector");
    return first_[index];
  }

  [[nodiscard]] constexpr auto Front() noexcept -> std::expected<std::reference_wrapper<ValueType>, VectorErrors> {
    if (first_ != current_) [[likely]] {
      return *first_;
    }
    return std::unexpected{VectorErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto Front() const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, VectorErrors> {
    if (first_ != current_) [[likely]] {
      return *first_;
    }
    return std::unexpected{VectorErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto Back() noexcept -> std::expected<std::reference_wrapper<ValueType>, VectorErrors> {
    if (first_ != current_) [[likely]] {
      return *(current_ - 1);
    }
    return std::unexpected{VectorErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto Back() const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, VectorErrors> {
    if (first_ != current_) [[likely]] {
      return *(current_ - 1);
    }
    return std::unexpected{VectorErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto GetAllocator() const noexcept -> AllocatorType { return allocator_; }

  constexpr auto PushBack(const ValueType& value) -> void { (void) EmplaceBack(value); }

  constexpr auto PushBack(ValueType&& value) -> void { (void) EmplaceBack(std::move(value)); }

 private:
  constexpr auto InternalResize(const SizeType new_size) -> void {
    Pointer new_first{AllocatorTraits::allocate(allocator_, new_size)};
    Pointer new_current{new_first};
    try {
      std::for_each(begin(), end(), [&new_current, this](auto& value) constexpr -> void {
        AllocatorTraits::construct(allocator_, new_current, std::move_if_noexcept(value));
        ++new_current;
      });
    } catch (...) {
      for (Pointer traverser{new_first}; traverser != new_current; ++traverser) {
        AllocatorTraits::destroy(allocator_, traverser);
      }
      AllocatorTraits::deallocate(allocator_, new_first, new_size);
      throw;
    }
    for (Pointer traverser{new_first}; traverser != new_current; ++traverser) {
      AllocatorTraits::destroy(allocator_, traverser);
    }
    if (first_) {
      AllocatorTraits::deallocate(allocator_, first_, Capacity());
    }
    first_ = new_first;
    current_ = new_current;
    last_ = new_first + new_size;
  }

  [[nodiscard]] constexpr auto ComputeNewCapacity() const noexcept -> SizeType {
    const SizeType old_capacity{Capacity()};
    return old_capacity + (old_capacity >> 1) + 2;
  }

 public:
  constexpr auto EmplaceBack(auto&&... args) -> Reference {
    if (current_ == last_) [[unlikely]] {
      const SizeType new_capacity{ComputeNewCapacity()};
      InternalResize(new_capacity);
    }
    AllocatorTraits::construct(allocator_, current_, std::forward<decltype(args)>(args)...);
    return *current_++;
  }

  constexpr auto PopBack() -> void {
    utility::LabContainersAssert(!Empty(), "Undefined behaviour: Vector::PopBack(): called on empty vector");
    AllocatorTraits::destroy(allocator_, --current_);
  }

  constexpr auto Emplace(ConstIterator position, auto&&... args) -> Iterator {
    if (position.current_ != current_) [[likely]] {
      Pointer emplace_position{position.current_};
      if (current_ == last_) [[unlikely]] {
        const SizeType delta{static_cast<SizeType>(std::distance(cbegin(), position))};
        {
          const SizeType new_capacity{ComputeNewCapacity()};
          InternalResize(new_capacity);
        }
        emplace_position = first_ + delta;
      }

      AllocatorTraits::construct(allocator_, current_, std::move_if_noexcept(*(current_ - 1)));
      ++current_;

      for (Pointer backward_traverser{current_ - 2}; backward_traverser != emplace_position; --backward_traverser) {
        *backward_traverser = std::move_if_noexcept(*(backward_traverser - 1));
      }

      AllocatorTraits::destroy(allocator_, emplace_position);
      AllocatorTraits::construct(allocator_, emplace_position, std::forward<decltype(args)>(args)...);
      return emplace_position;
    } else [[unlikely]] {
      return &EmplaceBack(std::forward<decltype(args)>(args)...);
    }
  }

  constexpr auto Insert(ConstIterator position, const ValueType& value) -> Iterator { return Emplace(position, value); }

  constexpr auto Insert(ConstIterator position, ValueType&& value) -> Iterator {
    return Emplace(position, std::move(value));
  }

  constexpr auto Erase(ConstIterator position) -> Iterator {
    utility::LabContainersAssert(!Empty(), "Undefined behaviour: Vector::Erase(): called on empty vector");
    for (Pointer next_element{position.current_ + 1}; next_element != current_; ++next_element) {
      *position.current_ = std::move_if_noexcept(*next_element);
      ++position;
    }
    AllocatorTraits::destroy(allocator_, --current_);
    return position;
  }

  constexpr auto Clear() -> void {
    if (!first_) [[unlikely]] {
      return;
    }
    for (Pointer traverser{first_}; traverser != current_; ++traverser) {
      AllocatorTraits::destroy(allocator_, traverser);
    }
    AllocatorTraits::deallocate(allocator_, first_, Capacity());
    first_ = nullptr;
    current_ = nullptr;
    last_ = nullptr;
  }

  constexpr auto Resize(const SizeType count) -> void { Resize(count, ValueType{}); }

  constexpr auto Resize(SizeType count, const ValueType& value) -> void {
    const SizeType size{Size()};
    if (!count || count == size) [[unlikely]] {
      return;
    }
    if (count < size) {
      for (SizeType delta{size - count}; delta; --delta) {
        AllocatorTraits::destroy(allocator_, --current_);
      }
    } else {
      if (SizeType new_size{size + count}; new_size >= Capacity()) {
        InternalResize(new_size);
      }
      Pointer previous_current{current_};
      for (SizeType delta{count - size}; delta; --delta) {
        try {
          AllocatorTraits::construct(allocator_, current_, value);
          ++current_;
        } catch (...) {
          for (Pointer traverser{previous_current}; traverser != current_; ++traverser) {
            AllocatorTraits::destroy(allocator_, traverser);
          }
          current_ = previous_current;
          throw;
        }
      }
    }
  }

  constexpr auto Reserve(const SizeType elements) -> void {
    if (!elements) [[unlikely]] {
      return;
    }
    const SizeType new_capacity{Capacity() + elements};
    Pointer new_first{AllocatorTraits::allocate(allocator_, new_capacity)};
    Pointer new_current{new_first};
    try {
      std::for_each(begin(), end(), [&new_current, this](auto& value) constexpr -> void {
        AllocatorTraits::construct(allocator_, new_current, std::move_if_noexcept(value));
        ++new_current;
      });
    } catch (...) {
      for (Pointer traverser{new_first}; traverser != new_current; ++traverser) {
        AllocatorTraits::destroy(allocator_, traverser);
      }
      AllocatorTraits::deallocate(allocator_, new_first, new_capacity);
      throw;
    }
    for (Pointer traverser{first_}; traverser != current_; ++traverser) {
      AllocatorTraits::destroy(allocator_, traverser);
    }
    AllocatorTraits::deallocate(allocator_, first_, new_capacity - elements);
    first_ = new_first;
    current_ = new_current;
    last_ = new_first + new_capacity;
  }

  constexpr auto ShrinkToFit() -> void {
    if (current_ == last_) [[unlikely]] {
      return;
    }
    InternalResize(Size());
  }

  constexpr auto Swap(Vector& other) noexcept -> void {
    utility::LabContainersAssert(this != &other, "Undefined behaviour: Vector::Swap(): self swap detected");
    std::swap(first_, other.first_);
    std::swap(current_, other.current_);
    std::swap(last_, other.last_);
    if constexpr (AllocatorTraits::propagate_on_container_swap::value) {
      using std::swap;
      swap(allocator_, other.allocator_);
    }
  }

 private:
  Pointer first_{nullptr};
  Pointer current_{nullptr};
  Pointer last_{nullptr};
  [[no_unique_address]] AllocatorType allocator_;
};

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto begin(Vector<T, Allocator>& vector) noexcept { // NOLINT: std compatible interface
  return vector.begin();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto end(Vector<T, Allocator>& vector) noexcept { // NOLINT: std compatible interface
  return vector.end();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto cbegin(const Vector<T, Allocator>& vector) noexcept { // NOLINT: std compatible interface
  return vector.cbegin();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto cend(const Vector<T, Allocator>& vector) noexcept { // NOLINT: std compatible interface
  return vector.cend();
}

template<typename T, typename Allocator>
constexpr auto swap(Vector<T, Allocator>& lhs, Vector<T, Allocator>& rhs) noexcept -> void { // NOLINT: std compatible interface
  lhs.Swap(rhs);
}

}  // namespace lab::containers
