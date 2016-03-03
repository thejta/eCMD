//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

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

  ecmdCageData tst = dynamic_cast<ecmdCageData>systemCopy[0];

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
