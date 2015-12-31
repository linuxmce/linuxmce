# -*- coding: utf-8 -*-

# Licensed under the MIT license
# http://opensource.org/licenses/mit-license.php

# Copyright 2011 sambuca

"""
A Coherence backend that provides a media store using the LinuxMCE
media database as storage.
"""

from twisted.internet import reactor, defer, task

from coherence.extern import db_row
from coherence.upnp.core import DIDLLite
from coherence.backend import BackendItem, BackendStore
from coherence.log import Loggable
import coherence.extern.louie as louie
from urlparse import urlsplit
import urllib2

import MySQLdb

import re
import os
import time

ROOT_CONTAINER_ID = 0
AUDIO_CONTAINER = 100
AUDIO_ALL_CONTAINER_ID = 101
AUDIO_ARTIST_CONTAINER_ID = 102
AUDIO_ALBUM_CONTAINER_ID = 103
AUDIO_PLAYLIST_CONTAINER_ID = 104

VIDEO_CONTAINER = 200
VIDEO_ALL_CONTAINER_ID = 201
VIDEO_PLAYLIST_CONTAINER_ID = 202

# LinuxMCE Constants
ATTRIBUTE_ARTIST = 2
ATTRIBUTE_ALBUM = 3
ATTRIBUTE_TRACK = 5
ATTRIBUTE_TITLE = 13

MEDIATYPE_AUDIO = 4
MEDIATYPE_VIDEO = 5

SQL_AUDIOTRACK_SELECT = "select f.PK_File, CONCAT(f.Path, '/', f.Filename) as Uri, "\
    "aart.PK_Attribute AS ArtistID, aart.Name as Artist, "\
    "aalb.PK_Attribute as AlbumID, aalb.Name as Album, "\
    "atit.PK_Attribute, atit.Name, atra.Name AS TrackNumber "

SQL_AUDIOTRACK_FROM = "FROM File f "
SQL_AUDIOTRACK_JOIN = "LEFT JOIN File_Attribute fart ON f.PK_File = fart.FK_File "\
    "JOIN Attribute aart ON fart.FK_Attribute = aart.PK_Attribute AND aart.FK_AttributeType = %s "\
    "LEFT JOIN File_Attribute falb ON f.PK_File = falb.FK_File "\
    "JOIN Attribute aalb ON falb.FK_Attribute = aalb.PK_Attribute AND aalb.FK_AttributeType = %s "\
    "LEFT JOIN File_Attribute ftit ON f.PK_File=ftit.FK_File "\
    "JOIN Attribute atit ON atit.PK_Attribute = ftit.FK_Attribute AND atit.FK_AttributeType = %s "\
    "LEFT JOIN File_Attribute ftra ON f.PK_File = ftra.FK_File "\
    "JOIN Attribute atra ON ftra.FK_Attribute = atra.PK_Attribute AND atra.FK_AttributeType = %s "

import mimetypes
mimetypes.init()
mimetypes.add_type('audio/x-m4a', '.m4a')
mimetypes.add_type('video/mp4', '.mp4')
mimetypes.add_type('video/mpegts', '.ts')
mimetypes.add_type('video/divx', '.divx')
mimetypes.add_type('video/divx', '.avi')
mimetypes.add_type('video/x-matroska', '.mkv')
mimetypes.add_type('audio/x-musepack', '.mpc')
mimetypes.add_type('audio/flac', '.flac')
mimetypes.add_type('audio/x-wavpack', '.wv')
mimetypes.add_type('audio/mp4', '.m4a')

