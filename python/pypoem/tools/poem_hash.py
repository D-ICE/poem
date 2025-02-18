#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
import os
import sha256

def get_parser():
    parser = argparse.ArgumentParser(
        description="""Get the SHA256 hash of a POEM File""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    # Please define this argument. We need to have a way to check the version of a tool from command line
    parser.add_argument('infilename',
                        help='The file we want to hash with SHA256 algorithm')

    return parser
def main():
    parser = get_parser()
    args = parser.parse_args()

    if os.path.isfile(args.infilename):
        # print("Generating hash for file %s" % args.infilename)
        hasher = sha256.sha256()
        with open(args.infilename, 'rb') as f:
            buf = f.read()
            hasher.update(buf)
        # print("Hash is: %s" % hasher.hexdigest())
        print(hasher.hexdigest())

    else:
        print("File not found: %s" % args.infilename)


if __name__ == '__main__':
    main()
