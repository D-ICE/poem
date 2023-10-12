//
// Created by hippolyte on 12/10/23.
//

#ifndef POEM_PYPOEM_H
#define POEM_PYPOEM_H
#include <boost/python.hpp>
#include "poem/polar/PolarSet.h"

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;

    class_<poem::PolarSet>("PolarSet").def("interp3", &poem::PolarSet::interp<double,3>);
}
#endif //POEM_PYPOEM_H
