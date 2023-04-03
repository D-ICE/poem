//
// Created by frongere on 28/03/2022.
//

#include "NDPolar.h"

#include "mship/exceptions.h"
#include "NetCDF.h"

#include <set>

namespace mship {

  template<size_t _dim>
  NDPolar<_dim>::NDPolar(const std::string &ncfile):
      PolarBase(),
      m_LEEWAY_deg_polar(nullptr),
      m_HEELING_deg_polar(nullptr),
      m_propsRPM_polar(nullptr),
      m_propsEfficiency_polar(nullptr),
      m_PB_kW_polar(nullptr),
      m_rudders_angles_deg_polar(nullptr),
      m_hybrid_power_gain_polar(nullptr),
      m_conso_polar(nullptr),
      m_sails_active_power_kW_polar(nullptr),
      m_solverStatus_polar(nullptr) {

    netCDF::NcFile dataFile(ncfile, netCDF::NcFile::read);

    // Control type extraction
    std::string ctrl_type;
    dataFile.getAtt("control_variable").getValues(ctrl_type);
    if (ctrl_type == "STW_kt") {
      m_ctrl_type = POLAR_CTRL::STW;
    } else {
      throw NotImplementedYet();
    }

    std::vector<std::string> vars;
    for (const auto &var: dataFile.getVars()) {
      vars.push_back(var.first);
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("LEEWAY")) != std::end(vars)) {
      m_LEEWAY_deg_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "LEEWAY",
                                         m_LEEWAY_deg_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("HEELING")) != std::end(vars)) {
      m_HEELING_deg_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "HEELING",
                                         m_HEELING_deg_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("PropsRPM")) != std::end(vars)) {
      m_propsRPM_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "PropsRPM",
                                         m_propsRPM_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("PropsEfficiency")) != std::end(vars)) {
      m_propsEfficiency_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "PropsEfficiency",
                                         m_propsEfficiency_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("BrakePower")) == std::end(vars)) {
      throw std::runtime_error("Missing required variable 'BrakePower' from NC file");
    }
    m_PB_kW_polar = std::make_unique<InterpolartorND>();
    internal::load_polar<double, _dim>(dataFile,
                                       {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                       "BrakePower",
                                       m_PB_kW_polar.get());

    if (std::find(std::begin(vars), std::end(vars), std::string("RuddersAngle")) != std::end(vars)) {
      m_rudders_angles_deg_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "RuddersAngle",
                                         m_rudders_angles_deg_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("HybridPowerGains")) != std::end(vars)) {
      m_hybrid_power_gain_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "HybridPowerGains",
                                         m_hybrid_power_gain_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("conso_t_h")) != std::end(vars)) {
      m_conso_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "conso_t_h",
                                         m_conso_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("SailsActivePower")) != std::end(vars)) {
      m_sails_active_power_kW_polar = std::make_unique<InterpolartorND>();
      internal::load_polar<double, _dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         "SailsActivePower",
                                         m_sails_active_power_kW_polar.get());
    }

    if (std::find(std::begin(vars), std::end(vars), std::string("SolverStatus")) != std::end(vars)) {
      m_solverStatus_polar = std::make_unique<NearestND>();
      internal::load_solver_status<_dim>(dataFile,
                                         {ctrl_type, "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"},
                                         m_solverStatus_polar.get());
    }

    // Extremum of coordinates
    m_STW_kt_min = m_PB_kW_polar->GetCoordMinVal(0);
    m_STW_kt_max = m_PB_kW_polar->GetCoordMaxVal(0);
    m_TWS_kt_min = m_PB_kW_polar->GetCoordMinVal(1);
    m_TWS_kt_max = m_PB_kW_polar->GetCoordMaxVal(1);
    m_TWA_deg_min = m_PB_kW_polar->GetCoordMinVal(2);
    m_TWA_deg_max = m_PB_kW_polar->GetCoordMaxVal(2);
    m_WA_deg_min = m_PB_kW_polar->GetCoordMinVal(3);
    m_WA_deg_max = m_PB_kW_polar->GetCoordMaxVal(3);
    m_Hs_m_min = m_PB_kW_polar->GetCoordMinVal(4);
    m_Hs_m_max = m_PB_kW_polar->GetCoordMaxVal(4);



