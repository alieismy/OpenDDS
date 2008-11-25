// -*- C++ -*-
//
// $Id$
#ifndef DIRECTPRIORITY_MAPPER_H
#define DIRECTPRIORITY_MAPPER_H

#include "dds/DCPS/dcps_export.h"

#include "PriorityMapper.h"
#include "ace/OS_NS_Thread.h"

namespace OpenDDS { namespace DCPS {

    /**
     * @class DirectPriorityMapper
     *
     * @brief map TRANSPORT_PRIORITY values directly.
     *
     * This implementation maps TRANSPORT_PRIORITY values directly to
     * both DiffServ codepoint and thread priority values.  The behavior
     * is saturating - that is when the TRANSPORT_PRIORITY value is out
     * of the target range, it is mapped to the nearest extremum.
     *
     * DiffServ codepoint values are mapped within the closed interval
     * [0,63], with the corresponding TRANSPORT_PRIORITY values mapped
     * directly within this range.
     *
     * Thread priorities are mapped to the system thread scheduler
     * minimum value (obtained from the sched_get_priority_min(sched)
     * system call, where available) up to the maximum value (obtained
     * from the sched_get_priority_max(sched) system call, where
     * available).  The TRANSPORT_PRIORITY value of 0 is mapped to the
     * minimum and a value of |max-min| is mapped to the maximum.
     */
    class OpenDDS_Dcps_Export DirectPriorityMapper : public PriorityMapper {
      public:
        /// Construct with a priority value.
        DirectPriorityMapper( CORBA::Long priority = 0);

        /// Virtual destructor.
        virtual ~DirectPriorityMapper();

        /// Access the mapped DiffServ codepoint value.
        virtual short codepoint() const;

        /// Access the mapped thread priority value.
        virtual short thread_priority() const;

      private:
        // Minimum value of thread priority values.
        static const short thread_min_;

        // Maximum value of thread priority values.
        static const short thread_max_;

        // Minimum value of DiffServ codepoint values.
        static const short dscp_min_ = 0;

        // Maximum value of DiffServ codepoint values.
        static const short dscp_max_ = 63;
    };

} } // End of namespace OpenDDS::DCPS

#if defined (__ACE_INLINE__)
#include "DirectPriorityMapper.inl"
#endif /* __ACE_INLINE__ */

#endif  /* DIRECTPRIORITY_MAPPER_H */

