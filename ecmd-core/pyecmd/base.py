#IBM_PROLOG_BEGIN_TAG
#
# Copyright 2015,2018 IBM International Business Machines Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#IBM_PROLOG_END_TAG
'''
functions/classes for pyecmd.
This is the hand-written part that will be augmented with generated code.

Created on Jul 16, 2015

@author: Joachim Fenkes <fenkes@de.ibm.com>
'''

import sys
import ecmd
import bitstring
from .ecmdbitstring import EcmdBitArray

class EcmdError(Exception):
    pass

class EcmdRCError(EcmdError):
    def __init__(self, rc):
        super(EcmdRCError, self).__init__(ecmd.ecmdGetErrorMsg(rc))
        self.rc_value = rc
        self.rc_name  = ecmd.ecmdParseReturnCode(rc)

class PCBResourceBusyError(EcmdRCError): pass
class PCBParityError(EcmdRCError): pass
class PCBOfflineError(EcmdRCError): pass
class PCBPartialError(EcmdRCError): pass
class PCBAddressError(EcmdRCError): pass
class PCBClockError(EcmdRCError): pass
class PCBTimeoutError(EcmdRCError): pass
class PIBAbortError(EcmdRCError): pass
class ClocksInInvalidStateError(EcmdRCError): pass

_error_map = {
    0x0100102B: ClocksInInvalidStateError,
    0x02014003: PCBResourceBusyError,
    0x02014005: PCBOfflineError,
    0x02014007: PCBPartialError,
    0x02014009: PCBAddressError,
    0x0201400B: PCBClockError,
    0x0201400D: PCBParityError,
    0x0201400F: PCBTimeoutError,
    0x02014011: PIBAbortError,
}

if sys.version_info[0] >= 3:
    _int_types = int
    _str_types = (str, bytes, bytearray)
else:
    _int_types = (int, long)
    _str_types = (str, unicode, bytes, bytearray)

def _rcwrap(rc):
    """
    Wrapper for eCmd functions yielding a return code.
    Transforms return code into exception if bad.
    """
    if rc != ecmd.ECMD_SUCCESS:
        raise _error_map.get(rc, EcmdRCError)(rc)

def _bufwrap(rc):
    """
    Same as _rcwrap, but for ecmdDataBuffer functions
    """
    if rc != ecmd.ECMD_DBUF_SUCCESS:
        raise EcmdRCError(rc)

def _to_ecmdDataBuffer(buf, defwidth=64):
    if isinstance(buf, _int_types):
        # negative numbers in Python start with infinite one bits, so strip off infinity minus 64 ;)
        buf = buf & ((1 << defwidth) - 1)

        ecmd_buf = ecmd.ecmdDataBuffer(defwidth)
        if defwidth == 64:
            ecmd_buf.setDoubleWord(0, buf)
        elif defwidth == 32:
            ecmd_buf.setWord(0, buf)
        else:
            raise NotImplementedError()

        return ecmd_buf

    if isinstance(buf, bitstring.Bits):
        ecmd_buf = ecmd.ecmdDataBuffer(len(buf))
        if len(buf):
            buf_bytes = bytearray(buf.tobytes())
            ecmd_buf.memCopyIn(buf_bytes, len(buf_bytes))

        return ecmd_buf

    if isinstance(buf, _str_types):
        bits = bitstring.Bits(buf)
        return _to_ecmdDataBuffer(bits)

    return buf

def _from_ecmdDataBuffer(buf):
    """
    @type buf: ecmd.ecmdDataBuffer
    """
    buf_len = buf.getByteLength()
    if buf_len == 0:
        return EcmdBitArray(0)

    buf_bytes = bytearray(buf_len)
    buf.memCopyOut(buf_bytes, buf_len)
    return EcmdBitArray(length=buf.getBitLength(), bytes=buf_bytes)
