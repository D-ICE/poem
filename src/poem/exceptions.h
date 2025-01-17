//
// Created by frongere on 29/03/23.
//

#ifndef POEM_EXCEPTIONS_H
#define POEM_EXCEPTIONS_H

#include <string>
#include <exception>
#include <spdlog/spdlog.h>

#define CRITICAL_ERROR_POEM                                                  \
  std::string msg = fmt::format("{}:{} CRITICAL ERROR", __FILE__, __LINE__); \
  throw PoemException(msg);

#define NIY_POEM                                                                  \
  std::string msg = fmt::format("{}:{} NOT IMPLEMENTED YET", __FILE__, __LINE__); \
  throw PoemException(msg);

namespace poem {

  /**
   * poem own exception class
   */
  struct PoemException : public std::exception {
    PoemException(const std::string &msg) : m_msg(msg) {};

    const char *what() const throw() override {
      spdlog::critical(m_msg);
      return "POEM EXCEPTION";
    }

    std::string m_msg;
  };

} // poem

#endif // POEM_EXCEPTIONS_H
