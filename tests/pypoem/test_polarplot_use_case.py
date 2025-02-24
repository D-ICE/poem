import json
from pypoem import pypoem

# root_node = pypoem.load("my_vessel.nc")
root_node = pypoem.load("b799ba3fcec6fd8d48b0ea174e9fc7bd748a8b8a3af21b573669fe5f3d503faa.nc")

layout = json.loads(root_node.layout())
# print(layout)

for path in layout['polar_tables']:
    polar_table = root_node.polar_node_from_path(path)
    if polar_table.is_polar_table_double():
        polar_table = polar_table.as_polar_table_double()
    if polar_table.is_polar_table_int():
        polar_table = polar_table.as_polar_table_int()

    sliced = polar_table.slice({'STW_dim': 10, 'TWS_dim': 12, 'WA_dim': 90, 'Hs_dim': 1})
    sliced.squeeze()
    array = sliced.array()
    print(array)