class LinuxMCEMediaDB(Loggable):
    logCategory = "lmce_media"

    def __init__(self, db_name):
        """ Connect to a db backend hosting the given database.
        """
        Loggable.__init__(self)
        self.db_name = db_name;
        self.connect()

    def disconnect(self):
        self._db.close()

    def connect(self):
        """
        Connect to the database, set L{_db} instance variable.
        """
        self._db = MySQLdb.connect('localhost','root','',self.db_name)

    def reconnect(self):
        """
        Disconnect and reconnect to the database.
        """
        self.disconnect()
        self.connect()

    def sql_execute(self, request, *params, **kw):
        """ Execute a SQL query in the db backend
        """
        t0 = time.time()
        debug_msg = request
        if params:
            debug_msg = u"%s params=%r" % (request, params)
        debug_msg = u''.join(debug_msg.splitlines())
        if debug_msg:
            self.debug('QUERY: %s', debug_msg)

        cursor = self._db.cursor()
        result = []
        cursor.execute(request, params)
        if cursor.description:
            all_rows = cursor.fetchall()
            result = db_row.getdict(all_rows, cursor.description)
        cursor.close()
        delta = time.time() - t0
        self.log("SQL request took %s seconds" % delta)
        return result

class Container(BackendItem):

    get_path = None

    def __init__(self, id, parent_id, name, children_callback=None, store=None,
                 play_container=False):
        self.id = id
        self.parent_id = parent_id
        self.name = name
        self.mimetype = 'directory'
        self.store = store
        self.play_container = play_container
        self.update_id = 0
        if children_callback != None:
            self.children = children_callback
        else:
            self.children = []

    def add_child(self, child):
        self.children.append(child)

    def get_children(self,start=0,request_count=0):
        def got_children(children):
            if request_count == 0:
                return children[start:]
            else:
                return children[start:request_count]

        if callable(self.children):
            dfr = defer.maybeDeferred(self.children)
        else:
            dfr = defer.succeed(self.children)
        dfr.addCallback(got_children)
        return dfr

    def get_child_count(self):
        count = 0
        if callable(self.children):
            count = defer.maybeDeferred(self.children)
            count.addCallback(lambda children: len(children))
        else:
            count = len(self.children)
        return count

    def get_item(self):
        item = DIDLLite.Container(self.id, self.parent_id,self.name)

        def got_count(count):
            item.childCount = count
            if self.store and self.play_container == True:
                if item.childCount > 0:
                    dfr = self.get_children(request_count=1)
                    dfr.addCallback(got_child, item)
                    return dfr
            return item

        def got_child(children, item):
            res = DIDLLite.PlayContainerResource(self.store.server.uuid,
                                                 cid=self.get_id(),
                                                 fid=children[0].get_id())
            item.res.append(res)
            return item


        dfr = defer.maybeDeferred(self.get_child_count)
        dfr.addCallback(got_count)
        return dfr

    def get_name(self):
        return self.name

    def get_id(self):
        return self.id

class Artist(BackendItem):

    def __init__(self, *args, **kwargs):
        BackendItem.__init__(self, *args, **kwargs)
        self._row = args[0]
        self._db = args[1]
        self.itemID = self._row.PK_Attribute
        self.name = self._row.Name or ''
#        if self.name:
#            self.name = self.name.encode("utf-8")

    def get_children(self,start=0, end=0):
        albums = []

        def query_db():
            q = "select DISTINCT aalb.PK_Attribute, aalb.Name from File_Attribute faart, File_Attribute faalb, Attribute aalb, File f "\
                "where faart.FK_Attribute = %s and faart.FK_File = f.PK_File and f.EK_MediaType = %s AND "\
                "aalb.FK_AttributeType = %s AND aalb.PK_Attribute = faalb.FK_Attribute AND faalb.FK_File = f.PK_File "\
                "AND f.Missing != 1 "\
                "ORDER BY aalb.Name"
            rows = self._db.sql_execute(q, self.itemID, MEDIATYPE_AUDIO, ATTRIBUTE_ALBUM)
            for row in rows:
                album = Album(row, self._db)
                albums.append(album)
                yield album

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: albums)
        return dfr

    def get_child_count(self):
        q = "select count(DISTINCT faalb.FK_Attribute) as c from File_Attribute faart, File_Attribute faalb, Attribute aalb, File f "\
            "where faart.FK_Attribute = %s and faart.FK_File = f.PK_File and faalb.FK_File = f.PK_File AND "\
            "aalb.FK_AttributeType = %s AND aalb.PK_Attribute = faalb.FK_Attribute AND f.EK_MediaType = %s "\
            "AND f.Missing != 1 "
        return self._db.sql_execute(q, self.itemID, ATTRIBUTE_ALBUM, MEDIATYPE_AUDIO)[0].c

    def get_item(self):
        item = DIDLLite.MusicArtist(self.get_id(),
                                    AUDIO_ARTIST_CONTAINER_ID, self.name)
        item.childCount = self.get_child_count()
        return item

    def get_id(self):
        return "artist.%d" % self.itemID
    
    def __repr__(self):
        return '<Artist %d name="%s" >' % (self.itemID, self.name)

