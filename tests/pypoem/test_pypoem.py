from pypoem import pypoem
import numpy as np


if __name__ == '__main__':
    # print(pypoem.current_standard_poem_version())

    STW = pypoem.make_dimension("STW", "kt", "Speed Through Water")
    TWS = pypoem.make_dimension("TWS", "kt", "True Wind Speed")
    TWA = pypoem.make_dimension("TWA", "deg", "True Wind Angle")

    dimension_set = pypoem.make_dimension_set((STW, TWS, TWA))

    dimension_grid = pypoem.make_dimension_grid(dimension_set)

    dimension_grid.set_values("STW", np.linspace(8, 20, 13))
    dimension_grid.set_values("TWS", np.linspace(0, 40, 9))
    dimension_grid.set_values("TWA", np.linspace(0, 180, 13))

    # polar_table = pypoem.make_polar_table_double("VAR", "-", "VAR", dimension_grid)

    polar = pypoem.make_polar("MPPP", pypoem.MPPP, dimension_grid)
    BrakePower = polar.create_polar_table_double("BrakePower", "kW", "Brake Power", pypoem.POEM_DOUBLE)

    BrakePower.fill_with(1.)

    polar_set = pypoem.make_polar_set("polar_set")
    polar_set.attach_polar(polar)

    pypoem.to_netcdf(polar, "Polar.nc")
    pypoem.to_netcdf(BrakePower, "BrakePower.nc")
    pypoem.to_netcdf(polar_set, "polar_set.nc")

    print("END")