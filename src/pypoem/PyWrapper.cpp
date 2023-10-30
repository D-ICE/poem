//
// Created by hippolyte on 20/10/23.
//

#include "PyWrapper.h"

PyWrapper::PyWrapper(const std::string &file_path) {
    m_polar_set = std::make_shared<poem::PolarSetReader>();
    m_polar_set->Read(file_path);
}

std::vector<std::string> PyWrapper::get_names() const {
    return m_polar_set->polar_set()->polar_names();
}

std::map<std::string, std::string> PyWrapper::get_attributes() const {
    std::map<std::string, std::string> attr_map;
    for (const auto& elem : *m_polar_set->attributes())
        attr_map.insert({elem.first, elem.second});

    return attr_map;
}
//
//double PyWrapper::interp5d(poem::PolarBase *polar, std::vector<double> x) {
//    std::array<double, 5> y{};
//    std::copy(x.begin(), x.begin() + 5, y.begin());
//    return polar->interp<double>(y, false);
//}
//
//double PyWrapper::interp(const std::string &name, const std::vector<double> point) {
//    auto select_polar = m_polar_set->get_polar(name);
//    size_t polar_dim = select_polar->dim();
//    if (polar_dim != point.size()){
//        spdlog::critical("Point and polar dims must match");
//    }
//
//    switch (polar_dim) {
//        case 5: return interp5d(select_polar,point);
//        default: NIY_POEM;
//    }
//}
