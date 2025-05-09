#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# RDMAPI.py
# Copyright (C) 2010 Simon Newton

"""The Python RDM API."""

__author__ = 'nomis52@gmail.com (Simon Newton)'

import sys
from ola.UID import UID
from ola.OlaClient import OlaClient
from ola import PidStore


class RDMAPI(object):
  """The RDM API."""

  # This maps ola.proto enums to PidStore enums
  COMMAND_CLASS_DICT = {
      OlaClient.RDM_GET_RESPONSE: PidStore.RDM_GET,
      OlaClient.RDM_SET_RESPONSE: PidStore.RDM_SET,
  }

  def __init__(self, client, pid_store, strict_checks = True):
    """Create a new RDM API.

    Args:
      client: A OlaClient object.
      pid_store: A PidStore instance
      strict_checks: Enable strict checking
    """
    self._client = client
    self._pid_store = pid_store
    self._strict_checks = strict_checks

  def Get(self, universe, uid, sub_device, pid, callback, args = []):
    """Send a RDM Get message, packing the arguments into a message.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      args: The args to pack into the param data section.

    Return:
      True if sent ok, False otherwise.
    """
    if self._strict_checks and uid.IsBroadcast():
      print >> sys.stderr, "Can't send GET to broadcast address %s" % uid
      return False

    return self._SendRequest(universe, uid, sub_device, pid, callback, args,
                             PidStore.RDM_GET)

  def RawGet(self, universe, uid, sub_device, pid, callback, data):
    """Send a RDM Get message with the raw data supplied.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      data: The param data

    Return:
      True if sent ok, False otherwise.
    """
    if self._strict_checks and uid.IsBroadcast():
      print >> sys.stderr, "Can't send GET to broadcast address %s" % uid
      return False

    return self._SendRawRequest(universe, uid, sub_device, pid, callback, data,
                                PidStore.RDM_GET)

  def Set(self, universe, uid, sub_device, pid, callback, args = []):
    """Send a RDM Set message.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      args: The args to pack into the param data section.

    Return:
      True if sent ok, False otherwise.
    """
    return self._SendRequest(universe, uid, sub_device, pid, callback, args,
                             PidStore.RDM_SET)

  def RawSet(self, universe, uid, sub_device, pid, callback, args = []):
    """Send a RDM Set message with the raw data supplied.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      data: The param data

    Return:
      True if sent ok, False otherwise.
    """
    return self._SendRawRequest(universe, uid, sub_device, pid, callback,
                                    args, PidStore.RDM_SET)

  def _SendRequest(self, universe, uid, sub_device, pid, callback, args,
                   request_type):
    """Send a RDM Request.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      args: The args to pack into the param data section.
      request_type: PidStore.RDM_GET or PidStore.RDM_SET

    Return:
      True if sent ok, False otherwise.
    """
    data = pid.Pack(args, request_type)
    if data is None:
      print >> sys.stderr, 'Could not pack data'
      return False

    return self._SendRawRequest(universe, uid, sub_device, pid, callback, data,
                                request_type)

  def _SendRawRequest(self, universe, uid, sub_device, pid, callback, data,
                      request_type):
    """Send a RDM Request.

    Args:
      universe: The universe to send the request on.
      uid: The UID to address the request to.
      sub_device: The Sub Device to send the request to.
      pid: A PID object that describes the format of the request.
      callback: The callback to run when the request completes.
      data: The param data.
      request_type: PidStore.RDM_GET or PidStore.RDM_SET

    Return:
      True if sent ok, False otherwise.
    """
    if self._strict_checks:
      request_params = {
        'uid': uid,
        'sub_device': sub_device,
      }
      if not pid.ValidateAddressing(request_params, request_type):
        return False

    if request_type == PidStore.RDM_SET:
      method = self._client.RDMSet
    else:
      method = self._client.RDMGet

    return method(
        universe,
        uid,
        sub_device,
        pid.value,
        lambda response: self._GenericHandler(callback, uid, response),
        data)

  def _GenericHandler(self, callback, uid, response):
    """

    Args:
      callback: the function to run
      uid: The uid the request was for
      response: A RDMResponse object
    """
    obj = None
    unpack_exception = None

    if response.WasAcked():
      request_type = self.COMMAND_CLASS_DICT[response.command_class]
      pid_descriptor = self._pid_store.GetPid(response.pid,
                                              uid.manufacturer_id)
      if pid_descriptor:
        try:
          obj = pid_descriptor.Unpack(response.data, request_type)
        except PidStore.UnpackException, e:
          obj = None
          unpack_exception = e
      else:
        obj = data

    callback(response, obj, unpack_exception)
