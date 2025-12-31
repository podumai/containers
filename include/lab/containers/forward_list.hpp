#pragma once

#include <algorithm>
#include <compare>
#include <concepts>
#include <expected>
#include <iterator>
#include <lab/containers/assert.hpp>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

namespace lab::containers {

namespace details {

template<typename T>
struct ForwardListNode {
  constexpr explicit ForwardListNode(auto&&... args) noexcept(std::is_nothrow_constructible_v<T, decltype(args)...>)
    : value_(std::forward<decltype(args)>(args)...) { }

  ForwardListNode* next_{nullptr};
  T value_;
};

}  // namespace details

struct [[maybe_unused]] ForwardListBeforeBeginSentinel final { };

struct [[maybe_unused]] ForwardListSentinel final { };

template<bool IsConst, typename ForwardListType>
class [[nodiscard]] ForwardListIteratorBase {
  template<typename, typename>
  friend class ForwardList;

  friend class ForwardListIteratorBase<!IsConst, ForwardListType>;

  using NodePointer = ForwardListType::NodePointer;

 public:
  using value_type = ForwardListType::value_type;
  using ValueType = ForwardListType::ValueType;
  using reference =
    std::conditional_t<IsConst, typename ForwardListType::const_reference, typename ForwardListType::reference>;
  using Reference = reference;
  using pointer =
    std::conditional_t<IsConst, typename ForwardListType::const_pointer, typename ForwardListType::pointer>;
  using Pointer = pointer;
  using difference_type = ForwardListType::difference_type;
  using DifferenceType = ForwardListType::DifferenceType;
  using iterator_category = std::forward_iterator_tag;
  using IteratorCategory = iterator_category;

  constexpr ForwardListIteratorBase() noexcept = default;

 protected:
  constexpr ForwardListIteratorBase(NodePointer list_node) noexcept : current_{list_node} { }

 public:
  constexpr ~ForwardListIteratorBase() = default;

  constexpr auto operator->() const noexcept -> Pointer { return &current_->value_; }

  constexpr auto operator*() const noexcept -> Reference { return current_->value_; }