class Album(BackendItem):
    """ definition for an album """
    mimetype = 'directory'
    get_path = None

    def __init__(self, *args, **kwargs):
        BackendItem.__init__(self, *args, **kwargs)
        self._row = args[0]
        self._db = args[1]
        self.itemID = self._row.PK_Attribute
        self.title = self._row.Name
        self.cover = self.get_cover_path()
#        if self.title:
#            self.title = self.title.encode("utf-8")
        self.cd_count = 1

    def get_children(self,start=0,request_count=0):
        tracks = []

        def query_db():
            q = "select f.PK_File, CONCAT(f.Path, '/', f.Filename) as Uri, IFNULL(atit.Name,f.Filename) as Title "\
                "FROM File f LEFT JOIN File_Attribute ftit ON f.PK_File = ftit.FK_File "\
                "JOIN Attribute atit ON ftit.FK_Attribute = atit.PK_Attribute AND atit.FK_AttributeType = %s "\
                "JOIN File_Attribute falb ON f.PK_File=falb.FK_File AND falb.FK_Attribute = %s "\
                "WHERE f.EK_MediaType = %s "\
                "AND f.Missing != 1 "\
                "ORDER BY Title"
            if request_count:
                q += " limit %d" % request_count
            rows = self._db.sql_execute(q, ATTRIBUTE_TITLE, self.itemID, MEDIATYPE_AUDIO)
            for row in rows:
                track = Track(row, self._db, self)
                tracks.append(track)
                yield track

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: tracks)
        return dfr

    def get_child_count(self):
#TODO: fix, to avoid duplicate entries
        q = "select count(DISTInCT f.PK_File) as c FROM "\
            "File f JOIN File_Attribute falb ON f.PK_File = falb.FK_File AND falb.FK_Attribute = %s "\
            "WHERE f.EK_MediaType = %s "
        count = self._db.sql_execute(q, self.itemID,  MEDIATYPE_AUDIO)[0].c
        return count

    def get_item(self):
        item = DIDLLite.MusicAlbum(self.get_id(), AUDIO_ALBUM_CONTAINER_ID, self.title)
#        item.artist = self.artist.name
        item.childCount = self.get_child_count()
        if self.cover:
            _,ext =  os.path.splitext(self.cover)
            item.albumArtURI = ''.join((self._db.urlbase,
                                        self.get_id(), '?cover', ext))

        def got_tracks(tracks):
            if (len(tracks) > 0):
                res = DIDLLite.PlayContainerResource(self._db.server.uuid,
                                                     cid=self.get_id(),
                                                     fid=tracks[0].get_id())
                item.res.append(res)
            return item

        if item.childCount > 0:
            dfr = self.get_children(request_count=1)
            dfr.addCallback(got_tracks)
        else:
            dfr = defer.succeed(item)
        return dfr

    def get_id(self):
        return "album.%d" % self.itemID

    def get_name(self):
        return self.title

    def get_cover_path(self):
        q = "SELECT FK_Picture from Picture_Attribute WHERE FK_Attribute = %s"
        row = self._db.sql_execute(q, self.itemID)
        path = ''
        if (len(row) > 0):
            path = "/home/mediapics/%d.jpg" % row[0].FK_Picture
        return path

    def get_cover(self):
        return self.cover

    def __repr__(self):
        return '<Album %d title="%s" #cds %d cover="%s">' \
               % (self.itemID, self.title,
                  self.cd_count,
                  self.cover)


