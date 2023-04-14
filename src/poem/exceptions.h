//
// Created by frongere on 29/03/23.
//

#ifndef POEM_EXCEPTIONS_H
#define POEM_EXCEPTIONS_H

#include <string>
#include <exception>

#define STOP throw StopForDevelopment();
#define NIY throw NotImplementedYet();
#define CRITICAL_ERROR throw CriticalError();

namespace poem {

  /// For the case we want to delay implementation in the code...
  struct NotImplementedYet : public std::exception {
    NotImplementedYet() = default;
    const char *what() const throw() override {
      return "Not implemented yet";
    }

  };

  struct StopForDevelopment : public std::exception {
    StopForDevelopment() = default;
    const char *what() const throw() override {
      return "This stop for development purpose";
    }
  };

  struct CriticalError : public std::exception {
    CriticalError() = default;
    const char *what() const throw() override {
      return "Critical Error";
    }
  };

}  // poem

#endif //POEM_EXCEPTIONS_H
