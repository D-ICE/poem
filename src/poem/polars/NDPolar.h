//
// Created by frongere on 28/03/2022.
//

#ifndef MSHIP_NDPOLAR_H
#define MSHIP_NDPOLAR_H

#include <MathUtils/RegularGridInterpolator.h>
#include <MathUtils/RegularGridNearest.h>

#include "PolarBase.h"


namespace mship {

  template<size_t _dim>
  class NDPolar : public PolarBase {

   public:
    explicit NDPolar(const std::string &ncfile);

    POLAR_CTRL ctrl_type() const override;

    void UpdateCache(OperationPoint *op) const override;

    bool CheckBounds(OperationPoint* op) const override;

   protected:
    void build_ideal_PB_to_STW_polar() override;

   private:
    POLAR_CTRL m_ctrl_type;

    using InterpolartorND = mathutils::RegularGridInterpolator<double, _dim>;
    using NearestND = mathutils::RegularGridNearest<int, _dim, double>;

    std::unique_ptr<InterpolartorND> m_LEEWAY_deg_polar;
    std::unique_ptr<InterpolartorND> m_HEELING_deg_polar;
    std::unique_ptr<InterpolartorND> m_propsRPM_polar;
    std::unique_ptr<InterpolartorND> m_propsEfficiency_polar;
    std::unique_ptr<InterpolartorND> m_PB_kW_polar;
    std::unique_ptr<InterpolartorND> m_rudders_angles_deg_polar;
    std::unique_ptr<InterpolartorND> m_hybrid_power_gain_polar;
    std::unique_ptr<InterpolartorND> m_conso_polar;
    std::unique_ptr<InterpolartorND> m_sails_active_power_kW_polar;
    std::unique_ptr<NearestND> m_solverStatus_polar;

    double m_STW_kt_min;
    double m_STW_kt_max;
    double m_TWS_kt_min;
    double m_TWS_kt_max;
    double m_TWA_deg_min;
    double m_TWA_deg_max;
    double m_WA_deg_min;
    double m_WA_deg_max;
    double m_Hs_m_min;
    double m_Hs_m_max;

  };


}  // mship


#include "NDPolar.hpp"


#endif //MSHIP_NDPOLAR_H
