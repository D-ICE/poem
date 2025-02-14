#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Clean a POEM File by removing every non mandatory PolarTables""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    # Please define this argument. We need to have a way to check the version of a tool from command line
    parser.add_argument('infilename', type=str, help='Input file')
    parser.add_argument('outfilename', type=str, help='Cleaned output file')

    return parser


def clean(polar_node):
    if polar_node.is_polar():
        pass
    else:
        for polar_node_ in polar_node.children():
            clean(polar_node_)


def main():
    parser = get_parser()
    args = parser.parse_args()

    polar_node = pypoem.load(args.infilename, "vessel")
    clean(polar_node)
    pypoem.to_netcdf(polar_node, polar_node.name(), args.outfilename)

    import poem_tree
    poem_tree.execute(args.outfilename)


if __name__ == '__main__':
    main()