  constexpr auto operator++() noexcept -> ForwardListIteratorBase& {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ForwardListIteratorBase::operator++(): nullptr dereference");
    current_ = current_->next_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> ForwardListIteratorBase {
    LAB_CONTAINERS_ASSERT(
      current_, "Undefined behaviour: ForwardListIteratorBase::operator++(int): nullptr dereference"
    );
    ForwardListIteratorBase previous{*this};
    current_ = current_->next_;
    return previous;
  }

  [[nodiscard]] friend constexpr auto operator==(
    const ForwardListIteratorBase lhs, const ForwardListIteratorBase rhs
  ) noexcept -> bool {
    return lhs.current_ == rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator!=(
    const ForwardListIteratorBase lhs, const ForwardListIteratorBase rhs
  ) noexcept -> bool {
    return lhs.current_ != rhs.current_;
  }

  [[nodiscard]] friend constexpr auto
  operator==(const ForwardListIteratorBase iterator, const ForwardListSentinel /* sentinel */) noexcept -> bool {
    return !iterator.current_;
  }

  [[nodiscard]] friend constexpr auto
  operator!=(const ForwardListIteratorBase iterator, const ForwardListSentinel /* sentinel */) noexcept -> bool {
    return iterator.current_;
  }

  [[nodiscard]] constexpr operator ForwardListIteratorBase<!IsConst, ForwardListType>() const noexcept {
    return current_;
  }

 private:
  NodePointer current_{nullptr};
};

enum struct ForwardListErrors : unsigned char { kUnknown, kEmpty, kOutOfRange };

template<typename T, typename Allocator = std::allocator<T>>
class [[nodiscard]] ForwardList {
  friend class ForwardListIteratorBase<false, ForwardList>;
  friend class ForwardListIteratorBase<true, ForwardList>;

  using InternalAllocatorType = std::allocator_traits<Allocator>::template rebind_alloc<details::ForwardListNode<T>>;
  using AllocatorTraits = std::allocator_traits<InternalAllocatorType>;
  using NodePointer = details::ForwardListNode<T>*;

 public:
  using value_type = T;
  using ValueType = T;
  using reference = value_type&;
  using Reference = reference;
  using const_reference = const value_type&;
  using ConstReference = const_reference;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using Pointer = pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;
  using ConstPointer = const_pointer;
  using size_type = AllocatorTraits::size_type;
  using SizeType = size_type;
  using difference_type = AllocatorTraits::difference_type;
  using DifferenceType = difference_type;
  using allocator_type = InternalAllocatorType;
  using AllocatorType = allocator_type;
  using iterator = ForwardListIteratorBase<false, ForwardList>;
  using Iterator = iterator;
  using const_iterator = ForwardListIteratorBase<true, ForwardList>;
  using ConstIterator = const_iterator;

  constexpr ForwardList() noexcept = default;

  constexpr explicit ForwardList(const Allocator& allocator) noexcept : allocator_{allocator} { }

  constexpr ForwardList(ForwardList&& other) noexcept
    : head_{std::exchange(other.head_, nullptr)}, allocator_{std::move(other.allocator_)} { }

  constexpr ForwardList(const ForwardList& other)
    : ForwardList{
        other.cbegin(), other.cend(), AllocatorTraits::select_on_container_copy_construction(other.allocator_)
      } { }

 private:
  constexpr auto ConstructNode(auto&&... args) -> NodePointer {
    NodePointer list_node{AllocatorTraits::allocate(allocator_, 1)};
    try {
      AllocatorTraits::construct(allocator_, list_node, std::forward<decltype(args)>(args)...);
    } catch (...) {
      AllocatorTraits::deallocate(allocator_, list_node, 1);
      throw;
    }
    return list_node;
  }

 public:
  template<std::input_iterator InputIter, std::sentinel_for<InputIter> Sentinel>
  constexpr ForwardList(InputIter first, Sentinel last, const Allocator& allocator = Allocator{})
    : ForwardList{allocator} {
    if (first == last) [[unlikely]] {
      return;
    }
    head_ = ConstructNode(*first++);
    NodePointer traverser{head_};
    while (first != last) {
      try {
        traverser->next_ = ConstructNode(*first);
      } catch (...) {
        Clear();
        throw;
      }
      traverser = traverser->next_;
      ++first;
    }
  }

  constexpr ForwardList(std::initializer_list<ValueType> ilist, const Allocator& allocator = Allocator{})
    : ForwardList{ilist.begin(), ilist.end(), allocator} { }

  constexpr ~ForwardList() { Clear(); }

  constexpr auto operator=(ForwardList&& other) noexcept -> ForwardList& {
    LAB_CONTAINERS_ASSERT(
      this != &other, "Undefined behaviour: ForwardList::operator=(ForwardList&&): self move detected"
    );
    Clear();
    head_ = std::exchange(other.head_, nullptr);
    if constexpr (AllocatorTraits::propagate_on_container_move_assignment::value) {
      allocator_ = std::move(other.allocator_);
    }
    return *this;
  }

  constexpr auto operator=(const ForwardList& other) -> ForwardList& {
    LAB_CONTAINERS_ASSERT(
      this != &other, "Undefined behaviour: ForwardList::operator=(const ForwardList&): self copy detected"
    );
    ForwardList{other}.Swap(*this);
    return *this;
  }

  [[nodiscard]] friend constexpr auto operator==(const ForwardList& lhs, const ForwardList& rhs) noexcept -> bool {
    return std::ranges::equal(lhs, rhs);
  }

  [[nodiscard]] friend constexpr auto operator<=>(const ForwardList& lhs, const ForwardList& rhs) noexcept {
    return std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
  }

 private:
  constexpr auto DeleteNode(NodePointer list_node) -> void {
    AllocatorTraits::destroy(allocator_, list_node);
    AllocatorTraits::deallocate(allocator_, list_node, 1);
  }

 public:
  constexpr auto Clear() -> void {
    while (head_) {
      NodePointer list_node{head_};
      head_ = head_->next_;
      DeleteNode(list_node);
    }
  }

  [[nodiscard]] constexpr auto BeforeBegin() noexcept -> ForwardListBeforeBeginSentinel { return {}; }

  [[nodiscard]] constexpr auto begin() noexcept -> Iterator { return head_; }

  [[nodiscard]] constexpr auto end() noexcept -> ForwardListSentinel { return {}; }

  [[nodiscard]] constexpr auto cbegin() const noexcept -> ConstIterator { return head_; }

  [[nodiscard]] constexpr auto cend() const noexcept -> ForwardListSentinel { return {}; }

  constexpr auto PushFront(const ValueType& value) -> void { EmplaceFront(value); }

  constexpr auto PushFront(ValueType&& value) -> void { EmplaceFront(std::move(value)); }

  constexpr auto EmplaceFront(auto&&... args) -> void {
    NodePointer list_node{ConstructNode(std::forward<decltype(args)>(args)...)};
    if (head_) [[likely]] {
      list_node->next_ = head_;
      head_ = list_node;
    } else [[unlikely]] {
      head_ = list_node;
    }
  }

  constexpr auto PopFront() -> void {
    LAB_CONTAINERS_ASSERT(!Empty(), "Undefined behaviour: ForwardList::PopFront(): called on empty list");
    NodePointer list_node{head_};
    head_ = head_->next_;
    DeleteNode(list_node);
  }

  constexpr auto PushBack(const ValueType& value) -> void { EmplaceBack(value); }

  constexpr auto PushBack(ValueType&& value) -> void { EmplaceBack(std::move(value)); }

  constexpr auto EmplaceBack(auto&&... args) -> void {
    NodePointer list_node{ConstructNode(std::forward<decltype(args)>(args)...)};
    if (head_) [[likely]] {
      NodePointer traverser{head_};
      while (traverser->next_) {
        traverser = traverser->next_;
      }
      traverser->next_ = list_node;
    } else [[unlikely]] {
      head_ = list_node;
    }
  }

  constexpr auto PopBack() -> void {
    LAB_CONTAINERS_ASSERT(Empty(), "Undefined behaviour: ForwardList::PopBack(): called on empty list");
    if (head_->next_) [[likely]] {
      NodePointer traverser{head_};
      while (traverser->next_->next_) {
        traverser = traverser->next_;
      }
      DeleteNode(std::exchange(traverser->next_, nullptr));
    } else [[unlikely]] {
      DeleteNode(std::exchange(head_, nullptr));
    }
  }

  [[nodiscard]] constexpr auto Size() const noexcept -> SizeType { return std::ranges::distance(cbegin(), cend()); }

  [[nodiscard]] constexpr auto Empty() const noexcept -> bool { return !head_; }

  [[nodiscard]] constexpr auto operator[](SizeType index) noexcept -> Reference {
    LAB_CONTAINERS_ASSERT(Empty(), "Undefined behaviour: ForwardList::operator[]: called on empty list");
#ifdef LAB_CONTAINERS_FORWARD_LIST_SUBSCRIPT_INDEX_CHECK
    LAB_CONTAINERS_ASSERT(Size() > index, "Undefined behaviour: ForwardList::operator[]: index is out of range");
#endif
    NodePointer traverser{head_};
    while (index--) {
      traverser = traverser->next_;
    }
    return traverser->value_;
  }

  [[nodiscard]] constexpr auto operator[](SizeType index) const noexcept -> ConstReference {
    LAB_CONTAINERS_ASSERT(Empty(), "Undefined behaviour: ForwardList::operator[] const: called on empty list");
#ifdef LAB_CONTAINERS_FORWARD_LIST_SUBSCRIPT_INDEX_CHECK
    LAB_CONTAINERS_ASSERT(Size() > index, "Undefined behaviour: ForwardList::operator[] const: index is out of range");
#endif
    NodePointer traverser{head_};
    while (index--) {
      traverser = traverser->next_;
    }
    return traverser->value_;
  }

  constexpr auto InsertAfter(ForwardListBeforeBeginSentinel /* sentinel */, const ValueType& value) -> void {
    PushFront(value);
  }

  constexpr auto InsertAfter(ForwardListBeforeBeginSentinel /* sentinel */, ValueType&& value) -> void {
    PushFront(std::move(value));
  }

  constexpr auto InsertAfter(ConstIterator position, const ValueType& value) -> Iterator {
    return EmplaceAfter(position, value);
  }

  constexpr auto InsertAfter(ConstIterator position, ValueType&& value) -> Iterator {
    return EmplaceAfter(position, std::move(value));
  }

  constexpr auto EmplaceAfter(ConstIterator position, auto&&... args) -> Iterator {
    NodePointer list_node{ConstructNode(std::forward<decltype(args)>(args)...)};
    if (head_) [[likely]] {
      list_node->next_ = position.current_->next_;
      position.current_->next_ = list_node;
    } else [[unlikely]] {
      head_ = list_node;
    }
    return list_node;
  }

  constexpr auto EraseAfter(ForwardListBeforeBeginSentinel /* sentinel */) -> void { PopFront(); }

  constexpr auto EraseAfter(ConstIterator position) -> void {
    LAB_CONTAINERS_ASSERT(!Empty(), "Undefined behaviour: ForwardList::EraseAfter(): called on empty list");
    NodePointer list_node{position.current_->next_};
    position.current_->next_ = list_node->next_;
    DeleteNode(list_node);
  }

  [[nodiscard]] constexpr auto Front() noexcept -> std::expected<std::reference_wrapper<ValueType>, ForwardListErrors> {
    if (head_) [[likely]] {
      return head_->value_;
    }
    return std::unexpected{ForwardListErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto Front() const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, ForwardListErrors> {
    if (head_) [[likely]] {
      return head_->value_;
    }
    return std::unexpected{ForwardListErrors::kEmpty};
  }

  [[nodiscard]] constexpr auto GetAllocator() const noexcept -> AllocatorType { return allocator_; }

  constexpr auto Swap(ForwardList& other) noexcept -> void {
    LAB_CONTAINERS_ASSERT(this != &other, "Undefined behaviour: ForwardList::Swap(): self copy detected");
    std::swap(head_, other.head_);
    if constexpr (AllocatorTraits::propagate_on_container_swap::value) {
      using std::swap;
      swap(allocator_, other.allocator_);
    }
  }

 private:
  NodePointer head_{nullptr};
  [[no_unique_address]] AllocatorType allocator_;
};

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto begin(ForwardList<T, Allocator>& list) noexcept {
  return list.begin();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto end(ForwardList<T, Allocator>& list) noexcept {
  return list.end();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto cbegin(const ForwardList<T, Allocator>& list) noexcept {
  return list.cbegin();
}

template<typename T, typename Allocator>
[[nodiscard]] constexpr auto cend(const ForwardList<T, Allocator>& list) noexcept {
  return list.cend();
}

template<typename T, typename Allocator>
constexpr auto swap(ForwardList<T, Allocator>& lhs, ForwardList<T, Allocator>& rhs) noexcept -> void {
  lhs.Swap(rhs);
}

}  // namespace lab::containers