class BasePlaylist(BackendItem):
    """ definition for a playlist """
    mimetype = 'directory'
    get_path = None

    def __init__(self, *args, **kwargs):
        BackendItem.__init__(self, *args, **kwargs)
        self._row = args[0]
        self._store = args[1]
        self._db = self._store.db
        self.title = self._row.Name
        if self.title:
            self.title = self.title.encode("utf-8")

    def get_tracks(self, request_count):
        return []

    def db_to_didl(self, row):
        track = Track(row, self._db)
        return track

    def get_id(self):
        return "%s.%d" % (self.id_type, self.db_id)

    def __repr__(self):
        return '<%s %d title="%s">' % (self.__class___.__name__,
                                       self.db_id, self.title)

    def get_children(self, start=0, request_count=0):
        tracks = []

        def query_db():
            rows = self.get_tracks(request_count)
            for row in rows:
                track = self.db_to_didl(row)
                tracks.append(track)
                yield track

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: tracks)
        return dfr

    def get_child_count(self):
        return 0

    def get_item(self):
        item = DIDLLite.PlaylistContainer(self.get_id(),
                                          AUDIO_PLAYLIST_CONTAINER_ID,
                                          self.title)
        item.childCount = self.get_child_count()

        def got_tracks(tracks):
            if (len(tracks) > 0):
                res = DIDLLite.PlayContainerResource(self._db.server.uuid,
                                                 cid=self.get_id(),
                                                 fid=tracks[0].get_id())
                item.res.append(res)
            return item

        if item.childCount > 0:
            dfr = self.get_children(request_count=1)
            dfr.addCallback(got_tracks)
        else:
            dfr = defer.succeed(item)
        return dfr

    def get_name(self):
        return self.title

class MusicPlaylist(BasePlaylist):
    id_type = "musicplaylist"
    mediatype = MEDIATYPE_AUDIO

    @property
    def db_id(self):
        return self._row.PK_Playlist

    def get_tracks(self, request_count):
        q = SQL_AUDIOTRACK_SELECT
        q += "FROM PlaylistEntry pe, File f "
        q += SQL_AUDIOTRACK_JOIN
        q += "WHERE f.EK_MediaType = %s AND "\
            "f.PK_File = pe.FK_File AND pe.FK_Playlist = %s "\
            "AND f.Missing != 1 "
        if request_count:
            q += " limit %d" % request_count
            
        
        return self._db.sql_execute(q, ATTRIBUTE_ARTIST, ATTRIBUTE_ALBUM, ATTRIBUTE_TITLE, ATTRIBUTE_TRACK, self.mediatype, self.db_id)

    def get_child_count(self):
        q = "select count(e.FK_File) from PlaylistEntry e WHERE e.FK_Playlist = %s"
        return self._db.sql_execute(q, self.db_id)

# Not in use atm. Would have to be added to LinuxMCE too first
class MusicSmartPlaylist(BasePlaylist):
    id_type = "musicsmartplaylist"

    @property
    def db_id(self):
        return self._row.SmartPlaylistID

    def get_tracks(self, request_count):
        q = "select * from CoreTracks where TrackID in (select TrackID "\
            "from CoreSmartPlaylistEntries where SmartPlaylistID=?)"
        if request_count:
            q += " limit %d" % request_count
        return self._db.sql_execute(q, self.db_id)

class VideoPlaylist(MusicPlaylist):
    id_type = "videoplaylist"
    mediatype = MEDIATYPE_VIDEO

    def db_to_didl(self, row):
        return Video(row, self._db)

# Not in use atm. Would have to be added to LinuxMCE too first
class VideoSmartPlaylist(MusicSmartPlaylist):
    id_type = "videosmartplaylist"

    def db_to_didl(self, row):
        return Video(row, self._db)

class BaseTrack(BackendItem):
    """ definition for a track """

    def __init__(self, *args, **kwargs):
        BackendItem.__init__(self, *args, **kwargs)
        self._row = args[0]
        self._db = args[1]
        self.itemID = self._row.PK_File
        self.title = self._row.Title
        self.location = self._row.Uri
        self.playlist = kwargs.get("playlist")

    def get_children(self,start=0,request_count=0):
        return []

    def get_child_count(self):
        return 0

    def get_resources(self):
        resources = []
        _,host_port,_,_,_ = urlsplit(self._db.urlbase)
        if host_port.find(':') != -1:
            host,port = tuple(host_port.split(':'))
        else:
            host = host_port

        _,ext =  os.path.splitext(self.location)
        ext = ext.lower()

        # FIXME: drop this hack when we switch to tagbin
        mimetype, dummy = mimetypes.guess_type("dummy%s" % ext)
