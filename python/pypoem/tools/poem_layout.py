#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Get a json layout of a POEM File""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    # Please define this argument. We need to have a way to check the version of a tool from command line
    parser.add_argument('infilename',
                        help='The file we want to check against POEM specifications')
    parser.add_argument("-i2", action="store_true",
                        help="The indent for the json string representation of the layout is set to 2")

    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()

    indent = -1
    if args.i2:
        indent = 2

    polar_node = pypoem.load(args.infilename, "vessel", True, False)
    print(polar_node.layout(indent))


if __name__ == '__main__':
    main()