//    auto STW_ = m_solverStatus_polar->GetCoord(0);
//    auto TWS_ = m_solverStatus_polar->GetCoord(0);
//    auto TWA_ = m_solverStatus_polar->GetCoord(0);
//    auto WA_ = m_solverStatus_polar->GetCoord(0);
//    auto Hs_ = m_solverStatus_polar->GetCoord(0);
//
//    for (const auto STW: STW_) {
//      for (const auto TWS: TWS_) {
//        for (const auto TWA: TWA_) {
//          for (const auto WA: WA_) {
//            for (const auto Hs: Hs_) {
//              std::array<double, 5> coords = {STW, TWS, TWA, WA, Hs};
//              auto LEE = m_LEEWAY_deg_polar->Interp(coords);
//              if (std::isnan(LEE)) {
//                std::cout << m_solverStatus_polar->Nearest(coords) << std::endl;
//              }
//            }
//          }
//        }
//      }
//    }


    build_ideal_PB_to_STW_polar();

  }

  template<size_t _dim>
  PolarBase::POLAR_CTRL NDPolar<_dim>::ctrl_type() const {
    return m_ctrl_type;
  }

  template<size_t _dim>
  void NDPolar<_dim>::UpdateCache(OperationPoint *op) const {
    /*
     * On veut remplir:
     *
     * m_leeway_deg
     * m_heeling_deg
     * m_RPM
     * m_PB_kW
     * m_etaD
     * m_rudders_angle
     * m_hybrid_power_gains
     * m_service_power_kW
     * m_sfc_t_h
     * m_m_sails_power
     *
     */

    // TODO: faire comparaison avec xarray en python

    // FIXME: la premiere coord n'est potentiellement pas STW, ca depend du type controle de la polaire...
    // WARNING : extrapolation permitted for Hs > Hs_max, by capping after Hs_max
    std::array<double, 5> point = {op->STW_kt, op->TWS_kt, std::fabs(op->TWA_deg), std::fabs(op->WA_deg), std::min(op->ground_weather.Hs_m, m_Hs_m_max)};

    /*
     * Differents cas de voisinnage
     *
     * - Aucun NaN dans le voisinage, on parvient a interpoler PB, tous les voisins sont a 1 -> cas ideal
     * - Au moins 1 NaN dans le voisinage: l'interpolation de PB renvoie NaN
     *    - le nearest est 1 -> on prend celui-ci...
     *    - le nearest est pas 1, on regarde tous les voisins pour voir s'il y en a pas un a 1
     *      - si un seul, on prend celui-ci
     *      - si plusieurs, on prend le plus proche
     *      - si aucun... A reflechir
     *
     *
     *
     */

    // Mandatory
    m_PB_kW = m_PB_kW_polar->Interp(point);

    bool got_nan = std::isnan(m_PB_kW);
    std::array<size_t, _dim> nearest_indices;

    if (got_nan) {
      if (!m_solverStatus_polar) {
        throw std::runtime_error("No solver status polar while reaching NaN in polar. This should never happen...");
      }

      // Looking for the nearest neighbour solver status value
      auto nearest_neighour = m_solverStatus_polar->Nearest(point);

      if (nearest_neighour.val() == 1) {
        // Ici, on aimerait changer point...
        // Pas certain que ce soit possible, les interpolations sur base NaN risquent de se reproduire...
        // -> il faut tester neanmoins
        nearest_indices = nearest_neighour.indices();
        m_PB_kW = m_PB_kW_polar->EvalAtIndice(nearest_indices);
      }
      else {
        got_nan = false;
      }

      // There is at least one NaN in the surrounding of point, getting the whole set of neighbours for point
//      auto nodes = m_solverStatus_polar->GetSurroundingGridNodes(point);
    }


    // TODO: voir si on conserve...
    if (m_solverStatus_polar) {
      m_solver_status = m_solverStatus_polar->Nearest(point).val();
      if (m_solver_status != 1) {
        throw NaNInPolar();
      }
    } else {
      m_solver_status = 1; // CDA...
    }


//    if (std::isnan(m_PB_kW)) {
//      auto nodes = m_solverStatus_polar->GetSurroundingGridNodes(point);
//
//      std::set<int> codes;
//
//      for (const auto &node : nodes) {
//        int val = node.val();
//        if (val!=1) codes.insert(val);
//      }
//
//      // if (codes.size() > 1) {
//      //   std::cerr << "Different codes: ";
//      //   for (const auto &code:codes) {
//      //     std::cerr << code << "; ";
//      //   }
//      //   std::cerr << std::endl;
//      // }
//
//      m_solver_status = *codes.begin();
//
//      throw NaNInPolar();
//    }




    if (m_LEEWAY_deg_polar) {
      if (got_nan) {
        m_leeway_deg = m_LEEWAY_deg_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_leeway_deg = m_LEEWAY_deg_polar->Interp(point);
      }
      if (op->TWA_deg < 0.) m_leeway_deg = -m_leeway_deg.value();
    }

    if (m_HEELING_deg_polar) {
      if (got_nan) {
        m_heeling_deg = m_HEELING_deg_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_heeling_deg = m_HEELING_deg_polar->Interp(point);
      }
      if (op->TWA_deg < 0.) m_heeling_deg = -m_heeling_deg.value();
    }

    if (m_propsRPM_polar) {
      if (got_nan) {
        m_RPM = m_propsRPM_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_RPM = m_propsRPM_polar->Interp(point);
      }
    }

    if (m_propsEfficiency_polar) {
      if (got_nan) {
        m_etaD = m_propsEfficiency_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_etaD = m_propsEfficiency_polar->Interp(point);
      }
    }

    if (m_rudders_angles_deg_polar) {
      if (got_nan) {
        m_rudders_angle_deg = m_rudders_angles_deg_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_rudders_angle_deg = m_rudders_angles_deg_polar->Interp(point);
      }
      if (op->TWA_deg < 0.) m_rudders_angle_deg = -m_rudders_angle_deg.value();
    }

    if (m_hybrid_power_gain_polar) {
      if (got_nan) {
        m_hybrid_power_gain = m_hybrid_power_gain_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_hybrid_power_gain = m_hybrid_power_gain_polar->Interp(point);
      }
    }

    if (m_conso_polar) {
      if (got_nan) {
        m_sfc_t_h = m_conso_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_sfc_t_h = m_conso_polar->Interp(point);
      }
    }

    if (m_sails_active_power_kW_polar) {
      if (got_nan) {
        m_sails_active_power_kW = m_sails_active_power_kW_polar->EvalAtIndice(nearest_indices);
      }
      else {
        m_sails_active_power_kW = m_sails_active_power_kW_polar->Interp(point);
      }
    }

    m_service_power_kW = 0.;  // Ajouter a terme...

    // TODO: terminer

  }

  template<size_t _dim>
  void NDPolar<_dim>::build_ideal_PB_to_STW_polar() {

    auto STW_kt_vec = m_PB_kW_polar->GetCoord(0);
    auto n = (int) STW_kt_vec.size();

    std::vector<double> PB_kW_vec;
    PB_kW_vec.reserve(n);

    boost::multi_array<double, 1> STW_kt_arr(boost::extents[n]);

    int i = 0;
    for (const auto &STW_kt: STW_kt_vec) {
      double PB_kW = m_PB_kW_polar->Interp({STW_kt, 0., 0., 0., 0.});
//      std::cout << PB_kW << "\t" << STW_kt << std::endl;
//      if (std::isnan(PB_kW)) {
//        std::cerr << "NaN in calm water power curve. This should not happen" << std::endl;
//        exit(EXIT_FAILURE);
//      }
      PB_kW_vec.push_back(PB_kW);
      STW_kt_arr[i] = STW_kt;
      ++i;
    }

    m_ideal_PB_to_STW_polar->AddCoord(PB_kW_vec);
    m_ideal_PB_to_STW_polar->AddVar(STW_kt_arr);

  }

  template<size_t _dim>
  bool NDPolar<_dim>::CheckBounds(OperationPoint* op) const {

    if (op->STW_kt < m_STW_kt_min || op->STW_kt > m_STW_kt_max) return false;
    if (op->TWS_kt < m_TWS_kt_min || op->TWS_kt > m_TWS_kt_max) return false;
//    if (op->TWA_deg < m_TWA_deg_min || op->TWA_deg > m_TWA_deg_max) return false;
//    if (op->WA_deg < m_WA_deg_min || op->WA_deg > m_WA_deg_max) return false;
//    if (op->ground_weather.Hs_m < m_Hs_m_min || op->ground_weather.Hs_m > m_Hs_m_max) return false;
    // WARNING : extrapolation permitted for Hs > Hs_max, by capping after Hs_max
    if (op->ground_weather.Hs_m < m_Hs_m_min) return false;

    return true;
  }

}  // mship
