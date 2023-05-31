//
// Created by frongere on 31/05/23.
//

#include <gtest/gtest.h>
#include <type_traits>
#include <iostream>

//template <typename T>
//using is_floating_point = std::enable_if_t<std::is_floating_point_v<T>>;
//template <typename T, typename = is_floating_point<T>>
template <typename T>
class A {
 public:
  explicit A(const T &t) : m_t(t) {

  }

//  template <typename fake, typename = std::enable_if_t<std::is_floating_point_v<T>>>
//  void function() {}

//  template <typename = std::enable_if_t<std::is_integral_v<T>>>
//  void function() {}

 private:
  T m_t;

};

TEST(enable_if, test) {


  std::cout << std::is_floating_point_v<int> << std::endl;

  A<int> a(1);
  A<double> b(1);


}