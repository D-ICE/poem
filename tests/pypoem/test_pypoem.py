from pypoem import pypoem
# import pypoem

import warnings
import numpy as np

if __name__ == '__main__':

    STW = pypoem.make_dimension("STW_dim", "kt", "Speed Through Water")
    TWS = pypoem.make_dimension("TWS_dim", "kt", "True Wind Speed")
    TWA = pypoem.make_dimension("TWA_dim", "deg", "True Wind Angle")
    WA = pypoem.make_dimension("WA_dim", "deg", "Mean Waves Angle")
    Hs = pypoem.make_dimension("Hs_dim", "deg", "Significant Waves Height")

    dimension_set = pypoem.make_dimension_set((STW, TWS, TWA, WA, Hs))

    dimension_grid = pypoem.make_dimension_grid(dimension_set)

    dimension_grid.set_values("STW_dim", np.linspace(8, 20, 13))
    dimension_grid.set_values("TWS_dim", np.linspace(0, 40, 9))
    dimension_grid.set_values("TWA_dim", np.linspace(0, 180, 13))
    dimension_grid.set_values("WA_dim", np.linspace(0, 180, 13))
    dimension_grid.set_values("Hs_dim", np.linspace(0, 8, 9))

    polar_MPPP = pypoem.make_polar("MPPP", pypoem.MPPP, dimension_grid)

    total_power = polar_MPPP.create_polar_table_double("TOTAL_POWER", "kW", "Total Power")

    # Filling with test data
    data = np.ones(dimension_grid.shape())
    val = 0
    for i0 in range(dimension_grid.size("STW_dim")):
        for i1 in range(dimension_grid.size("TWS_dim")):
            for i2 in range(dimension_grid.size("TWA_dim")):
                for i3 in range(dimension_grid.size("WA_dim")):
                    for i4 in range(dimension_grid.size("Hs_dim")):
                        data[i0, i1, i2, i3, i4] = val
                        val += 1

    # Set PolarTable data using NDArray
    total_power.set_values(data)

    # Get back data from PolarTable
    data_back = total_power.array()  # This is a view (directly data inside the PolarTable, no copy)
    assert np.all(data == data_back)  # data layout has been respected back and forth between python and poem

    assert data_back[0, 0, 0, 0, 0] == 0.  # value is currently 0

    data_back[0, 0, 0, 0, 0] = 999.  # change value
    assert data_back[0, 0, 0, 0, 0] == 999.  # yes changed

    assert total_power.array()[0, 0, 0, 0, 0] == 999.  # This is really a view, not a copy

    array_copy = total_power.array().copy()  # This is a copy
    array_copy[0, 0, 0, 0, 0] = 0.
    assert array_copy[0, 0, 0, 0, 0] == 0.
    assert total_power.array()[0, 0, 0, 0, 0] == 999. # array_copy was really a copy

    polar_MPPP.create_polar_table_double("LEEWAY", "deg", "LEEWAY Angle").fill_with(0.)
    polar_MPPP.create_polar_table_int("SOLVER_STATUS", "-", "Solver Status").fill_with(0)

    pypoem.to_netcdf(polar_MPPP, "my_vessel", "Polar_MPPP.nc")

    if pypoem.spec_check("Polar_MPPP.nc"):
        print("Polar_MPPP.nc COMPLIANT with POEM specs version 1")
    else:
        warnings.warn("Polar_MPPP.nc NOT COMPLIANT with POEM specs version 1")

    # TEST PolarSet
    polar_set = pypoem.make_polar_set("polar_set", "Description of this PolarSet")
    polar_set.attach_polar(polar_MPPP)


    pypoem.to_netcdf(polar_set, "my_vessel", "my_vessel.nc")
    # pypoem.to_netcdf(polar_set, "S2Z_OWx6INTAVG_vG", "my_vessel.nc")

    if pypoem.spec_check("my_vessel.nc"):
        print("my_vessel.nc COMPLIANT with POEM specs version 1")
    else:
        warnings.warn("my_vessel.nc NOT COMPLIANT with POEM specs version 1")


    pypoem.load("my_vessel.nc")