#        if not mimetype:
#            mimetype = 'audio/mpeg'
#            ext = "mp3"

        statinfo = os.stat(self.get_path())

        res = DIDLLite.Resource(self.location, 'internal:%s:%s:*' % (host,
                                                                     mimetype))
        try:
            res.size = statinfo.st_size
        except:
            res.size = 0

        resources.append(res)

        url = "%s%s%s" % (self._db.urlbase, self.get_id(), ext)

        res = DIDLLite.Resource(url, 'http-get:*:%s:*' % mimetype)
        try:
            res.size = statinfo.st_size
        except:
            res.size = 0
        resources.append(res)
        return statinfo, resources

    def get_path(self):
        return self.location

    def get_id(self):
        return "track.%d" % self.itemID

    def get_name(self):
        return self.title

    def get_url(self):
        return self._db.urlbase + str(self.itemID).encode('utf-8')

    def get_cover(self):
        return self.album.cover

    def __repr__(self):
        return '<Track %d title="%s" nr="%d" album="%s" artist="%s" path="%s">' \
               % (self.itemID, self.title , self.track_nr, self.album.title,
                  self.artistName, self.location)

class Track(BaseTrack):

    def __init__(self, *args, **kwargs):
        BaseTrack.__init__(self, *args, **kwargs)
        self.artistName = '<unknown>'
        self.track_nr = 0;
        # TODO: cache album entries?
        rows = self.get_metadata(ATTRIBUTE_ALBUM)
        if (len(rows) > 0):
            self.album = Album(rows[0], self._db)
        rows = self.get_metadata(ATTRIBUTE_ARTIST)
        if (len(rows) > 0):
            self.artistName = rows[0].Name
        rows = self.get_metadata(ATTRIBUTE_TRACK)
        if (len(rows) > 0):
            self.track_nr = rows[0].Name

    def get_metadata(self, attributeType):
        q = "select PK_Attribute, Name from File_Attribute fa JOIN Attribute a ON fa.FK_Attribute = a.PK_Attribute "\
            "WHERE fa.FK_File = %s AND a.FK_AttributeType = %s"
        return self._db.sql_execute(q,self.itemID, attributeType)

    def get_item(self):
        item = DIDLLite.MusicTrack(self.get_id(), self.album.itemID,self.title)
        item.artist = self.artistName
        item.album = self.album.title
        item.playlist = self.playlist

        if self.album.cover != '':
            _,ext =  os.path.splitext(self.album.cover)
            """ add the cover image extension to help clients not reacting on
                the mimetype """
            item.albumArtURI = ''.join((self._db.urlbase, self.get_id(),
                                        '?cover',ext))
        item.originalTrackNumber = self.track_nr
        item.server_uuid = str(self._db.server.uuid)[5:]

        statinfo, resources = self.get_resources()
        item.res.extend(resources)

        try:
            # FIXME: getmtime is deprecated in Twisted 2.6
            item.date = datetime.fromtimestamp(statinfo.st_mtime)
        except:
            item.date = None

        return item

class Video(BaseTrack):

    def get_item(self):
        item = DIDLLite.VideoItem(self.get_id(), VIDEO_ALL_CONTAINER_ID,
                                  self.title)
        item.server_uuid = str(self._db.server.uuid)[5:]

        statinfo, resources = self.get_resources()
        item.res.extend(resources)

        try:
            # FIXME: getmtime is deprecated in Twisted 2.6
            item.date = datetime.fromtimestamp(statinfo.st_mtime)
        except:
            item.date = None

        return item

    def get_id(self):
        return "video.%d" % self.itemID

