# Load the eCMD python module
import ecmd
import os
import sys

# Create our test number variable
# It will be incremented after each test that executes
testNum = 0

# Do some data buffer tests
testNum+=1; print("edb %02d) Create a buffer" % testNum)
edb = ecmd.ecmdDataBuffer(64)

testNum+=1; print("edb %02d) Set a bit" % testNum)
edb.setBit(4)

testNum+=1; print("edb %02d) Get a bit, and print it" % testNum)
print("The value of bit 4 is %s" % edb.getBit(4))

testNum+=1; print("edb %02d) Clear a bit, get it and print it" % testNum)
edb.clearBit(4)
print("The value of bit 4 is %s" % edb.getBit(4))

testNum+=1; print("edb %02d) Set work 1 to 0x01234567, get it and print it" % testNum)
edb.setWord(1, 0x01234567)
print("The value of word 1 is 0x%08x" % edb.getWord(1))

testNum+=1; print("edb %02d) Get byte 5, it should be 0x23" % testNum)
print("The value of byte 5 is 0x%02x" % edb.getByte(5))

testNum+=1; print("edb %02d) Or two databuffers together, result should be 0x89ABCDEF" % testNum)
edbO1 = ecmd.ecmdDataBuffer(32)
edbO2 = ecmd.ecmdDataBuffer(32)
edbO1.setHalfWord(0, 0x89AB)
edbO2.setHalfWord(1, 0xCDEF)
edbO3 = ecmd.ecmdDataBuffer(32)
edbO3 = edbO1 | edbO2
print("The value of word 0 is 0x%08x" % edbO3.getWord(0))
edbO1.setOr(edbO2, 0, 32)
print("The value of word 0 is 0x%08x" % edbO1.getWord(0))
edbO2.setOr(0x89AB0000, 0, 32)
print("The value of word 0 is 0x%08x" % edbO2.getWord(0))

testNum+=1; print("edb %02d) And two databuffers together, result should be 0x00018000" % testNum)
edbA1 = ecmd.ecmdDataBuffer(32)
edbA2 = ecmd.ecmdDataBuffer(32)
edbA1.setWord(0, 0xFFFF8000)
edbA2.setWord(0, 0x0001FFFF)
edbA3 = ecmd.ecmdDataBuffer(32)
edbA3 = edbA1 & edbA2
print("The value of word 0 is 0x%08x" % edbA3.getWord(0))
edbA1.setAnd(edbA2, 0, 32)
print("The value of word 0 is 0x%08x" % edbA1.getWord(0))
edbA2.setAnd(0xFFFF8000, 0, 32)
print("The value of word 0 is 0x%08x" % edbA2.getWord(0))

testNum+=1; print("edb %02d) Call genHexLeftStr, result should be 0xFFFF80000001FFFF" % testNum)
edb.setBitLength(64)
edb.setWord(0, 0xFFFF8000)
edb.setWord(1, 0x0001FFFF)
print("The value is 0x%s" % edb.genHexLeftStr(0, 64))
print("The value is 0x%s" % edb.genHexLeftStr())

testNum+=1; print("edb %02d) Call genBinStr, result should be the binary version of the test above" % testNum)
edb.setBitLength(64)
edb.setWord(0, 0xFFFF8000)
edb.setWord(1, 0x0001FFFF)
print("The value is %s" % edb.genBinStr(0, 64))
print("The value is %s" % edb.genBinStr())

# Plugin tests using the stub dll
print("\n\n")
testNum = 0

testNum+=1; print("dll %02d) Load the plugin" % testNum)
rc = ecmd.ecmdLoadDll(os.environ['ECMD_DLL_FILE'], "ver13,ver14")
if (rc):
    print("ERROR: problem on dll load")

for i in sys.argv:
    print(i)

#print("type argv: %s" % type(sys.argv))
#print("type argv[0]: %s" % type(sys.argv[0]))
#print("type argv[1]: %s" % type(sys.argv[1]))

rc = ecmd.ecmdCommandArgs(sys.argv)
if (rc):
    print("ERROR: problem calling ecmdCommandArgs")

for i in sys.argv:
    print(i)

optFound = False
#optFound = ecmd.ecmdParseOption(sys.argv, "-y")
print("optFound: %s" % optFound)

for i in sys.argv:
    print(i)

optValue = "Not Real Dummy"
#optValue = ecmd.ecmdParseOptionWithArgs(sys.argv, "-g")
print("optValue: %s" % optValue)

for i in sys.argv:
    print(i)

testNum+=1; print("dll %02d) Query Plugin Info" % testNum)
dllInfo = ecmd.ecmdDllInfo()
rc = ecmd.ecmdQueryDllInfo(dllInfo)
if (rc):
    print("ERROR: problem querying plugin")
print("some dll info:\n");
print("dllType        = %s" % dllInfo.dllType)
print("dllProduct     = %s" % dllInfo.dllProduct)
print("dllProductType = %s" % dllInfo.dllProductType)
print("dllEnv         = %s" % dllInfo.dllEnv)
print("dllBuildDate   = %s" % dllInfo.dllBuildDate)
print("dllCapiVersion = %s" % dllInfo.dllCapiVersion)
print("dllBuildInfo   = %s" % dllInfo.dllBuildInfo)

# Pulling this test for now
# It breaks builds where ring support isn't included, so some smarts would need to be put into it
# JTA 11/11/2014
if (0):
    testNum+=1; print("dll %02d) Query Ring Info" % testNum)
    tgt = ecmd.ecmdChipTarget()
    ringInfo = ecmd.ecmdRingDataList()
    rc = ecmd.ecmdQueryRing(tgt, ringInfo)
    print("size: %d" % ringInfo.size())
    for i in range(0, ringInfo.size()):
        for j in range(0, ringInfo[i].ringNames.size()):
            print("%s" % ringInfo[i].ringNames[j])
            
    for item in ringInfo:
        for itemName in item.ringNames:
            print("%s" % itemName)
        
    for item in ringInfo:
        print("ringNames: "),
        for itemName in item.ringNames:
            print("%s " % itemName),
        print("") # Close the line above
        print("address: 0x%08X" % item.address)
        print("bitLength: %d" % item.bitLength)
        print("isCheckable: %s" % item.isCheckable)
        print("clockState: %s" % item.clockState)
        item.clockState = ecmd.ECMD_CLOCKSTATE_OFF
        print("clockState: %s" % item.clockState)

rc = ecmd.ecmdUnloadDll()
if (rc):
    print("ERROR: problem unloading dll")

