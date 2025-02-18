#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
import fnmatch
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Clean a POEM File by removing every non mandatory PolarTables""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('infilename', type=str, help='Input file')
    parser.add_argument('outfilename', type=str, help='Cleaned output file')
    parser.add_argument('--keep', nargs='*', default='',
                        help="Give a list of PolarTables that we want to keep into the file. "
                             "Glob syntax (with * and ?) is accepted")

    return parser


def clean(polar_node: pypoem.PolarNode, keep_list: list):
    if polar_node.is_polar():
        polar = polar_node.as_polar()
        mandatory_polar_tables = [pt for pt in pypoem.mandatory_polar_tables(polar.mode())]

        for polar_table in polar.children():
            name = polar_table.name()

            keep = False
            for pattern in keep_list:
                if fnmatch.fnmatch(polar_table.name(), pattern):
                    keep = True
                    break

            if keep:
                continue

            if polar_table.name() not in mandatory_polar_tables:
                polar.remove_polar_table(polar_table.name())

    else:
        for polar_node_ in polar_node.children():
            clean(polar_node_, keep_list)


def main():
    parser = get_parser()
    args = parser.parse_args()

    polar_node = pypoem.load(args.infilename)
    clean(polar_node, args.keep)
    pypoem.to_netcdf(polar_node, polar_node.name(), args.outfilename)


if __name__ == '__main__':
    main()
