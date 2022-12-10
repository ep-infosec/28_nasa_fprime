// ======================================================================
// \title  Accumulate.hpp
// \author bocchino, mereweth
// \brief  Test drain mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Accumulate.hpp"

namespace Svc {

  namespace Accumulate {

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void Tester ::
      OK()
    {

      ASSERT_EQ(BufferAccumulator::DRAIN, this->component.mode);
      this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::ACCUMULATE);
      this->component.doDispatch();
      ASSERT_EQ(BufferAccumulator::ACCUMULATE, this->component.mode);
      ASSERT_FROM_PORT_HISTORY_SIZE(0);

      Fw::Buffer buffers[MAX_NUM_BUFFERS];
      const U32 managerID = 42;
      const U64 data = 0;
      const U32 size = 10;
      for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
        const U32 bufferID = i;
        Fw::Buffer b(managerID, bufferID, data, size);
        buffers[i] = b;
        this->invoke_to_bufferSendInFill(0, buffers[i]);
        this->component.doDispatch();
        ASSERT_FROM_PORT_HISTORY_SIZE(0);
      }

      this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::DRAIN);
      this->component.doDispatch();
      ASSERT_EQ(BufferAccumulator::DRAIN, this->component.mode);
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      ASSERT_from_bufferSendOutDrain_SIZE(1);
      ASSERT_from_bufferSendOutDrain(0, buffers[0]);

      U32 expectedNumBuffers = 1;
      for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
        this->invoke_to_bufferSendInReturn(0, buffers[i]);
        this->component.doDispatch();
        ++expectedNumBuffers;
        if (i + 1 < MAX_NUM_BUFFERS) {
          ++expectedNumBuffers;
          ASSERT_from_bufferSendOutDrain_SIZE(i + 2);
          ASSERT_from_bufferSendOutDrain(i + 1, buffers[i + 1]);
        }
        ASSERT_FROM_PORT_HISTORY_SIZE(expectedNumBuffers);
        ASSERT_from_bufferSendOutReturn_SIZE(i + 1);
        ASSERT_from_bufferSendOutReturn(i, buffers[i]);
      }

      ASSERT_EQ(2U * MAX_NUM_BUFFERS, expectedNumBuffers);
      ASSERT_FROM_PORT_HISTORY_SIZE(expectedNumBuffers);
      ASSERT_from_bufferSendOutDrain_SIZE(MAX_NUM_BUFFERS);
      ASSERT_from_bufferSendOutReturn_SIZE(MAX_NUM_BUFFERS);

      for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
        ASSERT_from_bufferSendOutDrain(i, buffers[i]);
        ASSERT_from_bufferSendOutReturn(i, buffers[i]);
      }

    }

  }

}
