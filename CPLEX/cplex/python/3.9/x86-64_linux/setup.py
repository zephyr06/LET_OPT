#!/usr/bin/env python
# ------------------------------------------------------------------------------
# Licensed Materials - Property of IBM
# 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
# Copyright IBM Corporation 2008, 2022. All Rights Reserved.
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
# ------------------------------------------------------------------------------
"""setup.py file for the CPLEX Python API"""

import platform

from distutils.core import setup
from sys import version_info


def check_platform():
    # The cpxsys string is injected by build scripts.
    cpxsys = 'x86-64_linux'
    pysys = platform.system()
    if cpxsys in ('x86-64_linux', 'ppc64le_linux', 's390x_linux'):
        expectedsys = ('Linux',)
    elif cpxsys in ('x86-64_osx',):
        expectedsys = ('Darwin',)
    elif cpxsys in ('x64_win64',):
        expectedsys = ('Windows', 'Microsoft')
    elif cpxsys in ('power7-64_aix', 'power7-64_aix_xlclang'):
        expectedsys = ('AIX',)
    else:
        raise Exception('unexpected cpxsys: {0}'.format(cpxsys))
    if pysys not in expectedsys:
        raise Exception(
            "this 'cplex' package (version 22.1.0.0) is not compatible " +
            "with this platform: expecting '{0}' but got '{1}'".format(
                ", ".join(expectedsys), pysys))


def check_version():
    # The expected python version string is injected by build scripts.
    expected_major, expected_minor = [int(x) for x in '3.9'.split('.')]
    actual_major, actual_minor, _, _, _ = version_info
    if (expected_major, expected_minor) != (actual_major, actual_minor):
        raise Exception(
            "this 'cplex' package (version 22.1.0.0) is not compatible " +
            "with this version of Python: " +
            "expected {0}.{1} but got {2}.{3}".format(
                expected_major, expected_minor, actual_major, actual_minor))


def is_windows():
    return platform.system() in ('Windows', 'Microsoft')
def is_mac():
    return platform.system() in ('Darwin')


def get_data():
    major, minor, _, _, _ = version_info
    # The expected CPLEX version string is injected by build scripts.
    if is_windows():
        ext = '.pyd'
        extra = ['cplex2210.dll']
    else:
        if is_mac():
            ext = '.so'
            extra = ['libcplex2210.dylib']
        else:
            ext = '.so'
            extra = ['libcplex2210.so']

    data = ['py{0:d}{1:d}_cplex2210{2:s}'.format(major, minor, ext)]
    data.extend(extra)
    return data


def main():
    check_platform()
    check_version()
    setup(
        name='cplex',
        version='22.1.0.0',
        author='IBM',
        description='A Python interface to the CPLEX Callable Library.',
        packages=['cplex',
                  'cplex._internal',
                  'cplex.exceptions'],
        package_dir={'cplex': 'cplex',
                     'cplex._internal': 'cplex/_internal',
                     'cplex.exceptions': 'cplex/exceptions'},
        package_data={'cplex._internal': get_data()},
        url='https://www.ibm.com/products/ilog-cplex-optimization-studio',
        zip_safe=False
    )


if __name__ == '__main__':
    main()
