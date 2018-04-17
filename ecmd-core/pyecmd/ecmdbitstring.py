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
Bit string classes building upon the bitstring module
and adding some eCMD specific methods

Created on Nov 23, 2017

@author: Joachim Fenkes <fenkes@de.ibm.com>
'''

import bitstring

class EcmdBitArray(bitstring.BitArray):
    """
    Replacement for ecmdDataBuffer that builds on bitstring; please take a look at
    https://pythonhosted.org/bitstring/ for full documentation.
    This class adds a few properties/methods you may expect from an ecmdDataBuffer
    replacement but aren't present in vanilla bitstring, like hex_left and hex_right.

    Here are some typical uses of ecmdDataBuffer with their bitstring counterparts,
    some of which are not immediately apparent:

    buf.extract(buf2, start, len)
        --> buf2 = buf[start:start+len]

    buf.extractToRight((uint32_t &)value, start, len)
        --> value = buf[start:start+len].uint

    buf.insert(buf2, target_start, len, source_start)
        --> buf[target_start:target_start+len] = buf2[source_start:source_start+len]

    buf.insertFromRight(42, target_start, len)
        --> buf[target_start:target_start+len] = 42

    buf.flushTo0()
        --> buf.set(False)

    buf.flushTo1()
        --> buf.set(True)

    buf.setBit(12)
        --> buf[12] = True

    buf.genHexLeftStr()
        --> buf.hex_left

    buf.genBinStr()
        --> buf.bin
    """

    @property
    def hex_left(self):
        """
        Return a hex string that is left aligned if the length is not a multiple of 4
        @rtype: str
        """
        return (self + bitstring.Bits(-len(self) & 3)).hex

    @property
    def hex_right(self):
        """
        Return a hex string that is right aligned if the length is not a multiple of 4
        @rtype: str
        """
        return (bitstring.Bits(-len(self) & 3) + self).hex

    @property
    def even_parity(self):
        """
        Return the even parity of this bitstring.
        @rtype: bool
        """
        return bool(self.count(True) & 1)

    @property
    def odd_parity(self):
        """
        Return the odd parity of this bitstring.
        @rtype: bool
        """
        return not bool(self.count(True) & 1)
