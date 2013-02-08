# -*- coding: utf-8 -*-
#
# VDR-NFO-FS creates a file system for VDR recordings, which maps each
# recording to a single mpg-file and nfo-file containing some meta data.
#
# Copyright (C) 2010 by Tobias Grimm
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import os

class ConcatenatedFileReader:
    def __init__(self, filenames):
        self.filenames = filenames

    def read(self, offset, size):
        buffer = ""
        current_offset = offset
        for filename in self.filenames:
            if current_offset > os.path.getsize(filename):
                current_offset -= os.path.getsize(filename)
            else:
                file = open(filename, 'r')
                file.seek(current_offset)
                current_offset = 0
                buffer += file.read(size - len(buffer))
                if len(buffer) == size:
                    return buffer
        return buffer
