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
import stat
import errno
import sys
import fuse
import syslog
import shutil

from concatenated_file_reader import *
from vdr import *
from filesystemnodes import *

fuse.fuse_python_api = (0, 2)

class VdrNfoFs(fuse.Fuse):

    def __init__(self, *args, **kw):
        fuse.Fuse.__init__(self, *args, **kw)
        self.video = ""

    def get_node(self, path):
        virtual_path, virtual_file_extension = os.path.splitext(path)
        if virtual_file_extension in ['.mpg', '.nfo', '.id3']:
            p = virtual_path.rfind('_')
            if p > 0:
                video_path = self.video + '/' + virtual_path[0:p] + '/' + virtual_path[p+1:]
                # for id3 files we need to cut of the mpg stuff as well.
                if virtual_file_extension == '.id3':
                    video_path, ignore_extension = os.path.splitext(video_path)
                    
                if not os.path.isdir(video_path):
                    return None
                elif virtual_file_extension == '.mpg':
                    return MpgNode(video_path)
                elif virtual_file_extension == '.nfo':
                    return NfoNode(video_path)
                elif virtual_file_extension == '.id3':
                    return Id3Node(video_path)
        else:
            if os.path.isdir(self.video + '/' + path):
                return DirNode(self.video + path)
        return None

    def getattr(self, path):
        try:
            node = self.get_node(path)
            if node:
                return node.get_stat()
            return -errno.ENOENT
        except:
            syslog.syslog('VdrFuseFs: Unexpected error for getattr(%s)' % path)

    def readdir(self, path, offset):
        try:
            yield fuse.Direntry('.')
            yield fuse.Direntry('..')
            node = self.get_node(path)
            if node:
                for item in node.content():
                    yield fuse.Direntry(item.file_system_name)
        except:
            syslog.syslog('VdrFuseFs: Unexpected error for readdir(%s)' % path)

    def open(self, path, flags):
        try:
            node = self.get_node(path)
            if not node:
                return -errno.ENOENT
            accmode = os.O_RDONLY | os.O_WRONLY | os.O_RDWR
            if (flags & accmode) != os.O_RDONLY:
                return -errno.EACCES
        except:
            syslog.syslog('VdrFuseFs: Unexpected error for open(%s)' % path)

    def read(self, path, size, offset):
        try:
            node = self.get_node(path)
            if not node:
                return -errno.ENOENT
            return node.read(offset, size)
        except:
            syslog.syslog('VdrFuseFs: Unexpected error for read(%s)' % path)

    def unlink(self, path):
        try:
            node = self.get_node(path)
            if not node:
                return -errno.ENOENT

            try:
                shutil.rmtree(node.path)
            except: 
                return -errno.EACCES
        except:
            syslog.syslog('VdrFuseFs: Unexpected error for delete(%s)' % path)

def main():
    usage =  "\nVDR-NFO-FS - access VDR recordings as mpg and nfo files\n"
    usage += fuse.Fuse.fusage

    version = "%prog " + fuse.__version__

    fs = VdrNfoFs(version=version,  usage=usage, dash_s_do='setsingle')
    fs.parser.add_option(mountopt="video", default='', help="The video directory containing the VDR recordings")
    fs.parse(values=fs, errex=1)
    fs.main()
