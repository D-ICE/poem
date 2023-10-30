//
// Created by hippolyte on 20/10/23.
//

#ifndef POEM_PYWRAPPER_H
#define POEM_PYWRAPPER_H

#include "poem/poem.h"
#include <string>
#include <vector>
#include <memory>


class PyWrapper {
public:
    PyWrapper(const std::string &file_path);
    std::vector<std::string> get_names() const;
    std::map<std::string, std::string> get_attributes() const;
    double interp(const std::string &name, const std::vector<double> point);
private:
    static double interp5d(poem::PolarBase *polar, std::vector<double> x);
    std::shared_ptr<poem::PolarSetReader> m_polar_set;
};


#endif //POEM_PYWRAPPER_H
