//
// Created by frongere on 29/03/23.
//

#ifndef POEM_EXCEPTIONS_H
#define POEM_EXCEPTIONS_H

#include <string>
#include <exception>

#define STOP_POEM throw StopForDevelopment();
#define TODO_POEM throw poem::Todo();
#define NIY_POEM throw poem::NotImplementedYet();
#define CRITICAL_ERROR_POEM throw poem::CriticalError();

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

  struct Todo : public std::exception {
    Todo() = default;
    const char *what() const throw() override {
      return "SOMETHING IS TO BE IMPLEMENTED HERE";
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
