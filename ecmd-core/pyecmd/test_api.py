from pyecmd import *

with Ecmd(fapi2="ver1"):
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
     try:
          t.fapi2GetAttr("ATTR_DOES_NOT_EXIST")
          assert(""=="That was supposed to throw!")
     except KeyError:
          pass

     t.fapi2SetAttr("ATTR_CHIP_ID", 42)
     assert(42 == t.fapi2GetAttr("ATTR_CHIP_ID"))
