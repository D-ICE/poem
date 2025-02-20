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




if __name__ == '__main__':
    main()
