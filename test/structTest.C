/* File structTest.C created by Joshua Wills on Wed Sep 17 2003. */

/* Change Activity: */
/* End Change Activity */

static const char ibmid[] = "Copyright IBM Corporation 2003 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

#include <ecmdStructs.H>
#include <vector>
#include <iostream>
#include <string>

int dummy1 (ecmdChipTarget & target, vector<ecmdCageData> & queryData);

int main (int argc, char *argv[])
{
  ecmdChipData ecd;
  ecd.chipType = "pu";
  ecd.pos = 0;
  ecd.numProcCores = 2;
  ecd.chipEc = 10;
  
  ecmdNodeData end;

  end.nodeId = 1;
  end.chipData.push_back(ecd);

  cout << "Node ID: " << end.nodeId << endl
       << "Chip Type: " << end.chipData[0].chipType << endl;


  vector<ecmdCageData> systemData;
  ecmdChipTarget myTarget;
  myTarget.chipType = "pu";
  myTarget.pos = 0;
  myTarget.posValid = 1;

  int rc = dummy1(myTarget, systemData);

  //i wonder how it handles recursive copy...
  vector<ecmdCageData> systemCopy = systemData;

  cout << "Cage Id: " << systemCopy[0].cageId << endl
       << "Node Id: " << systemCopy[0].nodeData[0].nodeId << endl
       << "ChipType: " << systemCopy[0].nodeData[0].chipData[0].chipType << endl;

  return 0;
}

int dummy1 (ecmdChipTarget & target, vector<ecmdCageData> & queryData) {

  if (target.chipType != "") {

    ecmdCageData cageInfo;

    ecmdNodeData nodeInfo;

    ecmdChipData chipInfo;

    chipInfo.chipType = target.chipType;
    chipInfo.pos = target.pos;
    
    nodeInfo.nodeId = 0;
    nodeInfo.chipData.push_back(chipInfo);

    cageInfo.cageId = 1;
    cageInfo.nodeData.push_back(nodeInfo);

    queryData.push_back(cageInfo);    
  }

  return 0;

}

/* Change Log
<@log@>

Wed Sep 17 2003  17:10:33  by Joshua Wills
<reason><version><Brief description and why change was made.>

Wed Sep 17 2003  15:25:53  by Joshua Wills
<reason><version><Brief description and why change was made.>

Wed Sep 17 2003  15:21:28  by Joshua Wills
<reason><version><Brief description and why change was made.>

Wed Sep 17 2003  15:19:07  by Joshua Wills
<reason><version><Brief description and why change was made.>

Wed Sep 17 2003  14:00:30  by Joshua Wills
<reason><version><Brief description and why change was made.>
*/
