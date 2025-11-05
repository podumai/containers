module;

#include <algorithm>
#include <concepts>
#include <exception>
#include <format>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

export module lab_vector;

template<typename T>
concept IsValidVectorValueType = (std::copyable<T> || std::movable<T>) && std::is_nothrow_destructible_v<T>;

template<typename Allocator, typename T>
concept IsValidVectorAllocatorType = IsValidVectorValueType<typename std::allocator_traits<Allocator>::value_type> &&
                                     std::same_as<typename std::allocator_traits<Allocator>::value_type, T>;

// clang-format off
#if __cplusplus > 202302L
  #define LAB_CXX26_CONSTEXPR constexpr
#else
  #define LAB_CXX26_CONSTEXPR
#endif
// clang-format on

#ifdef __cpp_exceptions
  #define LAB_TRY_BEGIN \
    try                 \
    {
  #define LAB_TRY_END }
  #define LAB_CATCH_BEGIN(exception) \
    catch (exception)                \
    {
  #define LAB_CATCH_END }
  #define LAB_PROPAGATE_EXCEPTION
#else
  #define LAB_TRY_BEGIN \
    if (true)           \
    {
  #define LAB_TRY_END }
  #define LAB_CATCH_BEGIN(exception) \
    if (false)                       \
    {
  #define LAB_CATCH_END }
  #define LAB_PROPAGATE_EXCEPTION throw;
#endif

template<IsValidVectorValueType T, IsValidVectorAllocatorType<T> Allocator>
class VectorBase
{
 public:
  using AllocatorType = Allocator;
  using AllocatorTraits = std::allocator_traits<AllocatorType>;
  using ValueType = T;
  using Reference = ValueType&;
  using ConstReference = const ValueType&;
  using Pointer = AllocatorTraits::pointer;
  using ConstPointer = AllocatorTraits::const_pointer;
  using SizeType = AllocatorTraits::size_type;
  using DifferenceType = AllocatorTraits::difference_type;

 protected:
  template<std::input_iterator InputIterator>
  static auto UninitializedCopyUsingAllocator(
    InputIterator first_s,  //
    InputIterator last_s,
    Pointer first_d,
    AllocatorType& allocator
  ) -> void
  {
    Pointer temp{first_d};
    LAB_TRY_BEGIN
    while (first_s != last_s)
    {
      AllocatorTraits::construct(allocator, first_d, *first_s);
      ++first_s;
      ++first_d;
    }
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] const std::exception& /* error */)
    while (temp != first_d)
    {
      AllocatorTraits::destroy(allocator, temp);
      ++temp;
    }
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END
  }

  template<std::input_iterator InputIterator>
  static auto UninitializedMoveUsingAllocator(
    InputIterator first_s,  //
    InputIterator last_s,
    Pointer first_d,
    Allocator& allocator
  ) -> void
  {
    Pointer temp{first_d};
    LAB_TRY_BEGIN
    while (first_s != last_s)
    {
      AllocatorTraits::construct(allocator, first_d, std::move(*first_s));
      ++first_s;
      ++first_d;
    }
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] const std::exception& /* error */)
    while (temp != first_d)
    {
      AllocatorTraits::destroy(allocator, temp);
      ++temp;
    }
    LAB_CATCH_END
  }

  template<std::input_iterator InputIterator, typename... Args>
  static auto UninitializedConstructUsingAllocator(
    InputIterator first,  //
    InputIterator last,
    AllocatorType& allocator,
    Args&&... args
  ) -> void
  {
    InputIterator temp{first};
    LAB_TRY_BEGIN
    while (first != last)
    {
      AllocatorTraits::construct(allocator, first, std::forward<Args>(args)...);
      ++first;
    }
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] const std::exception& /* error */)
    while (temp != first)
    {
      AllocatorTraits::destroy(allocator, temp);
      ++temp;
    }
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END
  }

  static auto DestroyUsingAllocator(
    Pointer first,  //
    Pointer last,
    AllocatorType& allocator
  ) -> void
  {
    while (first != last)
    {
      AllocatorTraits::destroy(allocator, first);
      ++first;
    }
  }
};

#define BEGIN_EXPORT_SECTION export {
#define END_EXPORT_SECTION }

