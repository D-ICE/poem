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
    parser.add_argument('infilename',
                        help='Input file')

    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()

    polar_node = pypoem.load(args.infilename, "vessel")
    print(polar_node.layout())


if __name__ == '__main__':
    main()
