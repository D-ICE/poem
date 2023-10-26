//
// Created by frongere on 29/03/23.
//

#ifndef POEM_EXCEPTIONS_H
#define POEM_EXCEPTIONS_H

#include <string>
#include <exception>

#define CRITICAL_ERROR_POEM \
spdlog::critical("{}:{} CRITICAL ERROR", __FILE__, __LINE__); \
exit(0);

#define NIY_POEM \
spdlog::critical("{}:{} NOT IMPLEMENTED YET", __FILE__, __LINE__); \
exit(0);


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
