#ifndef STAN_MATH_PRIM_FUN_ACOS_HPP
#define STAN_MATH_PRIM_FUN_ACOS_HPP

#include <stan/math/prim/core.hpp>
#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/fun/asin.hpp>
#include <stan/math/prim/fun/constants.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/isinf.hpp>
#include <stan/math/prim/fun/isfinite.hpp>
#include <stan/math/prim/fun/isnan.hpp>
#include <stan/math/prim/fun/polar.hpp>
#include <stan/math/prim/functor/apply_scalar_unary.hpp>
#include <stan/math/prim/functor/apply_vector_unary.hpp>
#include <cmath>
#include <complex>

namespace stan {
namespace math {

/**
 * Return the arc cosine of the arithmetic argument.
 *
 * @tparam V An `Arithmetic` argument
 * @param[in] x argument
 * @return arc cosine of the argument
 */
template <typename T, require_arithmetic_t<T>* = nullptr>
inline auto acos(const T x) {
  return std::acos(x);
}

/**
 * Return the arc cosine of the complex argument.
 *
 * @tparam V `complex<Arithmetic>` argument
 * @param[in] x argument
 * @return arc cosine of the argument
 */
template <typename T, require_complex_bt<std::is_arithmetic, T>* = nullptr>
inline auto acos(const T x) {
  return std::acos(x);
}

/**
 * Structure to wrap `acos()` so it can be vectorized.
 *
 * @tparam T type of variable
 * @param x argument
 * @return Arc cosine of variable in radians.
 */
struct acos_fun {
  template <typename T>
  static inline auto fun(const T& x) {
    return acos(x);
  }
};

/**
 * Returns the elementwise `acos()` of the input,
 * which may be a scalar or any Stan container of numeric scalars.
 *
 * @tparam Container type of container
 * @param x argument
 * @return Arc cosine of each variable in the container, in radians.
 */
template <typename Container, require_ad_container_t<Container>* = nullptr>
inline auto acos(const Container& x) {
  return apply_scalar_unary<acos_fun, Container>::apply(x);
}

/**
 * Version of `acos()` that accepts std::vectors, Eigen Matrix/Array objects
 *  or expressions, and containers of these.
 *
 * @tparam Container Type of x
 * @param x argument
 * @return Arc cosine of each variable in the container, in radians.
 */
template <typename Container,
          require_container_bt<std::is_arithmetic, Container>* = nullptr>
inline auto acos(const Container& x) {
  return apply_vector_unary<Container>::apply(
      x, [](const auto& v) { return v.array().acos(); });
}

namespace internal {
/**
 * Return the arc cosine of the complex argument.
 *
 * @tparam V value type of argument
 * @param[in] x argument
 * @return arc cosine of the argument
 */
template <typename V>
inline std::complex<V> complex_acos(const std::complex<V>& x) {
  return 0.5 * pi() - asin(x);
}
}  // namespace internal

}  // namespace math
}  // namespace stan

#endif
