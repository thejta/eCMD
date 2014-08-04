#!/usr/bin/python

import ecmd

tgts = ecmd.ecmdChipTargetList()
tgt = ecmd.ecmdChipTarget()

print("size: %d") % (tgts.size())

tgt.chipType="chip 1"
tgts.push_back(tgt)
tgt.chipType="chip 2"
tgts.push_back(tgt)

print("size: %d") % (tgts.size())

for tgt in tgts:
    print("name: %s") % (tgt.chipType)
print("****")

for i in range(0,tgts.size()):
    print("name: %s") % (tgts[i].chipType)
print("****")

tgt = tgts.pop()
print("name: %s") % (tgt.chipType)
print("****")

tgts.push_back(tgt)
for i in range(0,tgts.size()):
    print("name: %s") % (tgts[i].chipType)
print("****")

del(tgts[0:1])
for i in range(0,tgts.size()):
    print("name: %s") % (tgts[i].chipType)
print("****")

tgt.chipType = "chip 4"
tgts[1:1] = [tgt]
for i in range(0,tgts.size()):
    print("name: %s") % (tgts[i].chipType)



