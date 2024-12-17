//
// Created by frongere on 17/12/24.
//

#include <type_traits>
#include "string"
#include "iostream"
#include <memory>

///// ESSAI
//struct Base {
//  virtual void func() {
//    std::cerr << "On ne doit pas etre la !!" << std::endl;
//  }
//};
//
//
//template<class Derived>
//struct Base_ : public Base {
//  void func() override {
//    static_cast<Derived *>(this)->func_impl();
//  }
//};
//
//
//template<typename T>
//struct A : public Base_<A<T>> {
//
//  A() : Base_<A<T>>() {}
//
// private:
////  template<std::enable_if<std::is_floating_point_v<T>, bool>>
//  void func_impl() {
//    std::cout << "Dans implementation" << std::endl;
//  }
//
//  friend class Base_<A<T>>;
//
//};


/////////////////////////////////////


//template<typename T>
//class Polar;
//
//struct PolarBase {
//
//  template<typename T>
//  T nearest(double val) {
//    return static_cast<Polar<T> *>(this)->nearest(val);
//  }
//
//  template<typename T> std::enable_if_t<std::is_floating_point_v<T>>
//  interp(double val) {
//    return static_cast<Polar<T> *>(this)->template interp<T>(val);
//  }
//
//};
//
//template<class T>
//struct Polar : public PolarBase {
//
//  T nearest(double val) {
////    return PolarBase::nearest<T>(val);
//    std::cout << "nearest" << std::endl;
////    return nearest_impl(val);
//    return 0;
//  }
//
//
//  template<typename R=T>
//  std::enable_if_t<std::is_floating_point_v<R>>
//  interp(double val) {
//    std::cout << "interp" << std::endl;
//    return 0.1;
//  }

// private:
//  T nearest_impl(double val) {
//    std::cout << "nearest" << std::endl;
//  }

//  friend class PolarBase;
//};

template <typename T>
using is_floating_point = std::enable_if_t<std::is_floating_point_v<T>>;

template <typename T>
using is_arithmetic = std::enable_if_t<std::is_arithmetic_v<T>>;


template<typename T, class = is_arithmetic<T>>
class Polar;




struct PolarBase {
  template<typename T, class = is_arithmetic<T>>
  T func() {
    return static_cast<Polar<T> *>(this)->func();
  }

};

template<typename T, class>
struct Polar : public PolarBase {

  T func() {
    T a(0);
    std::cout << "Polar::func" << std::endl;
    return a;
  }

};


int main() {

  std::shared_ptr<PolarBase> polar_int_base = std::make_shared<Polar<int>>();

  polar_int_base->func<int>();








//  std::shared_ptr<PolarBase> polar_int = std::make_shared<Polar<int>>();
//  std::shared_ptr<PolarBase> polar_double = std::make_shared<Polar<double>>();
//
////  auto ret_int = polar_int->interp(2);
//
////  Polar<int> polar_int;
////  polar_int.nearest(1);
//////  polar_int.interp(1.);


  return 0;
}

/**
* On veut une classe de base non template pour pouvoir la placer dans un Polar
 *
 * On veut que cette classe de base ait les methodes interp et nearest
 *
 * On veut que la methode nearest soit toujours dispo
 *
 * On veut que la methode interp ne soit dispo que si on a un type float
 *
*/