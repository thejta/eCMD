from pyecmd import *

with Ecmd():
     t = loopTargets("pu", ECMD_SELECTED_TARGETS_LOOP)[0]
     data = t.getScom(0x1234)
     t.putScom(0x1234, 0x10100000)
     # These interfaces may not be defined for some plugins
     # Pull them to prevent compile issues
     #core_id, thread_id = t.targetToSequenceId()
     #unit_id_string = unitIdToString(2)
     #clock_state = t.queryClockState("SOMECLOCK")
     t.relatedTargets("pu.c")
     locs, version = t.queryFileLocationHidden2(ECMD_FILE_SCANDEF, "")
     for loc in locs:
         testval = loc.textFile + loc.hashFile + version
