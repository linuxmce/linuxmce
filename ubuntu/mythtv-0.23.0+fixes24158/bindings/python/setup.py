#!/usr/bin/env python

from distutils.core import setup

setup(
        name='MythTV',
        version='0.23.0',
        description='MythTV Python bindings.',
        packages=['MythTV', 'MythTV/tmdb', 'MythTV/ttvdb'],
        url=['http://www.mythtv.org/'],
        )
