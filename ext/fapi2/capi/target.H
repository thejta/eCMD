/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source$                                                             */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2014                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file target.H
 * @brief definitions for fapi2 targets
 */

#ifndef __FAPI2_TARGET__
#define __FAPI2_TARGET__

#include <stdint.h>
#include <vector>
#include <target_types.H>
#include <target_states.H>

#ifndef DOXYGEN
#include <iostream>
#endif

#include <ecmdStructs.H>
extern "C" {
uint32_t fapi2GetAssociatedTargets(
	const ecmdChipTarget & i_target, 
	const fapi2::TargetType i_associatedTargetType, 
	std::list<const ecmdChipTarget *> & o_targets, 
	const fapi2::TargetState i_state); 
}

namespace fapi2
{
    ///
    /// @brief Class representing a FAPI2 Target
    /// @tparam K the type (Kind) of target
    /// @tparam V the type of the target's Value
    /// @remark TargetLite targets are uint64_t, Targets
    /// are uintptr_t (void*).
    ///
    /// Assuming there are representations of a processor,
    /// a membuf and a system here are some examples:
    /// @code
    /// #define PROCESSOR_CHIP_A 0xFFFF0000
    /// #define MEMBUF_CHIP_B    0x0000FFFF
    /// #define SYSTEM_C         0x0000AAAA
    /// @endcode
    ///
    /// * To define a target:
    /// @code
    /// fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> A(PROCESSOR_CHIP_A);
    /// fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> C(SYSTEM_C);
    /// fapi2::Target<fapi2::TARGET_TYPE_MEMBUF_CHIP> B(MEMBUF_CHIP_B);
    /// @endcode
    ///
    /// * Functions which take composite target types
    /// @code
    /// void takesProcOrMembuf(
    /// const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP |
    ///                     fapi2::TARGET_TYPE_MEMBUF_CHIP>& V );
    ///
    /// void takesAny(const fapi2::Target<fapi2::TARGET_TYPE_ALL>& V );
    ///
    /// @endcode
    ///
    /// * Traversing the target "tree"
    /// @code
    /// fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> A(PROCESSOR_CHIP_A);
    ///
    /// // Get A's parent
    /// A.getParent<fapi2::TARGET_TYPE_SYSTEM>();
    ///
    /// // Get the 0x53'd core
    /// fapi2::getTarget<fapi2::TARGET_TYPE_CORE>(0x53);
    ///
    /// // Get all *my* present/functional children which are cores
    /// A.getChildren<fapi2::TARGET_TYPE_CORE>();
    ///
    /// // Get all of the the cores relative to my base target
    /// fapi2::getChildren<fapi2::TARGET_TYPE_CORE>();
    /// @endcode
    ///
    /// * Invalid casts
    /// @code
    /// // Can't cast to a specialized target
    /// fapi2::Target<fapi2::TARGET_TYPE_NONE> D(MEMBUF_CHIP_B);
    /// takesProcOrMembuf( D );
    ///
    /// // Not one of the shared types
    /// fapi2::Target<fapi2::TARGET_TYPE_ABUS_ENDPOINT> E;
    /// takesProcOrMembuf( E );
    /// @endcode
    template<TargetType K, typename V = const ecmdChipTarget *>
    class Target
    {
    public:

        ///
        /// @brief Create a Target, with a value
        /// @param[in] V the value (i.e., specific element this
        /// target represents, or pointer)
        /// @note Platforms can mangle the value and K to get a
        /// single uint64_t in value which represents all the information
        /// they might need. value( K | V ), for example
        ///
        Target(V Value = NULL):
            iv_handle(Value)
        {}

        ///
        /// @brief Assignment Operator.
        /// @param[in] i_right Reference to Target to assign from.
        /// @return Reference to 'this' Target
        ///
        Target& operator=(const Target& i_right)
            { iv_handle = i_right.iv_handle; return *this; }

        ///
        /// @brief Equality Comparison Operator
        /// @param[in] i_right Reference to Target to compare.
        /// @return bool. True if equal.
        /// @note Platforms need to define this so that the physical
        /// targets are determined to be equivilent rather than just the handles
        ///
        bool operator==(const Target& i_right) const
        {
            return ((*iv_handle) == (*(i_right.iv_handle)));
        }

