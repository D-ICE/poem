#!/bin/bash

cp Doxyfile.in Doxyfile
tag=$(git describe --tags)
sed -i "s/@GIT_TAG_VERSION@/$tag/g" Doxyfile
doxygen
rm Doxyfile
