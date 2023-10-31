//
// Created by frongere on 31/10/23.
//

#ifndef POEM_PERFORMANCEPOLARSET_H
#define POEM_PERFORMANCEPOLARSET_H

#include <memory>

#include "PolarSet.h"
#include "Attributes.h"


namespace poem {

  class PerformancePolarSet {
   public:
    PerformancePolarSet() :
        m_has_ppp(false),
        m_has_vpp(false),
        m_has_hvpp(false) {

    }

    void has_ppp(bool val) { m_has_ppp = val; }
    void has_vpp(bool val) { m_has_vpp = val; }
    void has_hvpp(bool val) { m_has_hvpp = val; }

    bool has_ppp(bool val) const { return m_has_ppp; }
    bool has_vpp(bool val) const  { return m_has_vpp; }
    bool has_hvpp(bool val) const  { return m_has_hvpp; }

    std::shared_ptr<PolarSet> ppp() const { return m_ppp; }

    std::shared_ptr<PolarSet> vpp() const { return m_vpp; }

    std::shared_ptr<PolarSet> hvpp() const { return m_hvpp; }


   private:
    bool m_has_ppp;
    std::shared_ptr<PolarSet> m_ppp;
    bool m_has_vpp;
    std::shared_ptr<PolarSet> m_vpp;
    bool m_has_hvpp;
    std::shared_ptr<PolarSet> m_hvpp;

    Attributes m_attributes;

  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSET_H