        ///
        /// @brief Inquality Comparison Operator
        /// @param[in] i_right Reference to Target to compare.
        /// @return bool. True if not equal.
        /// @note Platforms need to define this so that the physical
        /// targets are determined to be equivilent rather than just the handles
        ///
        bool operator!=(const Target& i_right) const
        {
            return !((*iv_handle) == (*(i_right.iv_handle)));
        }

        ///
        /// @brief Get the handle.
        /// @return V The target's handle, or value
        ///
        V get(void) const
            { return iv_handle; }

        ///
        /// @brief Get the handle as a V
        /// @return V The target's handle, or value
        ///
        inline operator V() const { return iv_handle; }

        ///
        /// @brief Get a target's value
        /// @return V The target's handle, or value
        ///
        inline V& operator()(void) { return iv_handle; }

        ///
        /// @brief Get the target type
        /// @return The type of target represented by this target
        ///
        inline TargetType getType(void) const { return iv_type; }

        ///
        /// @brief Get this target's immediate parent
        /// @tparam T The type of the parent
        /// @return Target<T> a target representing the parent
        ///
        template< TargetType T >
        inline Target<T> getParent(void) const
            {
                // FIXME add static asserts
	            std::list<const ecmdChipTarget *> l_targets;

                uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, TARGET_STATE_FUNCTIONAL);
                if (l_rc)
                {
                    // ERROR
                    return Target<T>();
                }
                if (l_targets.empty())
                {
                    // ERROR
                    return Target<T>();
                }
                return Target<T>(*(l_targets.front()));
            }

        ///
        /// @brief Get this target's children
        /// @tparam T The type of the parent
        /// @param[in] i_state The desired TargetState of the children
        /// @return std::vector<Target<T> > a vector of present/functional
        /// children
        ///
        template< TargetType T>
        inline std::vector<Target<T> >
        getChildren(const TargetState i_state = TARGET_STATE_FUNCTIONAL) const
            {
                static_assert((K == fapi2::TARGET_TYPE_SYSTEM) ||
                              (K == fapi2::TARGET_TYPE_PROC_CHIP) ||
                              (K == fapi2::TARGET_TYPE_MEMBUF_CHIP),
                              "Invalid parent for getChildren");

#define INVALID_CHILD(PARENT, CHILD) \
    static_assert(!((K == PARENT) && (T == CHILD)), \
                  #CHILD " is not a child of " #PARENT );

                // invalid children for proc
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_NONE)
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_SYSTEM)
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_DIMM)
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_PROC_CHIP)
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_MEMBUF_CHIP)
                INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_MBA)

                // invalid children for membuf
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_NONE)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_SYSTEM)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_DIMM)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_PROC_CHIP)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MEMBUF_CHIP)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_EX)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MCS)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_XBUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_ABUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_L4)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_CORE)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_EQ)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MCA)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MCBIST)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MCBIST)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MIA)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_MIS)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_DMI)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_OBUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_NV)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_SBE)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_PPE)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_PERV)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_PEC)
                INVALID_CHILD(fapi2::TARGET_TYPE_MEMBUF_CHIP, fapi2::TARGET_TYPE_PHB)

                // invalid children for system
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_NONE)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_SYSTEM)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_EX)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MBA)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MCS)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_XBUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_ABUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_L4)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_CORE)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_EQ)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MCA)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MCBIST)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MIA)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_MIS)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_DMI)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_OBUS)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_NV)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_SBE)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_PPE)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_PERV)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_PEC)
                INVALID_CHILD(fapi2::TARGET_TYPE_SYSTEM, fapi2::TARGET_TYPE_PHB)
#if 0
        TARGET_TYPE_NONE
        TARGET_TYPE_SYSTEM
        TARGET_TYPE_DIMM
        TARGET_TYPE_PROC_CHIP
        TARGET_TYPE_MEMBUF_CHIP
        TARGET_TYPE_EX
        TARGET_TYPE_MBA
        TARGET_TYPE_MCS
        TARGET_TYPE_XBUS
        TARGET_TYPE_ABUS
        TARGET_TYPE_L4
        TARGET_TYPE_CORE
        TARGET_TYPE_EQ
        TARGET_TYPE_MCA
        TARGET_TYPE_MCBIST
        TARGET_TYPE_MIA
        TARGET_TYPE_MIS
        TARGET_TYPE_DMI
        TARGET_TYPE_OBUS
        TARGET_TYPE_NV
        TARGET_TYPE_SBE
        TARGET_TYPE_PPE
        TARGET_TYPE_PERV
        TARGET_TYPE_PEC
        TARGET_TYPE_PHB
