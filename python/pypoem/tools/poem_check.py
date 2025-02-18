#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Check a POEM File against POEM Specifications""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('infilename',
                        help='The file we want to check against POEM specifications')

    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()

    if pypoem.spec_check(args.infilename):
        print("Specification version %i OK" % pypoem.get_version(args.infilename))


if __name__ == '__main__':
    main()