class LinuxMCEDB(Loggable):
    logCategory = "linuxmce_db"

    def __init__(self, db_name=None):
        Loggable.__init__(self)
        self.db_name = db_name

    def open_db(self):
        self.db = LinuxMCEMediaDB(self.db_name)

    def close(self):
        self.db.disconnect()

    def get_artists(self):
        artists = []

        def query_db():
            # Only return artists that has files
            q = "select DISTINCT a.PK_Attribute,a.Name from Attribute a, "\
                "File_Attribute fa, File f "\
                "WHERE a.PK_Attribute = fa.FK_Attribute AND fa.FK_File = f.PK_File AND a.FK_AttributeType = %s AND f.EK_MediaType = %s "\
                "AND f.Missing != 1 "\
                "order by a.Name"
            for row in self.db.sql_execute(q, ATTRIBUTE_ARTIST, MEDIATYPE_AUDIO):
                artist = Artist(row, self.db)
                artists.append(artist)
                yield artist

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: artists)
        return dfr

    def get_albums(self):
        albums = []
        artists = {}

        def query_db():
            # Only return album that has files
            q = "select DISTINCT a.PK_Attribute,a.Name from Attribute a "\
                "JOIN File_Attribute fa ON a.PK_Attribute = fa.FK_Attribute "\
                "JOIN File f ON fa.FK_File = f.PK_File where a.FK_AttributeType = %s AND f.EK_MediaType = %s "\
                "AND f.Missing != 1 "\
                "order by a.Name"
            for row in self.db.sql_execute(q, ATTRIBUTE_ALBUM, MEDIATYPE_AUDIO):
                album = Album(row, self.db)
                albums.append(album)
                yield album

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: albums)
        return dfr

    def get_music_playlists(self):
        return self.get_playlists(MusicPlaylist, MusicSmartPlaylist)

    def get_playlists(self, PlaylistClass, SmartPlaylistClass):
        playlists = []

        def query_db():
            q = "select distinct p.PK_Playlist, p.Name from Playlist p, PlaylistEntry pe, File f WHERE "\
                "p.PK_Playlist = pe.FK_Playlist AND pe.FK_File = f.PK_File and f.EK_MediaType = %s "\
                "AND f.Missing != 1 "\
                "order by Name"
            for row in self.db.sql_execute(q, PlaylistClass.mediatype):
                playlist = PlaylistClass(row, self)
                playlists.append(playlist)
                yield playlist

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: playlists)
        return dfr

    def get_artist_with_id(self, artist_id):
        q = "select PK_Attribute,Name from Attribute where PK_Attribute = %s AND FK_AttributeType = %s"
        
        row = self.db.sql_execute(q, artist_id, ATTRIBUTE_ARTIST)[0]
        return Artist(row, self.db)

    def get_album_with_id(self, album_id):
        q = "select PK_Attribute,Name from Attribute where PK_Attribute = %s AND FK_AttributeType = %s"
        row = self.db.sql_execute(q, album_id, ATTRIBUTE_ALBUM)[0]
        return Album(row, self.db)

    def get_playlist_with_id(self, playlist_id, PlaylistClass):
        q = "select PK_Playlist, Name from Playlist where PK_Playlist = %s limit 1"
        row = self.db.sql_execute(q, playlist_id)[0]
        return PlaylistClass(row, self)

    def get_smart_playlist_with_id(self, playlist_id, PlaylistClass):
        q = "select * from CoreSmartPlaylists where SmartPlaylistID=? limit 1"
        row = self.db.sql_execute(q, playlist_id)[0]
        return PlaylistClass(row, self)

    def get_music_playlist_with_id(self, playlist_id):
        return self.get_playlist_with_id(playlist_id, MusicPlaylist)

    def get_music_smart_playlist_with_id(self, playlist_id):
        return self.get_smart_playlist_with_id(playlist_id, MusicSmartPlaylist)

    def get_video_playlist_with_id(self, playlist_id):
        return self.get_playlist_with_id(playlist_id, VideoPlaylist)

    def get_video_smart_playlist_with_id(self, playlist_id):
        return self.get_smart_playlist_with_id(playlist_id, VideoSmartPlaylist)

    def get_track_with_id(self, track_id):
        q = "select f.PK_File, CONCAT(f.Path, '/', f.Filename) as Uri, IFNULL(atit.Name,f.Filename) as Title "\
            "FROM File f LEFT JOIN File_Attribute ftit ON f.PK_File = ftit.FK_File "\
            "JOIN Attribute atit ON ftit.FK_Attribute = atit.PK_Attribute AND atit.FK_AttributeType = %s "\
            "WHERE f.EK_MediaType = %s AND PK_File = %s "\
            "AND f.Missing != 1 "\
            "ORDER BY Title"
        row = self.db.sql_execute(q, ATTRIBUTE_TITLE, MEDIATYPE_AUDIO, track_id)[0]
        return Track(row, self.db)

    def get_track_for_uri(self, track_uri):
        q = "select * from CoreTracks where Uri=? limit 1"
        try:
            row = self.db.sql_execute(q, track_uri)[0]
        except IndexError:
            # not found
            track = None
        else:
            album = self.get_album_with_id(row.AlbumID)
            track = Track(row, self, album)
        return track

    def get_tracks(self):
        tracks = []

        def query_db():
            q = "select f.PK_File, CONCAT(f.Path, '/', f.Filename) as Uri, IFNULL(atit.Name,f.Filename) as Title "\
                "FROM File f LEFT JOIN File_Attribute ftit ON f.PK_File = ftit.FK_File "\
                "JOIN Attribute atit ON ftit.FK_Attribute = atit.PK_Attribute AND atit.FK_AttributeType = %s "\
                "WHERE f.EK_MediaType = %s "\
                "AND f.Missing != 1 "\
                "ORDER BY Title"
            for row in self.db.sql_execute(q, ATTRIBUTE_TITLE, MEDIATYPE_AUDIO):
                track = Track(row, self.db)
                tracks.append(track)
                yield track

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: tracks)
        return dfr

    def get_video_with_id(self, video_id):
        q = "select f.PK_File, IFNULL(atit.Name,f.Filename) as Title, CONCAT(f.Path,'/', f.Filename) as Uri "\
            "from File f LEFT JOIN File_Attribute fatit ON f.PK_File = fatit.FK_File "\
            "JOIN Attribute atit ON fatit.FK_Attribute = atit.PK_Attribute "\
            "where f.PK_File = %s "\
            "AND f.Missing != 1 "
        row = self.db.sql_execute(q, video_id)[0]
        return Video(row, self.db)

    def get_videos(self):
        videos = []

        def query_db():
            q = "select f.PK_File, IFNULL(atit.Name,f.Filename) as Title, CONCAT(f.Path,'/', f.Filename) as Uri "\
                "from File f LEFT JOIN File_Attribute fatit ON f.PK_File = fatit.FK_File "\
                "JOIN Attribute atit ON fatit.FK_Attribute = atit.PK_Attribute WHERE f.EK_MediaType = %s "\
                "AND f.Missing != 1 "
            for row in self.db.sql_execute(q, MEDIATYPE_VIDEO):
                video = Video(row, self.db)
                videos.append(video)
                yield video

        dfr = task.coiterate(query_db())
        dfr.addCallback(lambda gen: videos)
        return dfr

    def get_video_playlists(self):
        return self.get_playlists(VideoPlaylist, VideoSmartPlaylist)

