#!/bin/bash

tag=$(git describe --tags)
cp Doxyfile.in Doxyfile
sed -i "s/@GIT_TAG_VERSION@/$tag/g" Doxyfile
doxygen
