#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
from pypoem import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Get a ASCII tree view of a POEM File""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('infilename', help='Input file')

    return parser


def generate_tree_dict(tree_dict: dict, poem_node: pypoem.PolarNode):
    # TODO: mettre les descriptions (et l'avoir en option de CLI)
    if poem_node.is_polar_table():
        tree_dict[poem_node.name()] = poem_node.name()
    else:
        tree_dict[poem_node.name()] = dict()

    for poem_node_ in poem_node.children():
        generate_tree_dict(tree_dict[poem_node.name()], poem_node_)


def generate_ascii_tree(tree_dict: dict, ascii_tree: list, prefix='', level=0):
    if type(tree_dict) == str:
        pass

    else:
        for i, item in enumerate(tree_dict):
            if level == 0:
                item_prefix = ''
                next_prefix = ''
            else:
                is_last = (i == len(tree_dict.items()) - 1)
                item_prefix = prefix + ('└── ' if is_last else '├── ')
                next_prefix = prefix + ('    ' if is_last else '│   ')

            new_line = f"{item_prefix}{item}"
            ascii_tree.append(new_line)

            generate_ascii_tree(tree_dict[item], ascii_tree, next_prefix, level + 1)

    return ascii_tree


def execute(filename):
    polar_node = pypoem.load(filename)

    tree_dict = dict()
    generate_tree_dict(tree_dict, polar_node)

    ascii_tree = list()
    generate_ascii_tree(tree_dict, ascii_tree)

    for item in ascii_tree:
        print(item)


def main():
    parser = get_parser()
    args = parser.parse_args()

    execute(args.infilename)


if __name__ == '__main__':
    main()
