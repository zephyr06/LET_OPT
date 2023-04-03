# ------------------------------------------------------------------------------
# Licensed Materials - Property of IBM
# 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
# Copyright IBM Corporation 2008, 2022. All Rights Reserved.
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
# ------------------------------------------------------------------------------
"""Imports the shared library on supported platforms."""

import platform

from sys import version_info

ERROR_STRING = "CPLEX 22.1.0.0 is not compatible with this version of Python."

if platform.system() in ('Darwin', 'Linux', 'AIX', 'Windows', 'Microsoft'):
    if version_info < (3, 7, 0):
        raise Exception(ERROR_STRING)
    elif version_info < (3, 8, 0):
        from cplex._internal.py37_cplex2210 import *
    elif version_info < (3, 9, 0):
        from cplex._internal.py38_cplex2210 import *
    elif version_info < (3, 10, 0):
        from cplex._internal.py39_cplex2210 import *
    elif version_info < (3, 11, 0):
        from cplex._internal.py310_cplex2210 import *
    else:
        raise Exception(ERROR_STRING)
else:
    raise Exception("The CPLEX Python API is not supported on this platform.")
