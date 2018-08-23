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
else:
    _int_types = (int, long)

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
        overhang = len(buf) % 64
        bits = buf + bitstring.Bits(64 - (len(buf) % 64)) if overhang else buf # pad to multiples of 64 bits
        for i in range(len(bits) / 64):
            ecmd_buf.setDoubleWord(i, bits[i*64:(i+1)*64].uint)
        return ecmd_buf
    if isinstance(buf, (str, unicode)):
        bits = bitstring.Bits(buf)
        return _to_ecmdDataBuffer(bits)
    return buf

def _from_ecmdDataBuffer(buf):
    """
    @type buf: ecmd.ecmdDataBuffer
    """
    bits = EcmdBitArray((buf.getBitLength() + 63) & ~63)
    for i in range(buf.getDoubleWordLength()):
        bits[i*64:(i+1)*64] = buf.getDoubleWord(i)
    del bits[buf.getBitLength():]
    return bits