#endif
#undef INVALID_CHILD
	            std::list<const ecmdChipTarget *> l_targets;

                uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, i_state);
                if (l_rc)
                {
                    // ERROR
                    return {Target<T>()};
                }
                if (l_targets.empty())
                {
                    // ERROR
                    return {Target<T>()};
                }
                std::vector<Target<T> > l_children;
                auto l_target = l_targets.begin();
                while (l_target != l_targets.end())
                {
                    l_children.push_back(Target<T>(*l_target));
                    l_target++;
                }
                return l_children;
            }

        ///
        /// @brief Get the target at the other end of a bus - dimm included
        /// @tparam T The type of the parent
        /// @param[in] i_state The desired TargetState of the children
        /// @return Target<T> a target representing the thing on the other end
        /// @note Can be easily changed to a vector if needed
        ///
        template<TargetType T>
        inline Target<T>
        getOtherEnd(const TargetState i_state = TARGET_STATE_FUNCTIONAL) const
            {
                // FIXME add static asserts
	            std::list<const ecmdChipTarget *> l_targets;

                uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, i_state);
                if (l_rc)
                {
                    // ERROR
                    return Target<T>();
                }
                if (l_targets.empty())
                {
                    // ERROR
                    return Target<T>();
                }
                return Target<T>(*(l_targets.front()));
            }

        ///
        /// @brief Copy from a Target<O> to a Target<K>
        /// @tparam O the target type of the other
        ///
        template<TargetType O>
        inline Target( const Target<O>& Other ):
            Target<K, V>(Other.get())
        {
            static_assert( (K & O) != 0,
                           "unable to cast Target, no shared types");

            static_assert( bitCount<K>::count >= bitCount<O>::count,
                           "unable to cast to specialized Target");
        }

#ifndef DOXYGEN
        void print( const std::string& Id ) const
            {
                std::cout << "\t" << Id
                          << " is " << std::hex << get()
                          << "\ttype " << getType()
                          << std::dec << std::endl;
            }
#endif

    private:
        // Don't use enums here as it makes it hard to assign
        // in the platform target cast constructor.
        static const TargetType iv_type = K;
        V iv_handle;

    };

    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @tparam B The type of the buffer
    /// @param[in] i_target Target<T>
    /// @param[in] i_buffer buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template< TargetType T >
    inline void toString(const Target<T>& i_target, char* i_buffer, size_t i_bsize)
    {
        snprintf(i_buffer, i_bsize, "Target 0x%p/0x%x", i_target.get(), T);
    }

    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @tparam B The type of the buffer
    /// @param[in] A pointer to the Target<T>
    /// @param[in] The buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template< TargetType T >
    inline void toString(const Target<T>* i_target, char* i_buffer, size_t i_bsize)
    {
        snprintf(i_buffer, i_bsize, "Target 0x%p/0x%x", i_target->get(), T);
    }

    ///
    /// @brief Get an enumerated target of a specific type
    /// @tparam T The type of the target
    /// @param[in] uint64_t representing the ordinal number of
    /// the desired target
    /// @return Target<T> the target requested
    ///
    template<TargetType T>
    Target<T> getTarget(uint64_t Ordinal)
    {
        // For testing
        return Target<T>(NULL);
    }

    // Why has the been removed? For starters, the API name
    // is probably wrong as it's already been confused with
    // Target::getChildren(). And if I'm going to change it
    // I really want to see if we need it. I'm still not
    // clear on whether we're alloing this traversal or not.
#if 0
    ///
    /// @brief Get the base target's children
    /// @tparam T The type of the target
    /// @return std::vector<Target<T> > a vector of present/functional
    /// children
    ///
    template<TargetType T>
    inline std::vector<Target<T> > getChildren()
    {
        // For testing
        return {Target<T>(), Target<T>()};
    }
#endif

    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @tparam B The type of the buffer
    /// @param[in] i_target Target<T>
    /// @param[in] i_buffer buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template<TargetType T, typename B>
    inline void toString(const Target<T>& i_target, B& i_buffer)
    {
    }

    ///
    /// @brief Check if the target is of a type, or in a type subset.
    /// @tparam K the TargetType to check
    /// @tparam T TargetType or TargetType composite to check against
    /// @return True, iff K is a proper T
    ///
    template< TargetType K, TargetType T >
    inline constexpr bool is_same(void)
    { return (K & T) != 0; }


};

#endif
