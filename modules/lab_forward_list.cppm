module;

#include <cassert>
#include <concepts>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <tpu/helper_macros.hpp>
#include <tpu/modules/module_helper_macros.hpp>
#include <type_traits>
#include <utility>

export module lab_forward_list;

/**
 * @brief Concept for type validation
 * @internal
 * @concept IsValidForwardListType
 */
template<typename T>
concept IsValidForwardListType = (std::copyable<T> || std::movable<T>) && std::destructible<T>;

/**
 * @brief Concept for allocator validation
 * @internal
 * @concept IsValidForwardListAllocatorType
 */
template<typename Allocator, typename T>
concept IsValidForwardListAllocatorType =
  IsValidForwardListType<typename std::allocator_traits<Allocator>::value_type> &&
  std::same_as<typename std::allocator_traits<Allocator>::value_type, T>;

/**
 * @brief Internal node type for single-linked list
 * @internal
 * @struct
 *
 * @tparam T Value type to store in node
 */
template<typename T>
struct [[nodiscard]] ForwardListNode final {
  constexpr explicit ForwardListNode(auto&&... args) noexcept(std::is_nothrow_constructible_v<T, decltype(args)...>)
    : value_(std::forward<decltype(args)>(args)...) { }

  ForwardListNode* next_{nullptr};
  T value_{};
};

/**
 * @brief Iterator base for ForwardList (for const and non-const)
 * @internal
 * @class
 *
 * @tparam IsConst Boolean value for const iterator check
 * @tparam ForwardList ForwardList class type for traversing
 */
template<bool IsConst, typename ForwardList>
class ForwardListIteratorBase final {
  friend ForwardList;
  friend ForwardListIteratorBase<!IsConst, ForwardList>;
  using NodePointer = ForwardList::NodePointer;

 public:
  using ValueType = ForwardList::ValueType;
  using value_type = ForwardList::ValueType;
  using Reference = std::conditional_t<IsConst, typename ForwardList::ConstReference, typename ForwardList::Reference>;
  using reference = Reference;
  using Pointer = std::conditional_t<IsConst, typename ForwardList::ConstPointer, typename ForwardList::Pointer>;
  using pointer = Pointer;
  using DifferenceType = ForwardList::DifferenceType;
  using difference_type = ForwardList::DifferenceType;
  using IteratorCategory = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;

  constexpr ForwardListIteratorBase() noexcept = default;

 private:
  constexpr ForwardListIteratorBase(NodePointer node) noexcept : current_{node} { }

  template<bool IsConstLhs, bool IsConstRhs>
  static constexpr auto Bind(
    ForwardListIteratorBase<IsConstLhs, ForwardList> a,  //
    ForwardListIteratorBase<IsConstRhs, ForwardList>& b
  ) noexcept -> void {
    if (a.current_ != nullptr) {
      b.current_->next_ = a.current_->next_;
      a.current_->next_ = b.current_;
      return;
    }
    a.current_->next_ = b.current_;
  }

 public:
  constexpr ForwardListIteratorBase(const ForwardListIteratorBase<!IsConst, ForwardList> other) noexcept
    : current_{other.current_} { }

  constexpr auto operator*() const noexcept -> Reference {
    assert(current_);
    return current_->value_;
  }

  constexpr auto operator->() const noexcept -> Pointer {
    assert(current_);
    return &current_->value_;
  }