class LinuxMCEStore(BackendStore, LinuxMCEDB):
    logCategory = 'linuxmce_store'
    implements = ['MediaServer']

    def __init__(self, server, **kwargs):
        BackendStore.__init__(self,server,**kwargs)
        LinuxMCEDB.__init__(self, kwargs.get("db_name", "pluto_media"))
        self.update_id = 0
        self.name = kwargs.get('name', 'LinuxMCE')

        self.containers = {}
        self.containers[ROOT_CONTAINER_ID] = Container(ROOT_CONTAINER_ID,
                                                       -1, self.name, store=self)
        louie.send('Coherence.UPnP.Backend.init_completed', None, backend=self)

    def upnp_init(self):
        self.open_db()

        music = Container(AUDIO_CONTAINER, ROOT_CONTAINER_ID,
                          'Music', store=self)
        self.containers[ROOT_CONTAINER_ID].add_child(music)
        self.containers[AUDIO_CONTAINER] = music

        artists = Container(AUDIO_ARTIST_CONTAINER_ID, AUDIO_CONTAINER,
                            'Artists', children_callback=self.get_artists,
                            store=self)
        self.containers[AUDIO_ARTIST_CONTAINER_ID] = artists
        self.containers[AUDIO_CONTAINER].add_child(artists)

        albums = Container(AUDIO_ALBUM_CONTAINER_ID, AUDIO_CONTAINER,
                           'Albums', children_callback=self.get_albums,
                           store=self)
        self.containers[AUDIO_ALBUM_CONTAINER_ID] = albums
        self.containers[AUDIO_CONTAINER].add_child(albums)

        tracks = Container(AUDIO_ALL_CONTAINER_ID, AUDIO_CONTAINER,
                           'All tracks', children_callback=self.get_tracks,
                           play_container=True, store=self)
        self.containers[AUDIO_ALL_CONTAINER_ID] = tracks
        self.containers[AUDIO_CONTAINER].add_child(tracks)

        playlists = Container(AUDIO_PLAYLIST_CONTAINER_ID, AUDIO_CONTAINER,
                              'Playlists', store=self,
                              children_callback=self.get_music_playlists)
        self.containers[AUDIO_PLAYLIST_CONTAINER_ID] = playlists
        self.containers[AUDIO_CONTAINER].add_child(playlists)

        videos = Container(VIDEO_CONTAINER, ROOT_CONTAINER_ID,
                          'Videos', store=self)
        self.containers[ROOT_CONTAINER_ID].add_child(videos)
        self.containers[VIDEO_CONTAINER] = videos

        all_videos = Container(VIDEO_ALL_CONTAINER_ID, VIDEO_CONTAINER,
                               'All Videos', children_callback=self.get_videos,
                               store=self)
        self.containers[VIDEO_ALL_CONTAINER_ID] = all_videos
        self.containers[VIDEO_CONTAINER].add_child(all_videos)

        playlists = Container(VIDEO_PLAYLIST_CONTAINER_ID, VIDEO_CONTAINER,
                              'Playlists', store=self,
                              children_callback=self.get_video_playlists)
        self.containers[VIDEO_PLAYLIST_CONTAINER_ID] = playlists
        self.containers[VIDEO_CONTAINER].add_child(playlists)


        self.db.server = self.server
        self.db.urlbase = self.urlbase
        self.db.containers = self.containers

        self.current_connection_id = None
        if self.server:
            hostname = self.server.coherence.hostname
            source_protocol_info = ['internal:%s:audio/mpeg:*' % hostname,
                                    'http-get:*:audio/mpeg:*',
                                    'internal:%s:application/ogg:*' % hostname,
                                    'http-get:*:application/ogg:*']

            self.server.connection_manager_server.set_variable(0,
                                                               'SourceProtocolInfo',
                                                               source_protocol_info,
                                                               default=True)

    def release(self):
        self.db.disconnect()

    def get_by_id(self,item_id):
        self.info("get_by_id %s" % item_id)
        if isinstance(item_id, basestring) and item_id.find('.') > 0:
            item_id = item_id.split('@',1)
            item_type, item_id = item_id[0].split('.')[:2]
            item_id = int(item_id)
            dfr = self._lookup(item_type, item_id)
        else:
            item_id = int(item_id)
            item = self.containers[item_id]
            dfr = defer.succeed(item)
        return dfr

    def _lookup(self, item_type, item_id):
        lookup_mapping = dict(artist=self.get_artist_with_id,
                              album=self.get_album_with_id,
                              musicplaylist=self.get_music_playlist_with_id,
#                              musicsmartplaylist=self.get_music_smart_playlist_with_id,
                              videoplaylist=self.get_video_playlist_with_id,
#                              videosmartplaylist=self.get_video_smart_playlist_with_id,
                              track=self.get_track_with_id,
                              video=self.get_video_with_id)
        item = None
        func = lookup_mapping.get(item_type)
        if func:
            item = func(item_id)
        return defer.succeed(item)
