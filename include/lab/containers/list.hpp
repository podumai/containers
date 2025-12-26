#pragma once

#include <algorithm>
#include <concepts>
#include <expected>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include "assert.hpp"

namespace lab::containers {

namespace details {

template<typename T>
struct [[nodiscard]] ListNode final {
  template<typename... Args>
  explicit ListNode(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
    : value_(std::forward<Args>(args)...) { }

  ListNode* next_{nullptr};
  ListNode* prev_{nullptr};
  T value_;
};

template<typename NodeType>
struct [[nodiscard]] ListProxyNode final {
  using NodePointer = NodeType*;

  NodePointer head_{nullptr};
  NodePointer tail_{nullptr};
};

}  // namespace details

struct [[maybe_unused]] ListSentinel final { };

template<bool IsConst, typename ListType>
class [[nodiscard]] ListIteratorBase {
  template<typename, typename>
  friend class List;

  friend class ListIteratorBase<!IsConst, ListType>;

  using NodePointer = ListType::NodeType*;

 public:
  using value_type = ListType::value_type;
  using ValueType = ListType::ValueType;
  using reference = std::conditional_t<IsConst, typename ListType::const_reference, typename ListType::reference>;
  using Reference = reference;
  using pointer = std::conditional_t<IsConst, typename ListType::const_pointer, typename ListType::ConstPointer>;
  using Pointer = pointer;
  using difference_type = ListType::difference_type;
  using DifferenceType = ListType::DifferenceType;
  using iterator_category = std::bidirectional_iterator_tag;
  using IteratorCategory = iterator_category;

  constexpr ListIteratorBase() noexcept = default;

 protected:
  constexpr ListIteratorBase(NodePointer list_node) noexcept : current_{list_node} { }

 public:
  constexpr ~ListIteratorBase() = default;

  constexpr auto operator->() const noexcept -> Pointer {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator->(): nullptr dereference");
    return &current_->value_;
  }

  [[nodiscard]] constexpr auto operator*() const noexcept -> Reference {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator*(): nullptr dereference");
    return current_->value_;
  }

