# Licensed under the MIT license
# http://opensource.org/licenses/mit-license.php

# Backend to retrieve the live video streams from VDR TV

# Copyright 2007, Frank Scholz <coherence@beebits.net>
# Copyright 2008,2009 Jean-Michel Sizun <jmDOTsizunATfreeDOTfr>
# Copyright 2010 Michael Breu <Michael.BreuATarctis.at>

from twisted.internet import defer,reactor
from twisted.web import server

from coherence.upnp.core import utils

from coherence.upnp.core import DIDLLite

from coherence.extern.simple_plugin import Plugin

from coherence import log
from coherence.backend import BackendItem, BackendStore

from urlparse import urlsplit

import zlib
import re
import traceback
import time

from coherence.backend import BackendStore,BackendItem


ROOT_CONTAINER_ID = 0

VDR_WS_URL = 'http://arctisserver2:3000/TS/'
VIDEO_MIMETYPE = 'video/mpegts'

def funcInfo(object, spacing=10, collapse=1): 
    """Print methods and doc strings.
    
    Takes module, class, list, dictionary, or string."""
    methodList = [method for method in dir(object) if callable(getattr(object, method))]
    processFunc = collapse and (lambda s: " ".join(s.split())) or (lambda s: s)
    print "\n".join(["%s %s" %
                      (method.ljust(spacing),
                       processFunc(str(getattr(object, method).__doc__)))
                     for method in methodList])


class ProxyStream(utils.ReverseProxyUriResource, log.Loggable):
    logCategory = 'vdr'

    stream_url = None

    def __init__(self, uri):
#        self.info("ProxyStream created")
        self.stream_url = None
        utils.ReverseProxyUriResource.__init__(self, uri)

    def requestFinished(self, result):
        """ self.connection is set in utils.ReverseProxyResource.render """
        self.info("ProxyStream requestFinished")
#        funcInfo(self)
        self.info("ProxyStream reactor")
#        funcInfo(self.reactor)
#        self.reactor.removeAll()
#        self.reactor.disconnectAll()
        if self.connector is not None:
            self.info("disconnect")
            self.connector.disconnect()

    def render(self, request):
        self.info("ProxyStream rendering")

        if request.clientproto == 'HTTP/1.1':
            self.info("ProxyStream clientProto is HTTP/1.1")
            d = request.notifyFinish()
            d.addBoth(self.requestFinished)
        else:
            self.info("ProxyStream clientProto is not HTTP/1.1")
            d = request.notifyFinish()
            d.addBoth(self.requestFinished)
        return utils.ReverseProxyUriResource.render(self, request)


class Container(BackendItem):

    logCategory = 'vdr'

    def __init__(self, id, store, parent_id, title):
        self.url = store.urlbase+str(id)
        self.parent_id = parent_id
        self.id = id
        self.name = title
        self.mimetype = 'directory'
        self.update_id = 0
        self.children = []
        self.store = store

        self.item = DIDLLite.Container(self.id, self.parent_id, self.name)
        self.item.childCount = 0

        self.sorted = False
        self.timestamp = time.time()

    def add_child(self, child):
        id = child.id
        if isinstance(child.id, basestring):
            _,id = child.id.split('.')
        if self.children is None:
            self.children = []
        self.children.append(child)
        self.item.childCount += 1
        self.sorted = False
        if self.id == ROOT_CONTAINER_ID:
          self.debug('adding ' + child.name)

    def get_children(self, start=0, end=0):
        '''
        if self.sorted == False:
            def childs_sort(x,y):
                r = cmp(x.name,y.name)
                return r

            self.children.sort(cmp=childs_sort)
            self.sorted = True
        '''
        
        if self.id == ROOT_CONTAINER_ID:
          self.update()

        if end != 0:
            return self.children[start:end]
        return self.children[start:]

    def get_child_count(self):
        if self.id == ROOT_CONTAINER_ID:
          self.update()
        return self.internal_child_count()

    def internal_child_count(self):
        if self.children is None:
          return 0
        return len(self.children)

    def get_path(self):
        return self.url

    def get_item(self):
        return self.item

    def get_name(self):
        return self.name

    def get_id(self):
        return self.id

    def update(self):
        self.debug('get_children for rootcontainer called')
        cachetime = 5.0  # seconds
        if self.timestamp + cachetime < time.time() or self.internal_child_count()==0:
             self.debug('update children')
             self.timestamp = time.time()
             self.store.retrieveList(self)


class VDRItem(BackendItem):
    logCategory = 'vdr'
    
    def __init__(self, store, id, obj, parent):
        self.parent = parent
        self.id = id
        self.name = obj.get('name')
        self.mimetype = obj.get('mimetype')
        self.description = None
        self.date = None
        self.item = None
        self.duration = None
        self.store = store
        self.url = self.store.urlbase + str(self.id)
        self.stream_url = obj.get('url')
        self.location = ProxyStream(self.stream_url)

    def get_item(self):
        if self.item == None:
            _,host_port,_,_,_ = urlsplit(self.store.urlbase)
            if host_port.find(':') != -1:
              host,port = tuple(host_port.split(':'))
            else:
              host = host_port


            self.item = DIDLLite.VideoItem(self.id, self.parent.id, self.name)
            self.item.description = self.description
            self.item.date = self.date
            res = DIDLLite.Resource(self.url, 'http-get:*:%s:*' % self.mimetype) 
            res.duration = self.duration
            #res.size = 0 #None
            self.item.res.append(res)

            res = DIDLLite.Resource(self.url, 'internal:%s:%s:*' % self.mimetype) 
            res.duration = self.duration
            #res.size = 0 #None
            self.item.res.append(res)
        return self.item

    def get_path(self):
        return self.url


