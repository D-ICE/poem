//
// Created by smichel on 02/10/2024.
//

#include <gtest/gtest.h>
#include <poem/poem.h>
#include <poem/attributes.h>

#include <MathUtils/VectorGeneration.h>

using namespace poem;

TEST(poem_performance_polar_set, NestedForLoop)
{
    std::cout << "==== Setup started " << std::endl;
    // Create Dimensions
    std::cout << "     Dimension creation " << std::endl;
    auto STW_dim = std::make_shared<poem::Dimension>("STW_kt", "kt", "Speed Through Water");
    auto TWS_dim = std::make_shared<poem::Dimension>("TWS_kt", "kt", "True Wind Speed");
    auto TWA_dim = std::make_shared<poem::Dimension>("TWA_deg", "deg", "True Wind Angle");
    auto WA_dim = std::make_shared<poem::Dimension>("WA_deg", "deg", "Waves Angle");
    auto Hs_dim = std::make_shared<poem::Dimension>("Hs_m", "m", "Waves Significant Height");

    // Create a DimensionSet
    std::cout << "     DimensionSet creation " << std::endl;
    std::array<std::shared_ptr<poem::Dimension>, 5> array{STW_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim};
    auto dimension_set = std::make_shared<poem::DimensionSet<5>>(array);

    // Create samples for dimensions
    std::cout << "     Dimension samples creation " << std::endl;
    auto STW_vector = mathutils::arange<double>(0, 20, 1);
    auto TWS_vector = mathutils::arange<double>(0, 60, 5);
    auto TWA_vector = mathutils::arange<double>(0, 180, 15);
    auto WA_vector = mathutils::arange<double>(0, 180, 15);
    auto Hs_vector = mathutils::arange<double>(0, 8, 1);

    // Create the dimension grid
    std::cout << "     DimensionGrid creation " << std::endl;
    auto dimension_grid = poem::DimensionGrid(dimension_set);
    dimension_grid.set_values("STW_kt", STW_vector);
    ASSERT_FALSE(dimension_grid.is_filled());
    dimension_grid.set_values("TWS_kt", TWS_vector);
    dimension_grid.set_values("TWA_deg", TWA_vector);
    dimension_grid.set_values("WA_deg", WA_vector);
    dimension_grid.set_values("Hs_m", Hs_vector);
    ASSERT_TRUE(dimension_grid.is_filled());

    std::cout << "     DimensionPointSet creation " << std::endl;
    auto dimension_point_set = std::make_shared<poem::DimensionPointSet<5>>(dimension_grid);

    std::cout << "     Polar creation " << std::endl;
    auto polar = std::make_shared<poem::PolarTable<double, 5>>("BrakePower", "kW", "Brake Power",
                                                               poem::type::POEM_TYPES::DOUBLE, poem::POLAR_TYPE::PPP, dimension_point_set);

    std::cout << "     PolarSet creation " << std::endl;
    poem::Attributes attributes;
    attributes.add_attribute("polar_type", "PPP");
    std::string polar_name = "PPP";
//    attributes.add_attribute("name", "PPP");
    auto polar_set = std::make_shared<poem::PolarSet>(polar_name, attributes, poem::POLAR_TYPE::PPP);
    polar_set->new_polar<double, 5>("BrakePower", "kW", "Brake Power", poem::type::POEM_TYPES::DOUBLE, dimension_point_set);
    //(polar, "MylittlePolar");

    std::cout << "     PerformancePolarSet creation " << std::endl;
    poem::Attributes attributesPPS;
    attributesPPS.add_attribute("beam_m", "16.500000");
    attributesPPS.add_attribute("copyright", "D-ICE ENGINEERING (C)");
    attributesPPS.add_attribute("design_speed_kt", "8");
    attributesPPS.add_attribute("displacement_tons", "4643");
    attributesPPS.add_attribute("max_brake_power_kW", "9999");
    attributesPPS.add_attribute("name", "MyLittleShip");
    attributesPPS.add_attribute("poem_file_format_version", "v1");
    attributesPPS.add_attribute("vessel_type", "HYBRID");
    auto perf_polar_set = std::make_shared<poem::PerformancePolarSet>("/", attributesPPS);
    perf_polar_set->AddPolarSet(polar_set);

    std::cout << "==== Setup ended " << std::endl;

    std::cout << std::endl
              << "====================== LET'S START test_read.cpp >> PolarSetTest" << std::endl;
    std::cout << "Hola, this unit test load the PerformancePolarSetTest and read everything it can" << std::endl;
    auto res0 = perf_polar_set->name();
    std::cout << "Attribute name: " << res0 << std::endl;

    // smichel: pb, need to be fixed
    std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
    for (auto pl : all_polar_type_list)
    {
        auto name = polar_type_enum2s(pl);
        std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
        std::cout << perf_polar_set->exist(pl) << std::endl;
    }

    std::vector<POLAR_TYPE> res = perf_polar_set->polar_set_type_list();
    std::cout << "Polar set list (names): ";
    for (auto r : res)
    {
        std::cout << r << ", ";
    }

    std::cout << std::endl
              << std::endl
              << "======================" << std::endl;
    std::cout << "For each PolarSet, return info " << std::endl;
    std::cout << "======================" << std::endl;

    std::cout << "Construction of PolarSet from the polar_set function" << std::endl;
    // construct polars
    for (auto polar_name : res)
    {
        std::shared_ptr<PolarSet> ps_ptr = perf_polar_set->polar_set(polar_name);

        std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
        std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
        std::cout << "  PolarSet contains: ";
        auto polar_names = ps_ptr->polar_names();
        for (auto p : polar_names)
        {
            std::cout << p << ", ";
        }
        std::cout << std::endl;
        std::string pn = ps_ptr->polar_type_str();
        std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
        std::cout << "  Dimension point: (";
        for (auto i : dimension_point)
        {
            std::cout << i << ",";
        }
        std::cout << ")" << std::endl;
        for (auto p : polar_names)
        {
            std::shared_ptr<InterpolablePolarTable<5>> polar = ps_ptr->polar<double, 5>(p);
            std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
            const int dim = polar->dim();
            std::cout << "  Polar type: " << polar->polar_type() << std::endl;
            std::cout << "  Polar dim: " << polar->dim() << std::endl;
            std::cout << "  Polar size: " << polar->size() << std::endl;
            std::cout << "  Polar is filled ?: " << polar->is_filled() << std::endl;
            auto dim_name = polar->dimension_set_names();
            for (auto n : dim_name)
            {
                std::cout << "    Dimension " << n;
                std::cout << " - min: " << polar->min_bounds(n);
                std::cout << " - max: " << polar->max_bounds(n) << std::endl;
            }
            std::cout << "    Min & max values of " << p;
            std::cout << " - min: " << polar->min_value();
            std::cout << " - max: " << polar->max_value() << std::endl;

            // auto dpl = polar->dimension_point_list();
            // for (auto dp: dim_set){
            //   std::cout << dp << std::endl;
            // }

            auto nr = polar->nearest(dimension_point, true);
            std::cout << "  Nearest: " << nr << std::endl;
            auto interp = polar->interp(dimension_point, true);
            std::cout << "  Interpolation: " << interp << std::endl;
        }
        // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet_";
        // new_polar_file.append(ps_ptr->polar_type_str());
        // new_polar_file.append(".nc");
        // std::cout << "I like this PolarSet, I save it in " << new_polar_file << std::endl;
        // ps_ptr->to_netcdf(new_polar_file);
    }
    std::cout << "Well done, PerformancePolarSetTest ended " << std::endl;
}
