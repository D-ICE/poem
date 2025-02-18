#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
import sys

from pypoem import pypoem
from pypoem.pypoem import spec_check


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Upgrade a POEM File to the last Specification version""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("what_to_mount", help='What to mount with syntax path@file.nc')
    parser.add_argument("where_to_mount", help='Where to mount with syntax path@file.nc')

    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()

    # file_version = pypoem.get_version(args.infilename)
    # last_version = pypoem.current_standard_poem_version()
    #
    # if file_version == last_version and spec_check(args.infilename):
    #     print("File is already compliant with last POEM Specification v%i" %
    #           pypoem.current_standard_poem_version())
    #     sys.exit(0)
    # else:
    #     print("Upgrading file version v%i -> v%i" % (file_version, last_version))
    #
    # polar_node = pypoem.load(args.infilename, "vessel")
    # pypoem.to_netcdf(polar_node, polar_node.name(), args.outfilename)
    # if pypoem.spec_check(args.outfilename):
    #     print("File is compliant with last POEM Specification v%i" % pypoem.current_standard_poem_version())


if __name__ == '__main__':
    main()