BEGIN_EXPORT_SECTION

namespace lab
{

template<typename T, typename Allocator = std::allocator<T>>
class [[nodiscard]] Vector : protected VectorBase<T, Allocator>
{
 protected:
  using Base = VectorBase<T, Allocator>;
  using AllocatorTraits = Base::AllocatorTraits;

 public:
  using ValueType = Base::ValueType;
  using value_type = Base::ValueType;
  using Reference = Base::Reference;
  using reference = Base::Reference;
  using ConstReference = Base::ConstReference;
  using const_reference = Base::ConstReference;
  using Pointer = Base::Pointer;
  using pointer = Base::Pointer;
  using ConstPointer = Base::ConstPointer;
  using const_pointer = Base::ConstPointer;
  using DifferenceType = Base::DifferenceType;
  using difference_type = Base::DifferenceType;
  using SizeType = Base::SizeType;
  using size_type = Base::SizeType;
  using AllocatorType = Base::AllocatorType;
  using allocator_type = Base::AllocatorType;
  using Iterator = Pointer;
  using iterator = pointer;
  using ConstIterator = ConstPointer;
  using const_iterator = const_pointer;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
  using const_reverse_iterator = std::reverse_iterator<const_pointer>;

  LAB_CXX26_CONSTEXPR Vector() noexcept(std::is_nothrow_default_constructible_v<AllocatorType>) = default;

  explicit LAB_CXX26_CONSTEXPR Vector(
    const AllocatorType& allocator
  ) noexcept
    : allocator_{allocator}
  { }

  LAB_CXX26_CONSTEXPR Vector(
    SizeType n,  //
    const AllocatorType& allocator = AllocatorType{}
  )
    : allocator_{allocator}
  {
    if (!n)
    {
      return;
    }

    first_ = AllocatorTraits::allocate(allocator_, n);

    LAB_TRY_BEGIN
    this->UninitializedConstructUsingAllocator(first_, first_ + n, allocator_);
    LAB_TRY_END
    LAB_CATCH_BEGIN(const std::exception& /* error */)
    AllocatorTraits::deallocate(allocator_, std::exchange(first_, nullptr), std::distance(first_, last_));
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END

    current_ = last_ = first_ + n;
  }

  constexpr Vector(
    const Vector& other
  )
    : allocator_{AllocatorTraits::select_on_container_copy_construction(other.allocator_)}
  {
    if (other.Empty())
    {
      return;
    }

    SizeType capacity{other.Capacity()};
    first_ = AllocatorTraits::allocate(allocator_, capacity);
    LAB_TRY_BEGIN
    this->UninitializedCopyUsingAllocator(other.cbegin(), other.cend(), first_, allocator_);
    LAB_TRY_END
    LAB_CATCH_BEGIN(const std::exception& /* error */)
    AllocatorTraits::deallocate(allocator_, std::exchange(first_, nullptr), capacity);
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END
    current_ = last_ = first_ + other.Size();
  }

