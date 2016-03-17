//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2013,2016 IBM International Business Machines Corp.
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <ecmdChipTargetCompare.H>

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

bool ecmdChipTargetCompare::operator() (const ecmdChipTarget& lhs, const ecmdChipTarget& rhs) const
{
    bool result = false;
    bool search = true;

    // if state is not valid treat as unused
    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.cageState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.cageState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.cage < rhs.cage)
                {
                    result = true;
                }
                else if (lhs.cage == rhs.cage)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.nodeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.nodeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.node < rhs.node)
                {
                    result = true;
                }
                else if (lhs.node == rhs.node)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.slotState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.slotState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.slot < rhs.slot)
                {
                    result = true;
                }
                else if (lhs.slot == rhs.slot)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.chipTypeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.chipTypeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.chipType < rhs.chipType)
                {
                    result = true;
                }
                else if (lhs.chipType == rhs.chipType)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.posState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.posState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.pos < rhs.pos)
                {
                    result = true;
                }
                else if (lhs.pos == rhs.pos)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.chipUnitTypeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.chipUnitTypeState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.chipUnitType < rhs.chipUnitType)
                {
                    result = true;
                }
                else if (lhs.chipUnitType == rhs.chipUnitType)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.chipUnitNumState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.chipUnitNumState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.chipUnitNum < rhs.chipUnitNum)
                {
                    result = true;
                }
                else if (lhs.chipUnitNum == rhs.chipUnitNum)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

    if (search)
    {
        search = false;
        ecmdChipTargetState_t lState = (lhs.threadState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        ecmdChipTargetState_t rState = (rhs.threadState == ECMD_TARGET_FIELD_VALID ? ECMD_TARGET_FIELD_VALID : ECMD_TARGET_FIELD_UNUSED);
        if (lState < rState)
        {
            result = true;
        }
        else if (lState == rState)
        {
            if (lState == ECMD_TARGET_FIELD_VALID)
            {
                if (lhs.thread < rhs.thread)
                {
                    result = true;
                }
                else if (lhs.thread == rhs.thread)
                {
                    // keep searching
                    search = true;
                }
            }
        }
    }

#if 0
    ecmdChipTarget lhscopy = lhs;
    ecmdChipTarget rhscopy = rhs;
    out.note("ecmdChipTargetCompare::operator()", "%s %s %s\n", ecmdWriteTarget(lhscopy, ECMD_DISPLAY_TARGET_STATES_DECIMAL).c_str(), (result ? "<" : ">="), ecmdWriteTarget(rhscopy, ECMD_DISPLAY_TARGET_STATES_DECIMAL).c_str());
#endif

    return result;
}
