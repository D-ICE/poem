#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description=""" -- polar_hash --""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    # Please define this argument. We need to have a way to check the version of a tool from command line
    parser.add_argument('infilename',
                        help='The file we want to hash with SHA256 algorithm')

    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()

    if pypoem.spec_check(args.infilename):
        print("OK")


if __name__ == '__main__':
    main()