  LAB_CXX26_CONSTEXPR Vector(
    const Vector& other,  //
    const AllocatorType& allocator
  )
    : Vector{allocator}
  {
    if (other.Empty())
    {
      return;
    }

    SizeType capacity{other.Capacity()};
    first_ = AllocatorTraits::allocate(allocator_, capacity);
    LAB_TRY_BEGIN
    UninitializedCopyUsingAllocator(other.cbegin(), other.cend(), first_, allocator_);
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] const std::exception& /* error */)
    AllocatorTraits::deallocate(allocator_, std::exchange(first_, nullptr), capacity);
    LAB_CATCH_END
    current_ = last_ = first_ + other.Size();
  }

  LAB_CXX26_CONSTEXPR Vector(
    Vector&& other
  ) noexcept
    : first_{std::exchange(other.first_, nullptr)}  //
    , current_{std::exchange(other.current_, nullptr)}
    , last_{std::exchange(other.last_, nullptr)}
    , allocator_{std::move(other.allocator_)}
  { }

  template<std::input_iterator InputIterator>
  LAB_CXX26_CONSTEXPR Vector(
    InputIterator first,  //
    InputIterator last,
    const AllocatorType& allocator = AllocatorType{}
  )
    : allocator_{allocator}
  {
    if (first == last)
    {
      return;
    }

    const SizeType size{static_cast<SizeType>(std::distance(first, last))};
    first_ = AllocatorTraits::allocate(allocator_, size);
    LAB_TRY_BEGIN
    this->UninitializedCopyUsingAllocator(first, last, first_, allocator_);
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] std::exception& /* error */)
    AllocatorTraits::deallocate(allocator_, first_, size);
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END
    current_ = last_ = first_ + size;
  }

  LAB_CXX26_CONSTEXPR Vector(
    std::initializer_list<ValueType> ilist,  //
    const AllocatorType& allocator = AllocatorType{}
  )
    : Vector{ilist.begin(), ilist.end(), allocator}
  { }

  LAB_CXX26_CONSTEXPR ~Vector()
  {
    if (first_)
    {
      if constexpr (!std::is_fundamental_v<ValueType>)
      {
        this->DestroyUsingAllocator(first_, current_, allocator_);
      }
      AllocatorTraits::deallocate(allocator_, first_, Capacity());
      first_ = current_ = last_ = nullptr;
    }
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto begin() noexcept -> Iterator
  {
    return first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto end() noexcept -> Iterator
  {
    return current_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto cbegin() const noexcept -> ConstIterator
  {
    return first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto cend() const noexcept -> ConstIterator
  {
    return current_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto rbegin() noexcept -> ReverseIterator
  {
    return {current_};
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto rend() noexcept -> ReverseIterator
  {
    return {first_};
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto crbegin() const noexcept -> ConstReverseIterator
  {
    return {current_};
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto crend() const noexcept -> ConstReverseIterator
  {
    return {first_};
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Size() const noexcept -> SizeType
  {
    return std::distance(first_, current_);
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Capacity() const noexcept -> SizeType
  {
    return std::distance(first_, last_);
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Front() noexcept -> Reference
  {
    return *first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Front() const noexcept -> ConstReference
  {
    return *first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Back() noexcept -> Reference
  {
    return *std::prev(current_);
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Back() const noexcept -> ConstReference
  {
    return *std::prev(current_);
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto operator[](
    SizeType index
  ) noexcept -> Reference
  {
    return first_[index];
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto operator[](
    SizeType index
  ) const noexcept -> ConstReference
  {
    return first_[index];
  }

 private:
  auto RangeCheck(
    SizeType index
  ) const -> void
  {
    if (first_ == nullptr || first_ + index >= current_)
    {
      throw std::out_of_range{
        std::format("Vector::RangeCheck: index (which is {}) >= this->size() (which is {})", index, Size())
      };
    }
  }

 public:
  [[nodiscard]] LAB_CXX26_CONSTEXPR auto At(
    SizeType index
  ) -> Reference
  {
    RangeCheck(index);
    return first_[index];
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto At(
    SizeType index
  ) const -> ConstReference
  {
    RangeCheck(index);
    return first_[index];
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Data() noexcept -> Pointer
  {
    return first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Data() const noexcept -> ConstPointer
  {
    return first_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto Empty() const noexcept -> bool
  {
    return first_ == current_;
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto MaxSize() const noexcept -> SizeType
  {
    return AllocatorTraits::max_size(allocator_);
  }

  [[nodiscard]] LAB_CXX26_CONSTEXPR auto GetAllocator() const noexcept -> AllocatorType
  {
    return allocator_;
  }

  LAB_CXX26_CONSTEXPR auto ShrinkToFit() -> void
  {
    if (first_ == current_ || current_ == last_)
    {
      return;
    }

    SizeType new_size{Size()};
    Pointer new_first{AllocatorTraits::allocate(allocator_, new_size)};

    LAB_TRY_BEGIN
    if constexpr (std::is_nothrow_move_constructible_v<ValueType>)
    {
      UninitializedMoveUsingAllocator(first_, current_, new_first, allocator_);
    }
    else
    {
      UninitializedCopyUsingAllocator(first_, current_, new_first, allocator_);
    }
    LAB_TRY_END
    LAB_CATCH_BEGIN([[maybe_unused]] std::exception& /* error */)
    AllocatorTraits::deallocate(allocator_, new_first, new_size);
    LAB_PROPAGATE_EXCEPTION
    LAB_CATCH_END

    if constexpr (!std::is_fundamental_v<ValueType>)
    {
      DestroyUsingAllocator(first_, current_, allocator_);
    }
    AllocatorTraits::deallocate(allocator_, first_, Capacity());
    first_ = new_first;
    current_ = last_ = new_first + new_size;
  }

  LAB_CXX26_CONSTEXPR auto Clear() -> void
  {
    if (first_ == last_)
    {
      return;
    }

    if constexpr (!std::is_fundamental_v<ValueType>)
    {
      std::destroy(first_, current_);
    }
    AllocatorTraits::deallocate(allocator_, first_, Size());
  }

 private:
  [[nodiscard]] LAB_CXX26_CONSTEXPR auto ResizeFactor() const noexcept -> bool
  {
    return current_ == last_;
  }

  LAB_CXX26_CONSTEXPR auto ResizeImpl() -> void
  {
    SizeType current_capacity{Capacity()};
    SizeType new_capacity{current_capacity + (current_capacity >> 1) + 2};
    Pointer new_first{AllocatorTraits::allocate(allocator_, new_capacity)};

    if constexpr (std::is_nothrow_move_constructible_v<ValueType>)
    {
      std::uninitialized_move(first_, current_, new_first);
    }
    else
    {
      try
      {
        std::uninitialized_copy(first_, current_, new_first);
      }
      catch ([[maybe_unused]] const std::exception& /* error */)
      {
        AllocatorTraits::deallocate(allocator_, new_first, new_capacity);
        throw;
      }
    }

    if constexpr (std::is_fundamental_v<ValueType>)
    {
      std::destroy(first_, current_);
    }
    AllocatorTraits::deallocate(allocator_, first_, current_capacity);
    first_ = new_first;
    current_ = new_first + current_capacity;
    last_ = new_first + new_capacity;
  }

 public:
  LAB_CXX26_CONSTEXPR auto PushBack(
    const ValueType& value
  ) -> void
  {
    if (ResizeFactor())
    {
      ResizeImpl();
    }
    AllocatorTraits::construct(allocator_, current_, value);
    ++current_;
  }

  LAB_CXX26_CONSTEXPR auto PushBack(
    ValueType&& value
  ) -> void
  {
    if (ResizeFactor())
    {
      ResizeImpl();
    }
    AllocatorTraits::construct(allocator_, current_, std::move(value));
    ++current_;
  }

  template<typename... Args>
  LAB_CXX26_CONSTEXPR auto EmplaceBack(
    Args&&... args
  ) -> void
  {
    if (ResizeFactor())
    {
      ResizeImpl();
    }
    AllocatorTraits::construct(allocator_, current_, std::forward<Args>(args)...);
    ++current_;
  }

  LAB_CXX26_CONSTEXPR auto PopBack() noexcept -> void
  {
    AllocatorTraits::destroy(allocator_, current_ - 1);
    --current_;
  }

  LAB_CXX26_CONSTEXPR auto Swap(
    Vector& other
  ) noexcept(std::is_nothrow_swappable_v<AllocatorType>) -> void
  {
    std::swap(first_, other.first_);
    std::swap(current_, other.current_);
    std::swap(last_, other.last_);
    if constexpr (AllocatorTraits::propogate_on_container_swap::value)
    {
      std::swap(allocator_, other.allocator_);
    }
  }

  auto operator=(
    const Vector& other
  ) -> Vector&
  {
    assert(this != &other);

    return *this;
  }

  auto operator=(
    Vector&& other
  ) noexcept -> Vector&
  {
    assert(this != &other);

    Clear();
    std::swap(first_, other.first_);
    std::swap(current_, other.current_);
    std::swap(last_, other.last_);
    if constexpr (AllocatorTraits::on_container_move_assignment::value)
    {
      allocator_ = std::move(other.allocator_);
    }
    return *this;
  }

 private:
  Pointer first_{nullptr};
  Pointer current_{nullptr};
  Pointer last_{nullptr};
  [[no_unique_address]] AllocatorType allocator_{};
};

}  // namespace lab

END_EXPORT_SECTION
