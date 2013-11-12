/**
 *  @file platTarget.C
 *
 *  @brief Implements the platform part of the Target class.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation for platforms that use the
 *  handle pointer to point to a Component that is not created/deleted when a
 *  Target object is created/deleted (i.e. two Target objects that reference
 *  the same component have the same pointer). It could be possible for a
 *  platform specific ID structure to be created and pointed to each time a new
 *  Target is created, in that case, the pointed to object's type needs to be
 *  be known in order to do a deep compare/copy and a delete.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created. Based on Hlava prototype
 *                          mjjones     09/06/2011  Added toString
 *
 */

#include <fapiTarget.H>
#include <ecmdStructs.H>
#include <ecmdSharedUtils.H>


namespace fapi
{

//******************************************************************************
// Compare the handle
//
// If the pointers point to the same component then the handles are the same
//******************************************************************************
bool Target::compareHandle(const Target & i_right) const
{
    return (iv_pHandle == i_right.iv_pHandle);
}

//******************************************************************************
// Copy the handle
//
// Note shallow copy of iv_pHandle. Both Targets point to the same component
//******************************************************************************
void Target::copyHandle(const Target & i_right)
{
    iv_pHandle = i_right.iv_pHandle;
}

//******************************************************************************
// Delete the handle
//******************************************************************************
void Target::deleteHandle()
{
    // Intentionally does nothing. The component must not be deleted
}

void Target::toString(char (&o_ecmdString)[MAX_ECMD_STRING_LEN]) const
{
   ecmdChipTarget * ptr = reinterpret_cast<ecmdChipTarget *> (iv_pHandle);
#ifdef AIX
   strncpy(o_ecmdString, "BLANK" , sizeof(o_ecmdString));
#else
   strncpy(o_ecmdString, ecmdWriteTarget(*ptr).c_str(), sizeof(o_ecmdString));
#endif
   o_ecmdString[MAX_ECMD_STRING_LEN - 1] = 0;
}

//******************************************************************************
// Get the ecmd-format string
//******************************************************************************
const char * Target::toEcmdString() const
{
    if (iv_pEcmdString == NULL)
    {
        iv_pEcmdString = new char[fapi::MAX_ECMD_STRING_LEN];
        char (&l_ecmdString)[fapi::MAX_ECMD_STRING_LEN] =
            *(reinterpret_cast<char(*)[fapi::MAX_ECMD_STRING_LEN]>
                (iv_pEcmdString));
        toString(l_ecmdString);
    }

    return iv_pEcmdString;
}


}
