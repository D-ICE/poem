#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
import os.path
import sys

from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Mount a POEM file on another one giving paths for mount points""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("from_file", help='FROM file as file.nc:/path')
    parser.add_argument("to_file", help='TO file file.nc:/path')

    return parser


"""
TODO: si to n'existe pas, on le cree et on construit son arborescence de to_path
 pour ca il faut analyser le what pour lequel le fichier et le path doivent exister. Suivant le node type
 du path what, on ajuste le type de feuille qu'on cree dans le to
 si c'est un polarnode ou un polarset, on monte sur du polarnode
 si c'est un polar, on regarde dans to si une polar du meme type n'existe pas
 
"""

def main():
    parser = get_parser()
    args = parser.parse_args()

    from_file, from_path = args.from_file.split(':')
    from_root_node = pypoem.load(from_file, True, False)
    if not from_root_node.exists(from_path):
        raise ValueError

    from_node = from_root_node.polar_node_from_path(from_path)


    to_file, to_path = args.to_file.split(':')
    to_root_node = pypoem.load(to_file, True, False)
    if not to_root_node.exists(to_path):
        raise ValueError

    to_node = to_root_node.polar_node_from_path(to_path)
    if to_node.is_polar_node():
        if not from_node.is_polar_node() or not from_node.is_polar_set():
            raise ValueError("Only a PolarNode or a PolarSet can be mounted on a PolarNode")
    elif to_node.is_polar_set():
        if not from_node.is_polar():
            raise ValueError("Only a Polar can be mounted on a PolarSet")
    elif to_node.is_polar():
        if not from_node.is_polar_table():
            raise ValueError("Only a PolarTable can be mounted on a Polar")
    else:
        raise ValueError

    # We can only mount
    # PolarNode on PolarNode or root if it is a PolarNode
    # PolarSet on PolarNode or root if it is a PolarNode
    # Polar on PolarSet or root if it is a PolarSet (the PolarSet must not have another Polar already of the same mode)
    # PolarTable on Polar or root if it is a Polar. DimensionGrid MUST be identical




if __name__ == '__main__':
    main()
