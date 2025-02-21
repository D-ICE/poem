import json

from pypoem import pypoem
import numpy as np

root_node = pypoem.load("my_vessel.nc")

layout = json.loads(root_node.layout())
print(layout)

for polar_table in layout['polar_tables']:
    print(polar_table)


# We got a path from layout
