#ifndef STAN_MATH_PRIM_FUN_GRAD_PFQ_HPP
#define STAN_MATH_PRIM_FUN_GRAD_PFQ_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/fun/hypergeometric_pFq.hpp>
#include <stan/math/prim/fun/prod.hpp>
#include <stan/math/prim/fun/log.hpp>
#include <stan/math/prim/fun/log1p.hpp>
#include <stan/math/prim/fun/abs.hpp>
#include <stan/math/prim/fun/max.hpp>
#include <stan/math/prim/fun/select.hpp>
#include <stan/math/prim/fun/as_column_vector_or_scalar.hpp>
#include <stan/math/prim/fun/value_of_rec.hpp>

namespace stan {
namespace math {
namespace internal {
template <typename T>
inline auto binarysign(const T& x) {
  return select(x == 0.0, 1.0, sign(value_of_rec(x)));
}
}  // namespace internal

/**
 * Returns the gradient of generalized hypergeometric function wrt to the
 * input arguments:
 * \f$ _pF_q(a_1,...,a_p;b_1,...,b_q;z) \f$
 *
 * @tparam CalcA Boolean for whether to calculate derivatives wrt to 'a'
 * @tparam CalcB Boolean for whether to calculate derivatives wrt to 'b'
 * @tparam CalcZ Boolean for whether to calculate derivatives wrt to 'z'
 * @tparam TpFq Scalar type of hypergeometric_pFq return value
 * @tparam Ta Eigen type with either one row or column at compile time
 * @tparam Tb Eigen type with either one row or column at compile time
 * @tparam Tz Scalar type
 * @param[in] pfq_val Return value from the hypergeometric_pFq function
 * @param[in] a Vector of 'a' arguments to function
 * @param[in] b Vector of 'b' arguments to function
 * @param[in] z Scalar z argument
 * @param[in] precision Convergence criteria for infinite sum
 * @param[in] max_steps Maximum number of iterations
 * @return Tuple of gradients
 */
template <bool CalcA = true, bool CalcB = true, bool CalcZ = true,
          typename TpFq, typename Ta, typename Tb, typename Tz>
auto grad_pFq(const TpFq& pfq_val, const Ta& a, const Tb& b, const Tz& z,
              double precision = 1e-14, int max_steps = 1e6) {
  using std::max;
  using T_Rtn = return_type_t<Ta, Tb, Tz>;
  using Ta_Array = Eigen::Array<return_type_t<Ta>, -1, 1>;
  using Tb_Array = Eigen::Array<return_type_t<Tb>, -1, 1>;

  Ta_Array a_array = as_column_vector_or_scalar(a).array();
  Ta_Array a_k = a_array;
  Tb_Array b_array = as_column_vector_or_scalar(b).array();
  Tb_Array b_k = b_array;
  Tz log_z = log(abs(z));
  int z_sign = internal::binarysign(z);

  Ta_Array digamma_a = inv(a_k);
  Tb_Array digamma_b = inv(b_k);

  std::tuple<promote_scalar_t<T_Rtn, plain_type_t<Ta>>,
             promote_scalar_t<T_Rtn, plain_type_t<Tb>>, T_Rtn>
      ret_tuple;
  std::get<0>(ret_tuple).setConstant(a.size(), 0.0);
  std::get<1>(ret_tuple).setConstant(b.size(), 0.0);
  std::get<2>(ret_tuple) = 0.0;

  if (CalcA || CalcB) {
    Eigen::Array<T_Rtn, -1, 1> a_grad(a.size());
    Eigen::Array<T_Rtn, -1, 1> b_grad(b.size());

    int k = 0;
    int base_sign = 1;
    T_Rtn curr_log_prec = NEGATIVE_INFTY;
    T_Rtn log_base = 0;
    while ((k < 10 || curr_log_prec > log(precision)) && (k <= max_steps)) {
      curr_log_prec = NEGATIVE_INFTY;
      if (CalcA) {
        a_grad = log(select(digamma_a == 0.0, 1.0, abs(digamma_a))) + log_base;
        std::get<0>(ret_tuple).array()
            += exp(a_grad) * base_sign * internal::binarysign(digamma_a);

        curr_log_prec = max(curr_log_prec, a_grad.maxCoeff());
        digamma_a += select(a_k == 0.0, 0.0, inv(a_k));
      }

      if (CalcB) {
        b_grad = log(select(digamma_b == 0.0, 1.0, abs(digamma_b))) + log_base;
        std::get<1>(ret_tuple).array()
            -= exp(b_grad) * base_sign * internal::binarysign(digamma_b);

        curr_log_prec = max(curr_log_prec, b_grad.maxCoeff());
        digamma_b += select(b_k == 0.0, 0.0, inv(b_k));
      }

      log_base += (sum(log(select(a_k == 0.0, 0.0, abs(a_k)))) + log_z)
                  - (sum(log(select(b_k == 0.0, 0.0, abs(b_k)))) + log1p(k));
      base_sign *= z_sign * internal::binarysign(a_k).prod()
                   * internal::binarysign(b_k).prod();

      a_k += 1.0;
      b_k += 1.0;
      k += 1;
    }

    if (CalcA) {
      std::get<0>(ret_tuple).array() -= pfq_val / a_array;
    }
    if (CalcB) {
      std::get<1>(ret_tuple).array() += pfq_val / b_array;
    }
  }
  if (CalcZ) {
    T_Rtn pfq_p1_val
        = hypergeometric_pFq((a_array + 1).matrix(), (b_array + 1).matrix(), z);
    std::get<2>(ret_tuple) = prod(a) / prod(b) * pfq_p1_val;
  }
  return ret_tuple;
}

}  // namespace math
}  // namespace stan
#endif
