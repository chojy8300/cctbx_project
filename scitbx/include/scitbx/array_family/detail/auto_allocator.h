#ifndef SCITBX_ARRAY_FAMILY_AUTO_ALLOCATOR_H
#define SCITBX_ARRAY_FAMILY_AUTO_ALLOCATOR_H

#if !(defined(SCITBX_ARRAY_FAMILY_USE_CUSTOM_ALIGNMENT_CALCULATOR))

#include <boost/type_traits/alignment_traits.hpp>

namespace scitbx { namespace af { namespace detail {

  template <typename T, std::size_t N>
  union auto_allocator
  {
    typedef typename boost::type_with_alignment<
      (boost::alignment_of<T>::value)>::type
        align_t;
    align_t dummy_;
    char buffer[N * sizeof(T)];
  };

}}} // namespace scitbx::af::detail

#else

#include <cstddef>
#include <boost/config.hpp>

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

#include <scitbx/array_family/detail/alignment_calculator.h>

namespace scitbx { namespace af { namespace detail {

  template <typename T, std::size_t N>
  union auto_allocator {
    char buffer[N * sizeof(T)];
    typename scitbx::af::alignment_calculator::AlignmentCalculator<
      SCITBX_AF_TYPELIST1(T)>::Result dummy_;
  };

}}} // namespace scitbx::af::detail

#else // !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace scitbx { namespace af { namespace detail {

  template <typename T>
  struct SimpleAlignmentCalculator { typedef long double Result; };

#define SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(T) \
template <> struct SimpleAlignmentCalculator<T> { typedef T Result; };

  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(char)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(signed char)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(unsigned char)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(short int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(unsigned short int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(unsigned int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(long int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(unsigned long int)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(float)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(double)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(long double)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(char*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(short int*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(int*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(long int*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(float*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(double*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(long double*)
  SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR(void*)

#undef SCITBX_ARRAY_FAMILY_DEFINE_SIMPLE_ALIGNMENT_CALCULATOR

  template <typename T, std::size_t N>
  union auto_allocator {
    char buffer[N * sizeof(T)];
    typename SimpleAlignmentCalculator<T>::Result dummy_;
  };

}}} // namespace scitbx::af::detail

#endif // !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

#endif // !(defined(SCITBX_ARRAY_FAMILY_USE_CUSTOM_ALIGNMENT_CALCULATOR))

#endif // SCITBX_ARRAY_FAMILY_AUTO_ALLOCATOR_H
