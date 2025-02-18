#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Get information on a POEM File""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('infilename',
                        help='Input POEM file')

    return parser


# TODO: faire un poem_version !!
def get_info(polar_node: pypoem.PolarNode, infos: list):
    if polar_node.is_polar():
        polar = polar_node.as_polar()
        infos.append("Polar: " + polar_node.full_name())
        infos.append("\tDescription: %s" % polar_node.description())
        infos.append("\tMode: %s" % polar.mode())
        dimension_grid = polar.dimension_grid()
        infos.append("\tDimensions: %i" % dimension_grid.ndims())
        for dimension in dimension_grid.dimensions():
            dimension_size = dimension_grid.size(dimension)
            values_str = ' '.join(map(str, dimension_grid.values(dimension)))
            infos.append("\t\t%s (%i): %s" % (dimension, dimension_size, values_str))

        infos.append("\tPolarTables size: %i" % dimension_grid.size())
        infos.append("\tNumber of PolarTables: %i" % len(polar_node.children()))

    else:
        for polar_node_ in polar_node.children():
            get_info(polar_node_, infos)


def main():
    parser = get_parser()
    args = parser.parse_args()

    polar_node = pypoem.load(args.infilename)

    infos = list()
    get_info(polar_node, infos)

    for info in infos:
        print(info)


if __name__ == '__main__':
    main()
