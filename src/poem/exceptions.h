//
// Created by frongere on 29/03/23.
//

#ifndef POEM_EXCEPTIONS_H
#define POEM_EXCEPTIONS_H

#include <string>
#include <exception>
#include <cools/spdlog.h>


#define CRITICAL_ERROR_POEM                                                  \
  {std::string msg = fmt::format("{}:{} POEM CRITICAL ERROR", __FILE__, __LINE__); \
  throw poem::PoemException(msg);}

#define NIY_POEM                                                                  \
  {std::string msg = fmt::format("{}:{} POEM NOT IMPLEMENTED YET", __FILE__, __LINE__); \
  throw poem::PoemException(msg);}


using namespace cools::logging;

namespace poem {

  /**
   * poem own exception class
   */
  struct PoemException : public std::exception {
    explicit PoemException(const std::string &msg) : m_msg(msg) {};

    [[nodiscard]] const char *what() const throw() override {
      LogCriticalError(m_msg);
      return "POEM EXCEPTION";
    }

    std::string m_msg;
  };

} // poem

#endif // POEM_EXCEPTIONS_H