  constexpr auto operator++() noexcept -> ListIteratorBase& {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator++(): nullptr dereference");
    current_ = current_->next_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> ListIteratorBase {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator++(int): nullptr dereference");
    auto previous = *this;
    current_ = current_->next_;
    return previous;
  }

  constexpr auto operator--() noexcept -> ListIteratorBase& {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator--(): nullptr dereference");
    current_ = current_->prev_;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> ListIteratorBase {
    LAB_CONTAINERS_ASSERT(current_, "Undefined behaviour: ListIteratorBase::operator--(int): nullptr dereference");
    auto previous = *this;
    current_ = current_->prev_;
    return previous;
  }

  [[nodiscard]] friend constexpr auto operator==(const ListIteratorBase lhs, const ListIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ == rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator!=(const ListIteratorBase lhs, const ListIteratorBase rhs) noexcept
    -> bool {
    return lhs.current_ != rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator==(
    const ListSentinel /* sentinel */, const ListIteratorBase iterator
  ) noexcept -> bool {
    return !iterator.current_;
  }

  [[nodiscard]] friend constexpr auto
  operator==(const ListIteratorBase iterator, const ListSentinel /* sentinel */) noexcept -> bool {
    return !iterator.current_;
  }

  [[nodiscard]] friend constexpr auto operator!=(
    const ListSentinel /* sentinel */, const ListIteratorBase iterator
  ) noexcept -> bool {
    return iterator.current_;
  }

  [[nodiscard]] friend constexpr auto
  operator!=(const ListIteratorBase iterator, const ListSentinel /* sentinel */) noexcept -> bool {
    return iterator.current_;
  }

  [[nodiscard]] constexpr operator ListIteratorBase<!IsConst, ListType>() const noexcept { return current_; }

 private:
  NodePointer current_{nullptr};
};

template<typename ListIterator>
class [[nodiscard]] ListReverseIteratorAdaptor final : public ListIterator {
  using Base = ListIterator;

 public:
  using value_type = Base::value_type;
  using ValueType = Base::ValueType;
  using reference = Base::reference;
  using Reference = Base::Reference;
  using pointer = Base::pointer;
  using Pointer = Base::Pointer;
  using difference_type = Base::difference_type;
  using DifferenceType = Base::DifferenceType;
  using iterator_category = Base::iterator_category;
  using IteratorCategory = Base::IteratorCategory;

  using ListIterator::ListIterator;

  constexpr auto operator++() noexcept -> ListReverseIteratorAdaptor& {
    LAB_CONTAINERS_ASSERT(
      *this != ListSentinel{}, "Undefined behaviour: ListReverseIteratorAdaptor::operator++(): out of range"
    );
    Base::operator--();
    return *this;
  }

  constexpr auto operator++(int) noexcept -> ListReverseIteratorAdaptor {
    LAB_CONTAINERS_ASSERT(
      *this != ListSentinel{}, "Undefined behaviour: ListReverseIteratorAdaptor::operator++(int): out of range"
    );
    auto previous = *this;
    Base::operator--();
    return previous;
  }

  constexpr auto operator--() noexcept -> ListReverseIteratorAdaptor& {
    LAB_CONTAINERS_ASSERT(
      *this != ListSentinel{}, "Undefined behaviour: ListReverseIteratorAdaptor::operator--(): out of range"
    );
    Base::operator++();
    return *this;
  }

  constexpr auto operator--(int) noexcept -> ListReverseIteratorAdaptor {
    LAB_CONTAINERS_ASSERT(
      *this != ListSentinel{}, "Undefined behaviour: ListReverseIteratorAdaptor::operator--(int): out of range"
    );
    auto previous = *this;
    Base::operator++();
    return previous;
  }
};

enum struct ListErrors : unsigned char { kUnknown, kEmpty, kOutOfRange };

template<typename T, typename Allocator = std::allocator<T>>
class [[nodiscard]] List final {
  template<bool, typename>
  friend class ListIteratorBase;

  using InternalAllocatorType = std::allocator_traits<Allocator>::template rebind_alloc<details::ListNode<T>>;
  using AllocatorTraits = std::allocator_traits<InternalAllocatorType>;
  using NodeType = details::ListNode<T>;

 public:
  using value_type = T;
  using ValueType = T;
  using reference = value_type&;
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
  using allocator_type = InternalAllocatorType;
  using AllocatorType = allocator_type;
  using iterator = ListIteratorBase<false, List>;
  using Iterator = iterator;
  using const_iterator = ListIteratorBase<true, List>;
  using ConstIterator = const_iterator;
  using reverse_iterator = ListReverseIteratorAdaptor<Iterator>;
  using ReverseIterator = reverse_iterator;
  using const_reverse_iterator = ListReverseIteratorAdaptor<ConstIterator>;
  using ConstReverseIterator = const_reverse_iterator;

  constexpr List() = default;

  constexpr explicit List(const Allocator& allocator = Allocator{}) : allocator_{allocator} { }

  constexpr List(const List& other)
    : List{other.cbegin(), other.cend(), AllocatorTraits::select_on_container_copy_construction(other.allocator_)} { }

  constexpr List(List&& other) noexcept
    : proxy_node_{std::move(other.proxy_node_)}
    , size_{std::exchange(other.size_, 0)}
    , allocator_{std::move(other.allocator_)} { }

 private:
  /**
   * @private
   * @internal
   *
   * @param[in] args
   */
  [[nodiscard]] constexpr auto ConstructNode(auto&&... args) -> NodeType* {
    auto list_node = AllocatorTraits::allocate(allocator_, 1);
    if constexpr (std::is_nothrow_constructible_v<ValueType, decltype(args)...>) {
      AllocatorTraits::construct(allocator_, list_node, std::forward<decltype(args)>(args)...);
    } else {
      try {
        AllocatorTraits::construct(allocator_, list_node, std::forward<decltype(args)>(args)...);
      } catch (...) {
        AllocatorTraits::deallocate(allocator_, list_node, 1);
        throw;
      }
    }
    return list_node;
  }

 public:
  /**
   * @public
   *
   * @tparam InputIter
   * @tparam Sentinel
   *
   * @param[in] first
   * @param[in] last
   * @param[in] allocator
   */
  template<std::input_iterator InputIterator, typename Sentinel>
  constexpr List(InputIterator first, Sentinel last, const Allocator& allocator = Allocator{}) : List{allocator} {
    if (first == last) [[unlikely]] {
      return;
    }
    proxy_node_->head_ = ConstructNode(*first++);
    auto traverser = proxy_node_->head_;
    size_ = 1;
    while (first != last) {
      try {
        auto list_node = ConstructNode(*first++);
        traverser->next_ = list_node;
        list_node->prev_ = traverser;
        traverser = traverser->next_;
      } catch (...) {
        Clear();
        throw;
      }
      ++size_;
    }
    proxy_node_->tail_ = traverser;
  }

  /**
   * @public
   *
   * @param[in] ilist
   * @param[in] allocator
   */
  constexpr explicit List(std::initializer_list<ValueType> ilist, const Allocator& allocator = Allocator{})
    : List{ilist.begin(), ilist.end(), allocator} { }

  constexpr ~List() { Clear(); }

  constexpr auto operator=(List&& other) noexcept -> List& {
    LAB_CONTAINERS_ASSERT(this != &other, "List::operator=(List&&): self move operation");
    Clear();
    proxy_node_ = std::move(other.proxy_node_);
    size_ = std::exchange(other.size_, 0);
    if constexpr (AllocatorTraits::propagate_on_container_move_assignment::value) {
      allocator_ = std::move(other.allocator_);
    }
    return *this;
  }

  constexpr auto operator=(const List& other) -> List& {
    LAB_CONTAINERS_ASSERT(this != &other, "List::operator=(const List&): self copy operation");
    List{other}.Swap(*this);
    return *this;
  }

  /**
   * @public
   *
   * @param[in] value
   */
  constexpr auto PushFront(const ValueType& value) -> void { EmplaceFront(value); }

  /**
   * @public
   *
   * @param[in] value
   */
  constexpr auto PushFront(ValueType&& value) -> void { EmplaceFront(std::move(value)); }

  /**
   * @public
   *
   * @param[in] args
   */
  constexpr auto EmplaceFront(auto&&... args) -> void {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::EmplaceFront(): called on list in moved-from state");
    auto list_node = ConstructNode(std::forward<decltype(args)>(args)...);
    if (proxy_node_->head_) [[likely]] {
      list_node->next_ = proxy_node_->head_;
      proxy_node_->head_->prev_ = list_node;
      proxy_node_->head_ = list_node;
    } else [[unlikely]] {
      proxy_node_->head_ = list_node;
      proxy_node_->tail_ = list_node;
    }
    ++size_;
  }

  /**
   * @public
   *
   * @param[in] value
   */
  constexpr auto PushBack(const ValueType& value) -> void { EmplaceBack(value); }

  /**
   * @public
   *
   * @param[in] value
   */
  constexpr auto PushBack(ValueType&& value) -> void { EmplaceBack(std::move(value)); }

  /**
   * @public
   *
   * @param[in] args
   */
  constexpr auto EmplaceBack(auto&&... args) -> void {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::EmplaceBack(): called on list in moved-from state");
    auto list_node = ConstructNode(std::forward<decltype(args)>(args)...);
    if (proxy_node_->tail_) [[likely]] {
      proxy_node_->tail_->next_ = list_node;
      list_node->prev_ = proxy_node_->tail_;
      proxy_node_->tail_ = list_node;
    } else [[unlikely]] {
      proxy_node_->head_ = list_node;
      proxy_node_->tail_ = list_node;
    }
    ++size_;
  }

  /**
   * @public
   */
  constexpr auto PopFront() -> void {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::PopFront(): called on list in moved-from state");
    LAB_CONTAINERS_ASSERT(proxy_node_->head_, "Undefined behaviour: List::PopFront(): called on empty list");
    auto list_node = proxy_node_->head_;
    proxy_node_->head_ = list_node->next_;
    if (proxy_node_->tail_ != list_node) [[likely]] {
      list_node->next_->prev_ = nullptr;
    } else [[unlikely]] {
      proxy_node_->tail_ = nullptr;
    }
    --size_;
    DeleteNode(list_node);
  }

  /**
   * @public
   */
  constexpr auto PopBack() -> void {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::PopBack(): called on list in moved-from state");
    LAB_CONTAINERS_ASSERT(proxy_node_->tail_, "Undefined behaviour: List::PopBack(): called on empty list");
    auto list_node = proxy_node_->tail_;
    proxy_node_->tail_ = list_node->prev_;
    if (proxy_node_->head_ != list_node) [[likely]] {
      list_node->prev_->next_ = nullptr;
    } else [[unlikely]] {
      proxy_node_->head_ = nullptr;
    }
    --size_;
    DeleteNode(list_node);
  }

 private:
  /**
   * @private
   * @internal
   *
   * @param[in] list_node
   */
  constexpr auto DeleteNode(NodeType* list_node) -> void {
    AllocatorTraits::destroy(allocator_, list_node);
    AllocatorTraits::deallocate(allocator_, list_node, 1);
  }

  constexpr auto DeleteNodes() -> void {
    while (proxy_node_->head_) {
      auto list_node = proxy_node_->head_;
      proxy_node_->head_ = list_node->next_;
      DeleteNode(list_node);
    }
    proxy_node_->tail_ = nullptr;
    size_ = SizeType{0};
  }

 public:
  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto begin() noexcept -> Iterator { return {proxy_node_->head_}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto end() noexcept -> ListSentinel { return {}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto cbegin() const noexcept -> ConstIterator { return {proxy_node_->head_}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto cend() const noexcept -> ListSentinel { return {}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto rbegin() noexcept -> ReverseIterator { return {proxy_node_->tail_}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto rend() noexcept -> ListSentinel { return {}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto crbegin() const noexcept -> ConstReverseIterator { return {proxy_node_->tail_}; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto crend() const noexcept -> ListSentinel { return {}; }

  /**
   * @public
   */
  constexpr auto Clear() -> void {
    if (proxy_node_) {
      DeleteNodes();
    }
  }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Empty() const noexcept -> bool { return !size_; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Size() const noexcept -> SizeType { return size_; }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Front() noexcept -> std::expected<std::reference_wrapper<ValueType>, ListErrors> {
    if (proxy_node_->head_) [[likely]] {
      return proxy_node_->head_->value_;
    }
    return std::unexpected{ListErrors::kEmpty};
  }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Front() const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, ListErrors> {
    if (proxy_node_->head_) [[likely]] {
      return proxy_node_->head_->value_;
    }
    return std::unexpected{ListErrors::kEmpty};
  }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Back() noexcept -> std::expected<std::reference_wrapper<ValueType>, ListErrors> {
    if (proxy_node_->tail_) [[likely]] {
      return proxy_node_->tail_->value_;
    }
    return std::unexpected{ListErrors::kEmpty};
  }

  /**
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto Back() const noexcept
    -> std::expected<std::reference_wrapper<const ValueType>, ListErrors> {
    if (proxy_node_->tail_) [[likely]] {
      return proxy_node_->tail_->value_;
    }
    return std::unexpected{ListErrors::kEmpty};
  }

#ifdef LAB_CONTAINERS_LIST_ITERATOR_RANGE_CHECK
  [[nodiscard]] constexpr auto IteratorRangeCheck(ConstIterator iterator) const noexcept -> bool {
    auto first = cbegin();
    auto last = cend();
    while (first != last) {
      if (first == iterator) {
        return true;
      }
      ++first;
    }
    return false;
  }
#endif

  /**
   * @public
   *
   * @param[in] position
   * @param[in] value
   */
  constexpr auto Insert(ConstIterator position, const ValueType& value) -> void { Emplace(position, value); }

  /**
   * @public
   *
   * @param[in] position
   * @param[in] value
   */
  constexpr auto Insert(ConstIterator position, ValueType&& value) -> void { Emplace(position, std::move(value)); }

  /**
   * @public
   *
   * @param[in] position
   * @param[in] args
   */
  constexpr auto Emplace(ConstIterator position, auto&&... args) -> Iterator {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::Emplace(): called on list in moved-from state");
#ifdef LAB_CONTAINERS_LIST_ITERATOR_RANGE_CHECK
    LAB_CONTAINERS_ASSERT(IteratorRangeCheck(position), "Undefined behaviour: List::Emplace(): iterator not in range");
#endif
    auto list_node = ConstructNode(std::forward<decltype(args)>(args)...);
    if (proxy_node_->head_) [[likely]] {
      list_node->next_ = position.current_;
      list_node->prev_ = position.current_->prev_;
      position.current_->prev_ = list_node;
    } else [[unlikely]] {
      proxy_node_->head_ = list_node;
      proxy_node_->tail_ = list_node;
    }
  }

  /**
   * @public
   */
  constexpr auto Erase(ConstIterator position) -> void {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::Erase(): called on list in moved-from state");
    LAB_CONTAINERS_ASSERT(proxy_node_->head_, "Undefined behaviour: List::Erase(): called on empty list");
#ifdef LAB_CONTAINERS_LIST_ITERATOR_RANGE_CHECK
    LAB_CONTAINERS_ASSERT(IteratorRangeCheck(iterator), "Undefined behaviour: List::Erase(): iterator not in range");
#endif
    if (proxy_node_->head_ == position.current_) {
      proxy_node_->head_ = position.current_->next_;
    }
    if (proxy_node_->tail_ == position.current_) {
      proxy_node_->tail_ = position.current_->prev_;
    }
    if (position.current_->prev_) [[likely]] {
      position.current_->prev_->next_ = position.current_->next_;
    }
    if (position.current_->next_) [[likely]] {
      position.current_->next_->prev_ = position.current_->prev_;
    }
    --size_;
    DeleteNode(position.current_);
  }

  /**
   * @public
   * @brief Provides read/write access to the value with given index.
   *
   * @param[in] index The zero-base index of the value to access.
   * @return `Reference` to obtain the specified value.
   *
   * @throws None (no-throw guarantee).
   *
   * @complexity O(n)
   *
   * @par Example:
   * @code{.cpp}
   * auto list = lab::containers::List{1, 2, 3};
   * const auto list_size = list.Size();
   * using SizeType = typename decltype(list)::SizeType;
   * for (auto i = SizeType{}; i < list_size; ++i) {
   *   // list[i] ... code
   * }
   * @endcode
   */
  [[nodiscard]] constexpr auto operator[](SizeType index) noexcept -> Reference {
    LAB_CONTAINERS_ASSERT(proxy_node_, "Undefined behaviour: List::operator[]: called on list in moved-from state");
    LAB_CONTAINERS_ASSERT(proxy_node_->head_, "Undefined behaviour: List::operator[]: called on empty list");
    LAB_CONTAINERS_ASSERT(index <= size_, "Undefined behaviour: List::operator[]: index out of range");
    auto traverser = proxy_node_->head_;
    while (index--) {
      traverser = traverser->next_;
    }
    return traverser->value_;
  }

  /**
   * @public
   * @brief Provides read/write access to the value with given index.
   *
   * @param[in] index The zero-base index of the value to access.
   * @return `Reference` to obtain the specified value.
   *
   * @throws None (no-throw guarantee).
   *
   * @complexity O(n)
   *
   * @par Example:
   * @code{.cpp}
   * const auto list = lab::containers::List{1, 2, 3};
   * const auto list_size = list.Size();
   * using SizeType = typename decltype(list)::SizeType;
   * for (auto i = SizeType{}; i < list_size; ++i) {
   *   // list[i] ... code
   * }
   * @endcode
   */
  [[nodiscard]] constexpr auto operator[](SizeType index) const noexcept -> ConstReference {
    LAB_CONTAINERS_ASSERT(
      proxy_node_, "Undefined behaviour: List::operator[] const: called on list in moved-from state"
    );
    LAB_CONTAINERS_ASSERT(proxy_node_->head_, "Undefined behaviour: List::operator[] const: called on empty list");
    LAB_CONTAINERS_ASSERT(index <= size_, "Undefined behaviour: List::operator[] const: index out of range");
    auto traverser = proxy_node_->head_;
    while (index--) {
      traverser = traverser->next_;
    }
    return traverser->value_;
  }

  /**
   * @public
   */
  [[nodiscard]] constexpr auto GetAllocator() const noexcept -> AllocatorType { return allocator_; }

  /**
   * @public
   */
  constexpr auto Swap(List& other) noexcept -> void {
    LAB_CONTAINERS_ASSERT(this != &other, "Undefined behaviour: List::Swap(List&): self swap detected");
    proxy_node_.swap(other.proxy_node_);
    if constexpr (AllocatorTraits::propagate_on_container_swap::value) {
      using std::swap;
      swap(allocator_, other.allocator_);
    }
  }

 private:
  std::unique_ptr<details::ListProxyNode<NodeType>> proxy_node_{std::make_unique<details::ListProxyNode<NodeType>>()};
  SizeType size_{};
  [[no_unique_address]] AllocatorType allocator_;
};

template<typename T, typename Allocator>
constexpr auto swap(List<T, Allocator>& lhs, List<T, Allocator>& rhs) noexcept -> void {
  lhs.Swap(rhs);
}

}  // namespace lab::containers
