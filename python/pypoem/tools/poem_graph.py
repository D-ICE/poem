#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import argparse
import pydot
from pypoem import pypoem
# import pypoem


def get_parser():
    parser = argparse.ArgumentParser(
        description="""Get a graph from a POEM File""",
        formatter_class=argparse.RawTextHelpFormatter
    )
    # Please define this argument. We need to have a way to check the version of a tool from command line
    parser.add_argument('infilename',
                        help='Input file')

    return parser


def populate_graph(graph: pydot.Graph, parent_dot_node: pydot.Node, poem_node: pypoem.PolarNode):
    new_dot_node_name = poem_node.name()
    new_dot_node = pydot.Node(new_dot_node_name, label=new_dot_node_name)
    graph.add_node(new_dot_node)


    if parent_dot_node is not None:
        graph.add_edge(pydot.Edge(parent_dot_node.get_name(), new_dot_node_name))

    for poem_node_ in poem_node.children():
        populate_graph(graph, new_dot_node, poem_node_)


def main():
    parser = get_parser()
    args = parser.parse_args()

    polar_node = pypoem.load(args.infilename, "vessel")

    graph = pydot.Dot("vessel", graph_type="graph")
    populate_graph(graph, None, polar_node)

    print(graph.to_string())
    # graph.write_png("graph.png")


if __name__ == '__main__':
    main()