  constexpr auto operator++() noexcept -> ForwardListIteratorBase& {
    assert(current_);
    current_ = current_->next_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> ForwardListIteratorBase {
    assert(current_);
    auto temp{*this};
    current_ = current_->next_;
    return temp;
  }

  [[nodiscard]] friend constexpr auto operator==(
    const ForwardListIteratorBase lhs,  //
    const ForwardListIteratorBase rhs
  ) noexcept -> bool {
    return lhs.current_ == rhs.current_;
  }

  [[nodiscard]] friend constexpr auto operator!=(
    const ForwardListIteratorBase lhs,  //
    const ForwardListIteratorBase rhs
  ) noexcept -> bool {
    return lhs.current_ != rhs.current_;
  }

 private:
  NodePointer current_{nullptr};
};

START_EXPORT_SECTION

/**
 * @brief Namespace for Containers laboratory work
 * @namespace lab::containers
 */
namespace lab::containers {

/**
 * @brief Class that represents singly-linked list
 * @class
 *
 * @tparam T Value type to store in container
 * @tparam Allocator Allocator type to use in container
 *
 * @note `ForwardList` accepts stateless allocator types.
 */
template<IsValidForwardListType T, IsValidForwardListAllocatorType<T> Allocator = std::allocator<T>>
class [[nodiscard]] ForwardList {
  friend ForwardListIteratorBase<true, ForwardList<T, Allocator>>;
  friend ForwardListIteratorBase<false, ForwardList<T, Allocator>>;
  using InternalAllocatorType = std::allocator_traits<Allocator>::template rebind_alloc<ForwardListNode<T>>;
  using AllocatorTraits = std::allocator_traits<InternalAllocatorType>;
  using NodePointer = ForwardListNode<T>*;

 public:
  using ValueType = T;
  using value_type = T;
  using Reference = ValueType&;
  using reference = value_type&;
  using ConstReference = const ValueType&;
  using const_reference = const value_type&;
  using Pointer = ValueType*;
  using pointer = value_type*;
  using ConstPointer = const Pointer;
  using const_pointer = const pointer;
  using AllocatorType = InternalAllocatorType;
  using allocator_type = InternalAllocatorType;
  using SizeType = AllocatorTraits::size_type;
  using size_type = AllocatorTraits::size_type;
  using DifferenceType = AllocatorTraits::difference_type;
  using difference_type = AllocatorTraits::difference_type;
  using Iterator = ForwardListIteratorBase<false, ForwardList<T, Allocator>>;
  using iterator = Iterator;
  using ConstIterator = ForwardListIteratorBase<true, ForwardList<T, Allocator>>;
  using const_iterator = ConstIterator;

  /**
   * @brief Default constructor for `ForwardList`.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @details This constructor is defaulted.
   */
  constexpr ForwardList() noexcept = default;

  /**
   * @brief Constructs `ForwardList` with allocator by deducing it's type.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @details This constructor ignores `allocator` and only deduces it's type.
   */
  constexpr explicit ForwardList([[maybe_unused]] const Allocator& allocator) noexcept { }

  /**
   * @brief Copy constructor for `ForwardList`.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   */
  ForwardList(const ForwardList& other)
    : ForwardList{
        other.cbegin(),  //
        other.cend(),
        AllocatorTraits::select_on_container_copy_construction(other.allocator_)
      } { }

  /**
   * @brief Move constructor for `ForwardList`.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  constexpr ForwardList(ForwardList&& other) noexcept
    : head_{std::exchange(other.head_, nullptr)}  //
    , allocator_{std::move(other.allocator_)} { }

  /**
   * @brief Parametrisized constructor for `count` default constructed elements for `ForwardList`.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   *
   * @details This constructor ignores `allocator` and only deduces it's type.
   */
  constexpr explicit ForwardList(
    SizeType count,  //
    [[maybe_unused]] const Allocator& allocator = Allocator{}
  ) { }

  /**
   * @brief Parametrisized constructor `Range` like types for `ForwardList`.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   */
  template<std::input_iterator InputIterator>
  constexpr ForwardList(
    InputIterator first,  //
    InputIterator last,
    [[maybe_unused]] const Allocator& allocator = Allocator{}
  ) {
    if (first == last) {
      return;
    }

    head_ = ConstructNode(*first++);
    NodePointer traverser{head_};

    while (first != last) {
      LAB_TRY {
        NodePointer temp{ConstructNode(*first++)};
        traverser->next_ = temp;
        traverser = temp;
      }
      LAB_CATCH(...) {
        Clear();
        LAB_PROPAGATE_EXCEPTION;
      }
    }
  }

