#!/bin/bash

if [[ `uname` == "Linux" ]]
then
   # Use lsb_release to figure out the distro to return
   distrobase=`lsb_release -a 2>&1 | grep Distributor | cut -d : -f 2 | tr -d '[[:space:]]'`
   #echo $distrobase
   distrorelease=`lsb_release -a 2>&1 | grep Release | cut -d : -f 2 | tr -d '[[:space:]]'`
   #echo $distrorelease

   # We've got the two pieces we need, put them together
   if [[ "$distrobase" == *"RedHatEnterprise"* ]]
   then
       if [[ "$distrorelease" == *"6."* ]]
       then
           # Check to see if we have special things installed that indicate
           # this is box used to create builds only for internal IBM use
           if [[ -n "$CTEPATH" ]]
           then
               echo el6ibm
           else
               echo el6
           fi
       elif [[ "$distrorelease" == *"5."* ]]
       then
           echo el5
       else
           echo NONE
       fi
   elif [[ "$distrobase" == *"Fedora"* ]]
   then
       echo "fc"$distrorelease
   elif [[ "$distrobase" == *"Ubuntu"* ]]
   then
       echo "ub"$distrorelease
   elif [[ "$distrobase" == *"Debian"* ]]
   then
       if [[ "$distrorelease" == *"7."* ]]
       then
           echo deb7
       elif [[ "$distrorelease" == *"8."* ]]
       then
           echo deb8
       else
           echo NONE
       fi
   else
       echo NONE
   fi
else
    echo aix
fi