class VDRStream(BackendStore):

    logCategory = 'vdr'

    implements = ['MediaServer']

    description = ('VDR Streaming Server', 'Exposes the VDR Streams', None)

    options = [{'option':'name', 'text':'Server Name:', 'type':'string','default':'my media','help': 'the name under this MediaServer shall show up with on other UPnP clients'},
       {'option':'version','text':'UPnP Version:','type':'int','default':2,'enum': (2,1),'help': 'the highest UPnP version this MediaServer shall support','level':'advance'},
       {'option':'uuid','text':'UUID Identifier:','type':'string','help':'the unique (UPnP) identifier for this MediaServer, usually automatically set','level':'advance'},    
       {'option':'streamdevURL','text':'VDR streamdev server URL','type':'string', 'default':VDR_WS_URL}
    ]

    def __init__(self, server, **kwargs):
        BackendStore.__init__(self,server,**kwargs)
        self.next_id = 1000
        self.config = kwargs
        self.name = kwargs.get('name','vdr')

        self.update_id = 0
        self.store = {}

        self.wmc_mapping = {'4': 1000}


        self.VDR_ws_url = self.config.get('streamdevURL',VDR_WS_URL)

        self.init_completed()


    def __repr__(self):
        return self.__class__.__name__

    def storeItem(self, parent, item, id):
        self.store[id] = item
        parent.add_child(item)

    def appendGenre( self, genre, parent):
        id = self.getnextID()
        item = Container(id, self, -1, genre)
        self.storeItem(parent, item, id)
        return item

    def appendFeed( self, obj, parent):
        id = self.getnextID()
        item = VDRItem(self, id, obj, parent)
        self.storeItem(parent, item, id)
        return item

    def len(self):
        return len(self.store)

    def get_by_id(self,id):
        if isinstance(id, basestring):
            id = id.split('@',1)
            id = id[0]
        try:
            return self.store[int(id)]
        except (ValueError,KeyError):
            pass
        return None

    def getnextID(self):
        ret = self.next_id
        self.next_id += 1
        return ret

    def upnp_init(self):
        self.current_connection_id = None

        if self.server:
            self.server.connection_manager_server.set_variable(0, 'SourceProtocolInfo',
                                                                    ['http-get:*:%s:*' % VIDEO_MIMETYPE,
                                                                     'internal:%s:%s:*' % (self.server.coherence.hostname,VIDEO_MIMETYPE),
                                                                     ],
                                                                   default=True)
        rootItem = Container(ROOT_CONTAINER_ID,self,-1, self.name)
        self.store[ROOT_CONTAINER_ID] = rootItem
        self.retrieveList_attemptCount = 0
        self.retrieveList(rootItem)

    def retrieveList(self, rootContainer):
        self.info("Retrieving VDR TV listing...")

        def got_page(result):
            if self.retrieveList_attemptCount == 0:
                self.info("Connection to VDR streamdev server successful for TV listing")
            else:
                self.warning("Connection to VDR streamdev server successful for TV listing after %d attempts." % self.retrieveList_attemptCount)
            result = result[0]

            matches = re.findall('<h2>([^<]+)</h2>', result)
            self.info("got matches %r", matches)

#               self.info("got match %r", match.group(1))
#               self.info("  got matches %r", match.group(2))

            genres = []
            channels = []

	    for genrematch in re.finditer('<div class="group"><h2>([^<]+)</h2>(.*?)</div>', result, re.DOTALL):
               genre = genrematch.group(1)
#               self.info("got genre %r", genre)
	       for match in re.finditer('<a href="([^ ]+?)" vod  tvid="\d+">(.*?)</a>', genrematch.group(2), re.DOTALL):
                   channel_id = match.group(1)
                   name = match.group(2)
#                   self.info("got channel URL %r", channel_id)
#                   self.info("  got channel name %r", name)
     

                   mimetype = VIDEO_MIMETYPE
                   url = (self.VDR_ws_url+'%s') % (channel_id)

                   if genres.count(genre) == 0:
                       genres.append(genre)

                   channel = {'name':name,
                              'channel_id':channel_id,
                              'mimetype':mimetype,
                              'id':channel_id,
                              'url':url,
                              'genre':genre }

                   channels.append(channel)

            # reset childrens            
            rootContainer.children=[]

            genreItems = {}
            for genre in genres:
                genreItem = self.appendGenre(genre, rootContainer)
                genreItems[genre] = genreItem

            for channel in channels:
                genre = channel.get('genre')
                parentItem = genreItems[genre]
#                self.info("appending to %r channel %r with URL %r", genre, channel.get('name'), channel.get('url'))
                self.appendFeed({'name':channel.get('name'),
                                    'mimetype':channel['mimetype'],
                                    'id':channel.get('channel_id'),
                                    'url':channel.get('url')},
                            parentItem)


        def got_error(error):
            self.warning("Connection to VDR streamdev service failed. Will retry in 5s!")
            self.debug("%r", error.getTraceback())
            # will retry later
            self.retrieveList_attemptCount += 1
            reactor.callLater(5, self.retrieveList, parent=rootContainer)
            
        d = utils.getPage(self.VDR_ws_url)
        d.addCallbacks(got_page, got_error)
