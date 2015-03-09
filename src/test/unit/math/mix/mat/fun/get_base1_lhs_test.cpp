#include <stan/math/prim/mat/fun/get_base1_lhs.hpp>
#include <stan/math/mix/mat/fun/typedefs.hpp>
#include <stan/math/fwd/core.hpp>
#include <stan/math/rev/core.hpp>
#include <gtest/gtest.h>

using stan::agrad::fvar;
using stan::agrad::var;

TEST(AgradMixMatrixGetBase1LHS,failing_pre_20_fv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<var> ,Dynamic,1> y(3);
  y << 1, 2, 3;
  fvar<var>  z = get_base1_lhs(y,1,"y",1);
  EXPECT_FLOAT_EQ(1, z.val_.val());
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vec1_fv) {
  using stan::math::get_base1_lhs;
  std::vector<fvar<var> > x(2);
  x[0] = 10.0;
  x[1] = 20.0;
  EXPECT_FLOAT_EQ(10.0,get_base1_lhs(x,1,"x[1]",0).val_.val());
  EXPECT_FLOAT_EQ(20.0,get_base1_lhs(x,2,"x[1]",0).val_.val());
  
  get_base1_lhs(x,2,"x[2]",0) = 5.0;
  EXPECT_FLOAT_EQ(5.0,get_base1_lhs(x,2,"x[1]",0).val_.val());

  EXPECT_THROW(get_base1_lhs(x,0,"x[0]",0),
               std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,3,"x[3]",0),
               std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vec2_fv) {
  using stan::math::get_base1_lhs;
  using std::vector;
  size_t M = 3;
  size_t N = 4;

  vector<vector<fvar<var> > > x(M,vector<fvar<var> >(N,0.0));
  

  for (size_t m = 1; m <= M; ++m)
    for (size_t n = 1; n <= N; ++n)
      x[m - 1][n - 1] = (m * 10) + n;

  for (size_t m = 1; m <= M; ++m) {
    for (size_t n = 1; n <= N; ++n) {
      fvar<var>  expected = x[m - 1][n - 1];
      fvar<var>  found = get_base1_lhs(get_base1_lhs(x, m, "x[m]",1),
                               n, "x[m][n]",2);
      EXPECT_FLOAT_EQ(expected.val_.val(),found.val_.val());
    }
  }

  get_base1_lhs(get_base1_lhs(x,1,"",-1),2,"",-1) = 112.5;
  EXPECT_FLOAT_EQ(112.5, x[0][1].val_.val());

  EXPECT_THROW(get_base1_lhs(x,0,"",-1),std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,M+1,"",-1),std::out_of_range);
  
  EXPECT_THROW(get_base1_lhs(get_base1_lhs(x,1,"",-1), 
                         12,"",-1),
               std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_matrix_fv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<var> ,Dynamic,Dynamic> x(4,3);
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 3; ++j)
      x(i,j) = i * j;
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(x(i,j).val_.val(),
                      get_base1_lhs(x,i+1,j+1,"x",1).val_.val());
      EXPECT_FLOAT_EQ(x(i,j).val_.val(),
          get_base1_lhs(x,i+1,"x",1)(0,j).val_.val());
      Matrix<fvar<var> ,1,Dynamic> xi
        = get_base1_lhs<fvar<var> >(x,i+1,"x",1);
      EXPECT_FLOAT_EQ(x(i,j).val_.val(),xi[j].val_.val());
      EXPECT_FLOAT_EQ(x(i,j).val_.val(),get_base1_lhs(xi,j+1,"xi",2).val_.val());
      // this is no good because can't get ref to inside val
      // could remedy by adding const versions, but don't need for Stan GM
      // fvar<var>  xij = get_base1_lhs<fvar<var> >(get_base1_lhs<fvar<var> >(x,i+1,"x",1),
      //                                j+1,"xi",2);
    }
  }
  EXPECT_THROW(get_base1_lhs(x,10,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,1,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,1,100,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,0,1,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,1,0,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vector_fv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<var> ,1,Dynamic> x(3);
  x << 1, 2, 3;
  
  for (size_t i = 0; i < 3; ++i)
    EXPECT_FLOAT_EQ(x(i).val_.val(), get_base1_lhs(x,i+1,"x",1).val_.val());
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_row_vector_fv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<var> ,Dynamic,1> x(3);
  x << 1, 2, 3;
  
  for (size_t i = 0; i < 3; ++i)
    EXPECT_FLOAT_EQ(x(i).val_.val(), get_base1_lhs(x,i+1,"x",1).val_.val());
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_8_fv) {
  using stan::math::get_base1_lhs;
  using std::vector;
  fvar<var>  x0(42.0);
  // ~ 4m entries ~ 32MB memory + sizes
  vector<fvar<var> > x1(9,x0);
  vector<vector<fvar<var> > > x2(8,x1);
  vector<vector<vector<fvar<var> > > > x3(7,x2);
  vector<vector<vector<vector<fvar<var> > > > > x4(6,x3);
  vector<vector<vector<vector<vector<fvar<var> > > > > > x5(5,x4);
  vector<vector<vector<vector<vector<vector<fvar<var> > > > > > > x6(4,x5);
  vector<vector<vector<vector<vector<vector<vector<fvar<var> > > > > > > > x7(3,x6);
  vector<vector<vector<vector<vector<vector<vector<vector<fvar<var> > > > > > > > > x8(2,x7);

  EXPECT_EQ(x0.val_.val(), x8[0][0][0][0][0][0][0][0].val_.val());
  
  for (size_t i1 = 0; i1 < x8.size(); ++i1)
    for (size_t i2 = 0; i2 < x8[0].size(); ++i2)
      for (size_t i3 = 0; i3 < x8[0][0].size(); ++i3)
        for (size_t i4 = 0; i4 < x8[0][0][0].size(); ++i4)
          for (size_t i5 = 0; i5 < x8[0][0][0][0].size(); ++i5)
            for (size_t i6 = 0; i6 < x8[0][0][0][0][0].size(); ++i6)
              for (size_t i7 = 0; i7 < x8[0][0][0][0][0][0].size(); ++i7)
                for (size_t i8 = 0; i8 < x8[0][0][0][0][0][0][0].size(); ++i8)
                  x8[i1][i2][i3][i4][i5][i6][i7][i8]
                    = i1 * i2 * i3 * i4 * i5 * i6 * i7 * i8;

  for (size_t i1 = 0; i1 < x8.size(); ++i1)
    for (size_t i2 = 0; i2 < x8[0].size(); ++i2)
      for (size_t i3 = 0; i3 < x8[0][0].size(); ++i3)
        for (size_t i4 = 0; i4 < x8[0][0][0].size(); ++i4)
          for (size_t i5 = 0; i5 < x8[0][0][0][0].size(); ++i5)
            for (size_t i6 = 0; i6 < x8[0][0][0][0][0].size(); ++i6)
              for (size_t i7 = 0; i7 < x8[0][0][0][0][0][0].size(); ++i7)
                for (size_t i8 = 0; i8 < x8[0][0][0][0][0][0][0].size(); ++i8)
                  EXPECT_FLOAT_EQ(x8[i1][i2][i3][i4][i5][i6][i7][i8].val_.val(),
                                  get_base1_lhs(x8,i1+1,i2+1,i3+1,i4+1,i5+1,i6+1,i7+1,i8+1,
                                            "x8",1).val_.val());
}
TEST(AgradMixMatrixGetBase1LHS,failing_pre_20_ffv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<fvar<var> > ,Dynamic,1> y(3);
  y << 1, 2, 3;
  fvar<fvar<var> >  z = get_base1_lhs(y,1,"y",1);
  EXPECT_FLOAT_EQ(1, z.val_.val_.val());
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vec1_ffv) {
  using stan::math::get_base1_lhs;
  std::vector<fvar<fvar<var> > > x(2);
  x[0] = 10.0;
  x[1] = 20.0;
  EXPECT_FLOAT_EQ(10.0,get_base1_lhs(x,1,"x[1]",0).val_.val_.val());
  EXPECT_FLOAT_EQ(20.0,get_base1_lhs(x,2,"x[1]",0).val_.val_.val());
  
  get_base1_lhs(x,2,"x[2]",0) = 5.0;
  EXPECT_FLOAT_EQ(5.0,get_base1_lhs(x,2,"x[1]",0).val_.val_.val());

  EXPECT_THROW(get_base1_lhs(x,0,"x[0]",0),
               std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,3,"x[3]",0),
               std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vec2_ffv) {
  using stan::math::get_base1_lhs;
  using std::vector;
  size_t M = 3;
  size_t N = 4;

  vector<vector<fvar<fvar<var> > > > x(M,vector<fvar<fvar<var> > >(N,0.0));
  

  for (size_t m = 1; m <= M; ++m)
    for (size_t n = 1; n <= N; ++n)
      x[m - 1][n - 1] = (m * 10) + n;

  for (size_t m = 1; m <= M; ++m) {
    for (size_t n = 1; n <= N; ++n) {
      fvar<fvar<var> >  expected = x[m - 1][n - 1];
      fvar<fvar<var> >  found = get_base1_lhs(get_base1_lhs(x, m, "x[m]",1),
                               n, "x[m][n]",2);
      EXPECT_FLOAT_EQ(expected.val_.val_.val(),found.val_.val_.val());
    }
  }

  get_base1_lhs(get_base1_lhs(x,1,"",-1),2,"",-1) = 112.5;
  EXPECT_FLOAT_EQ(112.5, x[0][1].val_.val_.val());

  EXPECT_THROW(get_base1_lhs(x,0,"",-1),std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,M+1,"",-1),std::out_of_range);
  
  EXPECT_THROW(get_base1_lhs(get_base1_lhs(x,1,"",-1), 
                         12,"",-1),
               std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_matrix_ffv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<fvar<var> > ,Dynamic,Dynamic> x(4,3);
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 3; ++j)
      x(i,j) = i * j;
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(x(i,j).val_.val_.val(),
                      get_base1_lhs(x,i+1,j+1,"x",1).val_.val_.val());
      EXPECT_FLOAT_EQ(x(i,j).val_.val_.val(),
          get_base1_lhs(x,i+1,"x",1)(0,j).val_.val_.val());
      Matrix<fvar<fvar<var> > ,1,Dynamic> xi
        = get_base1_lhs<fvar<fvar<var> > >(x,i+1,"x",1);
      EXPECT_FLOAT_EQ(x(i,j).val_.val_.val(),xi[j].val_.val_.val());
      EXPECT_FLOAT_EQ(x(i,j).val_.val_.val(),get_base1_lhs(xi,j+1,"xi",2).val_.val_.val());
      // this is no good because can't get ref to inside val
      // could remedy by adding const versions, but don't need for Stan GM
      // fvar<fvar<var> >  xij = get_base1_lhs<fvar<fvar<var> > >(get_base1_lhs<fvar<fvar<var> > >(x,i+1,"x",1),
      //                                j+1,"xi",2);
    }
  }
  EXPECT_THROW(get_base1_lhs(x,10,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,1,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,1,100,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,0,1,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,1,0,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_vector_ffv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<fvar<var> > ,1,Dynamic> x(3);
  x << 1, 2, 3;
  
  for (size_t i = 0; i < 3; ++i)
    EXPECT_FLOAT_EQ(x(i).val_.val_.val(), get_base1_lhs(x,i+1,"x",1).val_.val_.val());
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_row_vector_ffv) {
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using stan::math::get_base1_lhs;
  Matrix<fvar<fvar<var> > ,Dynamic,1> x(3);
  x << 1, 2, 3;
  
  for (size_t i = 0; i < 3; ++i)
    EXPECT_FLOAT_EQ(x(i).val_.val_.val(), get_base1_lhs(x,i+1,"x",1).val_.val_.val());
  EXPECT_THROW(get_base1_lhs(x,0,"x",1), std::out_of_range);
  EXPECT_THROW(get_base1_lhs(x,100,"x",1), std::out_of_range);
}
TEST(AgradMixMatrixGetBase1LHS,get_base1_lhs_8_ffv) {
  using stan::math::get_base1_lhs;
  using std::vector;
  fvar<fvar<var> >  x0(42.0);
  // ~ 4m entries ~ 32MB memory + sizes
  vector<fvar<fvar<var> > > x1(9,x0);
  vector<vector<fvar<fvar<var> > > > x2(8,x1);
  vector<vector<vector<fvar<fvar<var> > > > > x3(7,x2);
  vector<vector<vector<vector<fvar<fvar<var> > > > > > x4(6,x3);
  vector<vector<vector<vector<vector<fvar<fvar<var> > > > > > > x5(5,x4);
  vector<vector<vector<vector<vector<vector<fvar<fvar<var> > > > > > > > x6(4,x5);
  vector<vector<vector<vector<vector<vector<vector<fvar<fvar<var> > > > > > > > > x7(3,x6);
  vector<vector<vector<vector<vector<vector<vector<vector<fvar<fvar<var> > > > > > > > > > x8(2,x7);

  EXPECT_EQ(x0.val_.val_.val(), x8[0][0][0][0][0][0][0][0].val_.val_.val());
  
  for (size_t i1 = 0; i1 < x8.size(); ++i1)
    for (size_t i2 = 0; i2 < x8[0].size(); ++i2)
      for (size_t i3 = 0; i3 < x8[0][0].size(); ++i3)
        for (size_t i4 = 0; i4 < x8[0][0][0].size(); ++i4)
          for (size_t i5 = 0; i5 < x8[0][0][0][0].size(); ++i5)
            for (size_t i6 = 0; i6 < x8[0][0][0][0][0].size(); ++i6)
              for (size_t i7 = 0; i7 < x8[0][0][0][0][0][0].size(); ++i7)
                for (size_t i8 = 0; i8 < x8[0][0][0][0][0][0][0].size(); ++i8)
                  x8[i1][i2][i3][i4][i5][i6][i7][i8]
                    = i1 * i2 * i3 * i4 * i5 * i6 * i7 * i8;

  for (size_t i1 = 0; i1 < x8.size(); ++i1)
    for (size_t i2 = 0; i2 < x8[0].size(); ++i2)
      for (size_t i3 = 0; i3 < x8[0][0].size(); ++i3)
        for (size_t i4 = 0; i4 < x8[0][0][0].size(); ++i4)
          for (size_t i5 = 0; i5 < x8[0][0][0][0].size(); ++i5)
            for (size_t i6 = 0; i6 < x8[0][0][0][0][0].size(); ++i6)
              for (size_t i7 = 0; i7 < x8[0][0][0][0][0][0].size(); ++i7)
                for (size_t i8 = 0; i8 < x8[0][0][0][0][0][0][0].size(); ++i8)
                  EXPECT_FLOAT_EQ(x8[i1][i2][i3][i4][i5][i6][i7][i8].val_.val_.val(),
                                  get_base1_lhs(x8,i1+1,i2+1,i3+1,i4+1,i5+1,i6+1,i7+1,i8+1,
                                            "x8",1).val_.val_.val());
}
