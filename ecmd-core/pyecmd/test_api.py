from pyecmd import *
from ecmd import ecmdDataBuffer

extensions = {}
if hasattr(ecmd, "fapi2InitExtension"):
     extensions["fapi2"] = "ver1"

with Ecmd(**extensions):
     t = loopTargets("pu", ECMD_SELECTED_TARGETS_LOOP)[0]
     data = t.getScom(0x1234)
     t.putScom(0x1234, 0x10100000)
     # These interfaces may not be defined for some plugins
     # Pull them to prevent compile issues
     #core_id, thread_id = t.targetToSequenceId()
     #unit_id_string = unitIdToString(2)
     #clock_state = t.queryClockState("SOMECLOCK")
     t.relatedTargets("pu.c")
     retval = t.queryFileLocation(ECMD_FILE_SCANDEF, "")
     for loc in retval.fileLocations:
         testval = loc.textFile + loc.hashFile + retval.version

     if "fapi2" in extensions:
         try:
              t.fapi2GetAttr("ATTR_DOES_NOT_EXIST")
              assert(""=="That was supposed to throw!")
         except KeyError:
              pass

         t.fapi2SetAttr("ATTR_CHIP_ID", 42)
         assert(42 == t.fapi2GetAttr("ATTR_CHIP_ID"))

     # Some buffer tests
     b = ecmdDataBuffer(64)
     b.setDoubleWord(0, 0x1234567812345678)
     assert(convertFromDataBuffer(b).uint == 0x1234567812345678)

     b = EcmdBitArray("0x1234567812345678")
     assert(convertToDataBuffer(b).getDoubleWord(0) == 0x1234567812345678)
     assert(convertToDataBuffer("0x1234567812345678").getDoubleWord(0) == 0x1234567812345678)
     assert(convertToDataBuffer(0x1234567812345678).getDoubleWord(0) == 0x1234567812345678)