  /**
   * @brief Parametrisized constructor with `std::initializer_list` for `ForwardList`.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   *
   * @details This constructor ignores `allocator` and only deduces it's type.
   */
  constexpr explicit ForwardList(
    std::initializer_list<ValueType> ilist,  //
    [[maybe_unused]] const Allocator& allocator = Allocator{}
  )
    : ForwardList{ilist.begin(), ilist.end(), allocator} { }

  /**
   * @brief Destructor for `ForwardList`.
   * @public
   * @internal
   *
   * @details Delegates node sequence destruction to `DeleteRange`.
   * @see DeleteRange
   */
  constexpr ~ForwardList() { DeleteRange(); }

  /**
   * @brief Returns `Iterator` to the beginning of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto begin() noexcept -> Iterator { return {head_}; }

  /**
   * @brief Returns `Iterator` to the end of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto end() noexcept -> Iterator { return {}; }

  /**
   * @brief Returns `ConstIterator` to the beginning of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto cbegin() const noexcept -> ConstIterator { return {head_}; }

  /**
   * @brief Returns `ConstIterator` to the end of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  [[nodiscard]] constexpr auto cend() const noexcept -> ConstIterator { return {}; }

  /**
   * @brief Provides access to the first element of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @return `Reference` to the first element.
   */
  [[nodiscard]] constexpr auto Front() noexcept -> Reference {
    assert(head_);
    return head_->value_;
  }

  /**
   * @brief Provides access to the first element of the sequence (const overload).
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @return `ConstReference` to the first element.
   *
   * @warning **Undefined Behaviour** if:
   *   - `Empty() == true`
   * @see Empty
   */
  [[nodiscard]] constexpr auto Front() const noexcept -> ConstReference {
    assert(head_);
    return head_->value_;
  }

  /**
   * @brief Provides the ability to check underlying container state.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @return `true` if container is empty, `false` otherwise.
   */
  [[nodiscard]] constexpr auto Empty() const noexcept -> bool { return !head_; }

  /**
   * @brief Provides the access of allocator type used by the contianer.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @return `AllocatorType` Underlying allocator.
   */
  [[nodiscard]] constexpr auto GetAllocator() const noexcept -> AllocatorType { return allocator_; }

  /**
   * @brief Returns theoretical maximum size of the container.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @return `SizeType` The maximum amount of elements.
   */
  [[nodiscard]] constexpr auto MaxSize() const noexcept -> SizeType { return AllocatorTraits::max_size(allocator_); }

  /**
   * @brief Destroys the sequence obtained by the container.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  constexpr auto Clear() noexcept -> void { DeleteRange(); }

 private:
  /**
   * @brief Helper method for Node construction.
   * @private
   * @internal
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   *
   * @return `NodePointer` Pointer to the new constructed node.
   *
   * @details Delegates allocation/construct and deallocation/destroy operations to `std::allocator_traits`.
   */
  constexpr auto ConstructNode(auto&&... args) -> NodePointer {
    NodePointer temp{AllocatorTraits::allocate(allocator_, 1)};
    LAB_TRY { AllocatorTraits::construct(allocator_, temp, std::forward<decltype(args)>(args)...); }
    LAB_CATCH(...) { AllocatorTraits::deallocate(allocator_, temp, 1); }
    return temp;
  }

  constexpr auto SetHead(NodePointer node) noexcept -> void {
    assert(node);
    node->next_ = head_;
    head_ = node;
  }

 public:
  /**
   * @brief Copy constructs the element at the beginning of the sequence.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   */
  constexpr auto PushFront(const ValueType& value) -> void {
    NodePointer temp{ConstructNode(value)};
    SetHead(temp);
  }

  /**
   * @brief Move constructs the element at the beginning of the sequence.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   */
  constexpr auto PushFront(ValueType&& value) -> void {
    NodePointer temp{ConstructNode(std::move(value))};
    SetHead(temp);
  }

  /**
   * @brief Constructs the object at the beginning of the sequence with `args`.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails (std::allocator) or propagates user defined exception.
   */
  constexpr auto EmplaceFront(auto&&... args) -> void {
    NodePointer temp{ConstructNode(std::forward<decltype(args)>(args)...)};
    SetHead(temp);
  }

