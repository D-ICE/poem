#!/usr/bin/env python
#  -*- coding: utf-8 -*-

from pypoem import pypoem


def main():
    print("POEM Library info")
    print("\tPOEM Library development status:           %s" % pypoem.poem_library_status())
    print("\tPOEM Library version:                      %s" % pypoem.poem_lib_version())
    print("\tPOEM Library last revision date:           %s" % pypoem.poem_lib_last_commit_date())
    print("\tPOEM file format Specification version:    v%i" % pypoem.current_standard_poem_version())


if __name__ == '__main__':
    main()
