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

    polar_MPPP = pypoem.make_polar("MPPP", pypoem.MPPP, dimension_grid)
    BrakePower = polar_MPPP.create_polar_table_double("BrakePower", "kW", "Brake Power", pypoem.POEM_DOUBLE)

    # Filling with test data
    data = np.ones(dimension_grid.shape())
    val = 0
    for i in range(dimension_grid.size("STW")):
        for j in range(dimension_grid.size("TWS")):
            for k in range(dimension_grid.size("TWA")):
                data[i][j][k] = val
                val += 1

    # Set PolarTable data using NDArray
    BrakePower.set_values(data)

    # Get back data from PolarTable
    data_back = BrakePower.array()  # This is a view (directly data inside the PolarTable, no copy)
    assert np.all(data == data_back)  # data layout has been respected back and forth between python and poem

    assert data_back[0, 0, 0] == 0.  # value is currently 0

    data_back[0, 0, 0] = 999.  # change value
    assert data_back[0, 0, 0] == 999.  # yes changed

    assert BrakePower.array()[0, 0, 0] == 999.  # This is really a view, not a copy

    array_copy = BrakePower.array().copy()  # This is a copy
    array_copy[0, 0, 0] = 0.
    assert array_copy[0, 0, 0] == 0.
    assert BrakePower.array()[0, 0, 0] == 999. # array_copy was really a copy

    pypoem.to_netcdf(BrakePower, "BrakePower.nc")