 private:
  /**
   * @brief Inserts the element at the beginning of the sequence.
   * @private
   * @internal
   *
   * @details Delegates pointer binding to the `ConstIterator::Bind`
   */
  constexpr auto InsertAfterImpl(ConstIterator position, auto&& value) -> Iterator {
    Iterator temp{ConstructNode(std::forward<decltype(value)>(value))};
    if (position != cend()) {
      ConstIterator::Bind(position, temp);
      return temp;
    }
    head_ = temp.current_;
    return temp;
  }

 public:
  /**
   * @brief Inserts the element at the beginning of the sequence by copy constructing.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails or propagates user defined exception.
   */
  constexpr auto InsertAfter(ConstIterator position, const ValueType& value) -> Iterator {
    return InsertAfterImpl(position, value);
  }

  /**
   * @brief Inserts the element at the beginning of the sequence by move constructing.
   * @public
   *
   * @throws `std::bad_alloc` if memory allocation fails or propagates user defined exception.
   */
  constexpr auto InsertAfter(ConstIterator position, ValueType&& value) -> Iterator {
    return InsertAfterImpl(position, std::move(value));
  }

 private:
  /**
   * @brief Helper function for node destruction.
   * @private
   * @internal
   *
   * @details Delegates deallocate/destroy to `std::allocator_traits`.
   */
  constexpr auto DestroyNode(NodePointer node) -> void {
    assert(node);
    if constexpr (!std::is_fundamental_v<ValueType>) {
      AllocatorTraits::destroy(allocator_, node);
    }
    AllocatorTraits::deallocate(allocator_, node, 1);
  }

 public:
  /**
   * @brief Erases element at the specified position.
   * @public
   *
   * @throws None (no-throw guarantee).
   *
   * @warning **Undefined Behaviour** if:
   *   - `position` argument is not in range [`cbegin()`, `cend()`)
   * @see cbegin, cend
   */
  constexpr auto Erase(ConstIterator position) noexcept -> void {
    assert(head_);
    if (position == cbegin()) {
      DestroyNodeFront();
      return;
    }
    NodePointer traverser{head_};
    while (traverser->next_ != position.current_) {
      traverser = traverser->next_;
    }
    traverser->next_ = position.current_->next_;
    DestroyNode(position.current_);
  }

  /**
   * @brief Provides the ability to swap `ForwardList` instances.
   * @public
   *
   * @throws May throw exception if user defined allocator throws when swapping.
   */
  constexpr auto Swap(ForwardList& other) noexcept(AllocatorTraits::is_always_equal::value) -> void {
    assert(this != &other);
    if constexpr (AllocatorTraits::propagate_on_container_swap::value) {
      std::swap(allocator_, other.allocator_);
    }
    std::swap(head_, other.head_);
  }

 private:
  /**
   * @brief Helper function for destructing node at the beginning.
   * @private
   * @internal
   *
   * @details Delegates to `DestroyNode` helper function.
   * @see DestroyNode
   */
  constexpr auto DestroyNodeFront() -> void {
    assert(head_);
    NodePointer temp{head_};
    head_ = head_->next_;
    DestroyNode(temp);
  }

 public:
  /**
   * @brief Destroys element from the beginning of the sequence.
   * @public
   *
   * @throws None (no-throw guarantee).
   */
  constexpr auto PopFront() noexcept -> void { DestroyNodeFront(); }

 private:
  constexpr auto DeleteRange() -> void {
    while (head_) {
      DestroyNodeFront();
    }
  }

 public:
  auto operator=(const ForwardList& other) -> ForwardList& {
    assert(this != &other);
    auto temp{other};
    return *this = std::move(temp);
  }

  constexpr auto operator=(ForwardList&& other) noexcept -> ForwardList& {
    assert(this != &other);
    DeleteRange();
    std::swap(head_, other.head_);
    if constexpr (AllocatorTraits::propagate_on_container_move_assignment::value) {
      allocator_ = std::move(other.allocator_);
    }
    return *this;
  }

 private:
  NodePointer head_{nullptr};
  [[no_unique_address]] AllocatorType allocator_;
};

}  // namespace lab::containers

END_EXPORT_SECTION
