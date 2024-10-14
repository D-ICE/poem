# -*- coding: utf-8 -*-
# smichel: test setup following weval as a 'template'

import os
import subprocess  
import sys
import platform
import logging
logging.basicConfig(level=logging.INFO)

import boto3

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.egg_info import egg_info
from setuptools_scm import get_version

module_name = "pypoem"

# branch_name = os.environ.get("CI_COMMIT_BRANCH", "dvlp/pywrapper")
branch_name = "dvlp/pywrapper"

# A CMakeExtension needs a sourcedir instead of a file list.
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        # This is optional - will print a nicer error if CMake is missing.
        # Since we force CMake via PEP 518 in the pyproject.toml, this should
        # never happen and this whole method can be removed in your code if you
        # want.
        try:
            subprocess.check_output(["cmake", "--version"])
        except OSError:
            msg = "CMake missing - probably upgrade to a newer version of Pip?"
            raise RuntimeError(msg)

        # To support Python 2, we have to avoid super(), since distutils is all
        # old-style classes.
        build_ext.run(self)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep
        if not os.path.exists(extdir):
            os.makedirs(extdir, exist_ok=True)

        # try to download pre-build
        try:
            self.__download_prebuild(extdir)
            return
        except Exception as e:
            logging.warning("Couldn't download pre-built: {}".format(str(e)))

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            "-DCMAKE_BUILD_TYPE=Release",
            "-DPOEM_BUILD_TESTS=OFF",
            "-DPOEM_BUILD_TOOLS=OFF",
            "-DPOEM_ALLOW_DIRTY=OFF",
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}".format(extdir)
        ]
        build_args = []


        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp
        )
        subprocess.check_call(
            ["cmake", "--build", "."] + build_args, cwd=self.build_temp
        )

    def __download_prebuild(self, destination):
        if platform.system().lower() == "linux":
            platform_sufix = "linux-gnu"
        else:
            raise Exception("Pre-build are only available on linux")

        version = sys.version_info
        lib_name = "{}.cpython-{}{}-x86_64-{}.so".format(module_name,
            version.major, version.minor, platform_sufix)

        package_version = branch_name # get_version() # smichel for testing 
        
        # Download from s3
        file_key = os.path.join('poem', package_version, lib_name)
        logging.info("Downloading {} from s3".format(file_key))
        s3 = boto3.client('s3')
        s3.download_file(
            'dice-code', file_key, os.path.join(destination, lib_name))


class egg_info_ex(egg_info):
    """Includes license file into `.egg-info` folder."""

    def run(self):
        # don't duplicate license into `.egg-info` when building a distribution
        if not self.distribution.have_run.get('install', True):
            # `install` command is in progress, copy license
            self.mkpath(self.egg_info)
            self.copy_file('LICENSE.txt', self.egg_info)

        egg_info.run(self)

setup(
    name=module_name,
    ext_modules=[CMakeExtension(module_name)],
    cmdclass={"build_ext": CMakeBuild,
              'egg_info': egg_info_ex},
    zip_safe=False,
    license_files = ('LICENSE.txt',),
)
