/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : OW1.c
**     CDE edition : Community
**     Project     : FRDM-KL27Z_McuOnEclipseLib
**     Processor   : MKL25Z128VLK4
**     Component   : OneWire
**     Version     : Component 01.103, Driver 01.00, CPU db: 3.00.000
**     Repository  : Legacy User Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-05-11, 16:35, # CodeGen: 146
**     Abstract    :
**
This is a component implementing the 1-Wire protocol.
**     Settings    :
**          Component Name                                 : OW1
**          Data Pin I/O                                   : SDK_BitIO
**          Timer                                          : Disabled
**          RTOS                                           : Enabled
**            RTOS                                         : FRTOS1
**          Connection time settings                       : 
**            Connection Mode                              : Master - One slave
**            A: Write 1 Low time (us)                     : 6
**            B: Write 1 High time (us)                    : 64
**            C: Write 0 Low time (us)                     : 60
**            D: Write 0 High time (us)                    : 10
**            E: delay time before read (us)               : 3
**            F: After read delay time                     : 55
**            H: Reset time (us)                           : 480
**            I: Device response time (us)                 : 70
**            Total slot time (us)                         : 100
**          Buffers                                        : 
**            Output                                       : RBOutput
**            Input                                        : RBInput
**            Time                                         : RBTime
**            Program                                      : RBProgram
**          Debug                                          : Enabled
**            Debug Read Pin                               : SDK_BitIO
**          CriticalSection                                : CS1
**          SDK                                            : MCUC1
**     Contents    :
**         add_CRC      - void OW1_add_CRC(uint8_t bitValue);
**         i_run        - void OW1_i_run(void);
**         i_action     - void OW1_i_action(void);
**         i_reset      - void OW1_i_reset(void);
**         i_presence   - void OW1_i_presence(void);
**         i_send_low   - void OW1_i_send_low(void);
**         i_send_float - void OW1_i_send_float(void);
**         i_recv_float - void OW1_i_recv_float(void);
**         i_recv_get   - void OW1_i_recv_get(void);
**         i_recv_low   - void OW1_i_recv_low(void);
**         i_wait       - void OW1_i_wait(void);
**         CalcCRC      - uint8_t OW1_CalcCRC(uint8_t *data, uint8_t dataSize);
**         SendByte     - bool OW1_SendByte(uint8_t data);
**         Receive      - bool OW1_Receive(uint8_t counter);
**         SendReset    - void OW1_SendReset(void);
**         Count        - uint8_t OW1_Count(void);
**         Waitms       - bool OW1_Waitms(uint8_t key, uint8_t time_ms);
**         ProgramEvent - bool OW1_ProgramEvent(uint8_t key);
**         SendBytes    - bool OW1_SendBytes(uint8_t *data, uint8_t count);
**         GetBytes     - bool OW1_GetBytes(uint8_t *data, uint8_t count);
**         GetByte      - bool OW1_GetByte(uint8_t *data);
**         GetError     - void OW1_GetError(void);
**         isBusy       - bool OW1_isBusy(void);
**         Deinit       - void OW1%.Init(void) OW1_Deinit(void);
**         Init         - void OW1%.Init(void) OW1_Init(void);
**
**     * Copyright (c) Original implementation: Omar Isa� Pinales Ayala, 2014, all rights reserved.
**      * Updated and maintained by Erich Styger, 2014-2017
**      * Web:         https://mcuoneclipse.com
**      * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**      * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**      * All rights reserved.
**      *
**      * Redistribution and use in source and binary forms, with or without modification,
**      * are permitted provided that the following conditions are met:
**      *
**      * - Redistributions of source code must retain the above copyright notice, this list
**      *   of conditions and the following disclaimer.
**      *
**      * - Redistributions in binary form must reproduce the above copyright notice, this
**      *   list of conditions and the following disclaimer in the documentation and/or
**      *   other materials provided with the distribution.
**      *
**      * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**      * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**      * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**      * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**      * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**      * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**      * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**      * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**      * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**      * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file OW1.c
** @version 01.00
** @brief
**
This is a component implementing the 1-Wire protocol.
*/         
/*!
**  @addtogroup OW1_module OW1 module documentation
**  @{
*/         

/* MODULE OW1. */

#include "OW1.h"

#define INPUT          0U
#define OUTPUT         1U
#define DQ_Init()               DQ1_Init()
#define DQ_Floating()           DQ1_SetInput()
#define DQ_SetLow()             DQ1_ClrVal()
#define DQ_Low()                DQ1_SetOutput()
#if OW1_CONFIG_DEBUG_READ_PIN_ENABLED
  #define DBG_Init()              OW1_CONFIG_DEBUG_READ_PIN_INIT()
  #define DQ_Read()               (OW1_CONFIG_DEBUG_READ_PIN_TOGGLE(), DQ1_GetVal()!=0)
#else
  #define DBG_Init()              /* empty */
  #define DQ_Read()               DQ1_GetVal()!=0
#endif

/* timer macros */
#if OW1_CONFIG_TIMER_UNIT_LDD
  #define TU_Reset()
#else
#endif

#if OW1_CONFIG_TIMER_UNIT_LDD
  #define TU_Init()               NULL
  #define TU_Deinit()             /* empty */
  #define TU_GetTimerFrequency()  0
  #define TU_Disable()            /* empty */
  #define TU_ResetCounter()       /* empty */
  #define TU_SetPeriodUS(us)      /* empty */
  #define TU_Enable()             /* empty */
#else
  #define TU_Init()               NULL
  #define TU_Deinit()             /* empty */
  #define TU_GetTimerFrequency()  0
  #define TU_Disable()            /* empty */
  #define TU_ResetCounter()       /* empty */
  #define TU_SetPeriodUS(us)      /* empty */
  #define TU_Enable()             /* empty */
#endif

typedef enum {
  I_RESET, /* reset instruction */
  I_SEND,  /* send data instruction */
  I_RECV,  /* receive data instruction */
  I_WAIT,  /* wait instruction */
  I_EVENT  /* event instruction */
} INSTR;

typedef enum {
  TS_NOTHING, /* end of timer step */
  /* I_RESET: reset sequence */
  TS_RESET_LOW, /* start of reset, pulling pin LOW */
  TS_RESET_FLOAT, /* reset sequence, release pin */
  TS_PRESENCE, /* check for presence */
  /* I_SEND: writing sequence */
  TS_WRITE_LOW, /* strt of write, pulling pin LOW */
  TS_WRITE_FLOAT, /* writing sequence, releasing pin */
  /* I_RECV: reading sequence */
  TS_READ_LOW, /* start of read, pulling pin low */
  TS_READ_FLOAT, /* middle of read, releasing pin */
  TS_READ_GET, /* reading data */
  /* I_WAIT_ waiting sequence */
  TS_WAIT_INTER,
  TS_WAIT,
  TS_EVENT
} OW1_TimerStep;

typedef struct {
  INSTR Instr     :3; /* instruction code */
  unsigned Count  :5; /* e.g. how many bytes to send */
} PROG;

struct {
#if !MCUC1_CONFIG_NXP_SDK_USED
  LDD_TDeviceData *TUDeviceDataPtr; /* timer handle */
#endif
  OW1_TimerStep Step;
  uint16 Ticks; /* number of timer ticks for one us */
  uint16 WaitTime;
  uint32 WaitTotal;
  uint16 WaitDiv;
  uint16 WaitRes;
  uint8 CRC;
  OW1_Error Error;
  PROG Prog;
  uint8 WorkByte;
  unsigned ToWork     :6;
  unsigned WorkBit    :1;
  volatile unsigned Busy       :1;
  unsigned WorkBitPos :3;
  unsigned WaitEvent  :1;
  unsigned SkipWEvent :1;
  unsigned WaitKey    :5;
} Data;

static void TU_SetTime(uint32_t us) {
  TU_Disable();
  TU_ResetCounter();
  TU_SetPeriodUS(us);
  TU_Enable();
}

void OW1_OnTimerRestart(void) {
  switch(Data.Step) {
    case TS_NOTHING:
      if(ProgramRB1_NofElements()==0) {
        TU_Disable();
        Data.Busy = FALSE;
      } else {
        OW1_i_action();
      }
      break;
    case TS_RESET_LOW:
      OW1_i_reset();
      break;
    case TS_RESET_FLOAT:
      DQ_Floating();
      TU_SetTime(OW1_CONFIG_I_RESPONSE_TIME); /* have it floating for this time until handling of TS_PRESENCE */
      Data.Step = TS_PRESENCE;
      break;
    case TS_PRESENCE:
      OW1_i_presence();
      break;
    case TS_WRITE_LOW:
      OW1_i_send_low();
      break;
    case TS_WRITE_FLOAT:
      OW1_i_send_float();
      break;
    case TS_READ_LOW:
      OW1_i_recv_low();
      break;
    case TS_READ_FLOAT:
      OW1_i_recv_float();
      break;
    case TS_READ_GET:
      OW1_i_recv_get();
      break;
    case TS_WAIT_INTER:
      Data.WaitDiv--;
      if(Data.WaitDiv == 0) {
        TU_SetPeriodUS(Data.WaitRes);
        Data.Step = TS_WAIT;
      }
      break;
    case TS_WAIT:
      Data.WaitKey = Data.Prog.Count;
      OW1_i_action();
      Data.WaitEvent = TRUE;
      Data.SkipWEvent = TRUE;
      break;
    case TS_EVENT:
      OW1_i_action();
      break;
    default:
      break;
  } /* switch */
  if(Data.SkipWEvent) {
    Data.SkipWEvent = FALSE;
  } else {
    if(Data.WaitEvent) {
      Data.WaitEvent = FALSE;
    }
  }
}

/*
** ===================================================================
**     Method      :  OW1_GetError (component OneWire)
**     Description :
**         Returns the error
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
OW1_Error OW1_GetError(void)
{
  return Data.Error;
}

/*
** ===================================================================
**     Method      :  OW1_SendBytes (component OneWire)
**     Description :
**         Sends multiple bytes
**     Parameters  :
**         NAME            - DESCRIPTION
**       * data            - Array of bytes to add to output stream.
**         count           - Number of bytes to add to output
**                           stream. (Valid range 0 - 31)
**     Returns     :
**         ---             - error or not
** ===================================================================
*/
bool OW1_SendBytes(uint8_t *data, uint8_t count)
{
  PROG pr;
  PROG last;

  (void)ProgramRB1_Peek(0, (uint8_t*)&last);
  if((last.Instr != I_SEND && ProgramRB1_NofFreeElements()==0) || (OutputRB1_NofFreeElements() < count)) {
    return FALSE;
  }
  pr.Instr = I_SEND;
  pr.Count = count;
  for(;count>0;count--) {
    (void)OutputRB1_Put(*data);
    data++;
  }
  if(last.Instr == I_SEND) { /* update last element */
    last.Count += pr.Count; /* update */
    (void)ProgramRB1_Update(0, (uint8_t*)&last); /* update */
  } else {
    (void)ProgramRB1_Put(*(uint8_t*)&pr);
  }
  if(!Data.Busy) {
    OW1_i_action();
    OW1_i_run();
    OW1_i_send_low();
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_i_run (component OneWire)
**     Description :
**         Instruction run
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_run(void)
{
  Data.Busy = TRUE;
  TU_ResetCounter();
  TU_Enable();
}

/*
** ===================================================================
**     Method      :  OW1_i_action (component OneWire)
**     Description :
**         action instruction
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_action(void)
{
  if(ProgramRB1_NofElements()==0) {
    TU_SetTime(OW1_CONFIG_SLOT_TIME);
    Data.Step = TS_NOTHING;
    return;
  }
  (void)ProgramRB1_Get((uint8_t*)&Data.Prog);
  switch(Data.Prog.Instr) {
    case I_RESET:
      TU_SetTime(OW1_CONFIG_H_RESET_TIME);
      Data.Step = TS_RESET_LOW;
      break;
    case I_SEND:
      Data.ToWork = Data.Prog.Count;
      OW1_i_send_float();
      break;
    case I_RECV:
      Data.ToWork = Data.Prog.Count;
      Data.WorkByte = 0;
      TU_SetTime(OW1_CONFIG_A_READ_TIME);
      Data.Step = TS_READ_LOW;
      break;
    case I_WAIT:
      (void)TimeRB1_Get(&Data.WaitTime);
      Data.WaitTotal = (uint32) Data.WaitTime * Data.Ticks * 1000U;
      Data.WaitDiv = Data.WaitTotal >> 16;
      Data.WaitRes = Data.WaitTotal & 0x0000FFFF;
      if(Data.WaitDiv == 0) {
        TU_SetPeriodUS(Data.WaitRes);
        Data.Step = TS_WAIT;
      } else {
        TU_SetPeriodUS(0xFFFF);
        Data.Step = TS_WAIT_INTER;
      }
      break;
    case I_EVENT:
      TU_SetTime(OW1_CONFIG_H_RESET_TIME);
      Data.Step = TS_EVENT;
      break;
    default:
      break;
  } /* switch */
}

/*
** ===================================================================
**     Method      :  OW1_i_presence (component OneWire)
**     Description :
**         presence instruction
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_presence(void)
{
  if(DQ_Read()) {	//No response
    Data.Step = TS_NOTHING;
    Data.Error = OWERR_NO_DEVICE;
  } else {
    Data.Error = OWERR_OK;
    OW1_i_action();
  }
}

/*
** ===================================================================
**     Method      :  OW1_i_reset (component OneWire)
**     Description :
**         reset instruction
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_reset(void)
{
  DQ_Low();
  TU_SetTime(OW1_CONFIG_H_RESET_TIME);
  Data.Step = TS_RESET_FLOAT;
}

/*
** ===================================================================
**     Method      :  OW1_SendReset (component OneWire)
**     Description :
**         Sends a reset to the bus
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
bool OW1_SendReset(void)
{
  PROG pr;

  if(ProgramRB1_NofFreeElements()==0) {
    return FALSE;
  }
  pr.Instr = I_RESET;
  (void)ProgramRB1_Put(*(uint8_t*)&pr);
  if(!Data.Busy) { /* start action */
    OW1_i_action();
    OW1_i_reset();
    OW1_i_run();
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_i_send_low (component OneWire)
**     Description :
**         sending a low signal
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_send_low(void)
{
  DQ_Low();
  if(Data.WorkBit) {
    TU_SetTime(OW1_CONFIG_A_WRITE_1_LOW_TIME);
    Data.SkipWEvent = TRUE;
  } else {
    TU_SetTime(OW1_CONFIG_C_WRITE_0_LOW_TIME);
  }
  Data.Step = TS_WRITE_FLOAT; /* next step is to release pin */
}

/*
** ===================================================================
**     Method      :  OW1_i_send_float (component OneWire)
**     Description :
**         using data pin in input mode
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_send_float(void)
{
  DQ_Floating();
  if(Data.WorkBitPos==0) { /* all bits sent? */
    if(Data.ToWork) {
      (void)OutputRB1_Get(&Data.WorkByte);
      Data.ToWork--;
    } else {
      OW1_i_action();
      return;
    }
  }
  Data.WorkBit = Data.WorkByte & 1;
  Data.WorkByte >>= 1;
  if(Data.WorkBit) { /* set time for when we will get back to floating */
    TU_SetTime(OW1_CONFIG_B_WRITE_1_HIGH_TIME);
  } else {
    TU_SetTime(OW1_CONFIG_D_WRITE_0_HIGH_TIME);
  }
  Data.WorkBitPos++;
  Data.Step = TS_WRITE_LOW; /* next action for the next bit: pull LOW to start new bit */
}

/*
** ===================================================================
**     Method      :  OW1_SendByte (component OneWire)
**     Description :
**         Sends a single byte
**     Parameters  :
**         NAME            - DESCRIPTION
**         data            - Variable to save the byte.
**     Returns     :
**         ---             - error or not
** ===================================================================
*/
bool OW1_SendByte(uint8_t data)
{
  PROG pr;
  PROG last;

  (void)ProgramRB1_Peek(0, (uint8_t*)&last);
  if(last.Instr == I_SEND) {
    (void)OutputRB1_Put(data);
    last.Count++; /* update */
    (void)ProgramRB1_Update(0, (uint8_t*)&last); /* update element */
  } else {
    if(ProgramRB1_NofFreeElements()==0) {
      return FALSE;
    }
    (void)OutputRB1_Put(data);
    pr.Instr = I_SEND;
    pr.Count = 1;
    (void)ProgramRB1_Put(*(uint8_t*)&pr);
  }
  if(!Data.Busy) {
    OW1_i_action();
    OW1_i_send_low();
    OW1_i_run();
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_i_recv_get (component OneWire)
**     Description :
**         instruction to get a bit
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_recv_get(void)
{
  Data.WorkByte >>= 1;
  Data.WorkBit = DQ_Read();
  OW1_add_CRC(Data.WorkBit);
  Data.WorkByte += (Data.WorkBit ? 0x80 : 0);
  if(Data.WorkBitPos == 7) { //Full byte
    (void)InputRB1_Put(Data.WorkByte);
    Data.ToWork--;
    if(Data.ToWork == 0) {  //finish
      if(Data.CRC){
        Data.Error = OWERR_CRC;
      } else {
        Data.Error = OWERR_OK;
      }
      OW1_i_action();
      return;
    }
  }
  Data.WorkBitPos++;
  TU_SetTime(OW1_CONFIG_F_AFTER_READ_DELAY_TIME);
  Data.Step = TS_READ_LOW;
}

/*
** ===================================================================
**     Method      :  OW1_i_recv_float (component OneWire)
**     Description :
**         start receiving a byte in floating mode
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_recv_float(void)
{
  DQ_Floating();
  TU_SetTime(OW1_CONFIG_E_BEFORE_READ_DELAY_TIME);
  Data.SkipWEvent = TRUE;
  Data.Step = TS_READ_GET;
}

/*
** ===================================================================
**     Method      :  OW1_i_recv_low (component OneWire)
**     Description :
**         receive a bit from the low state
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_recv_low(void)
{
  DQ_Low();
  Data.SkipWEvent = TRUE;
  TU_SetTime(OW1_CONFIG_A_READ_TIME);
  Data.Step = TS_READ_FLOAT;
}

/*
** ===================================================================
**     Method      :  OW1_Receive (component OneWire)
**     Description :
**         Programs a read operation after the master send all in
**         output buffer. Don't use a SendReset while the data is
**         coming.
**     Parameters  :
**         NAME            - DESCRIPTION
**         counter         - Number of bytes to receive from
**                           slave
**     Returns     :
**         ---             - Returns FALSE if the program buffer is
**                           full or if count is greater than the free
**                           space of input buffer-
** ===================================================================
*/
bool OW1_Receive(uint8_t counter)
{
  PROG pr;

  if(ProgramRB1_NofFreeElements()==0) {
    return FALSE;
  }
  pr.Instr = I_RECV;
  pr.Count = counter;
  (void)ProgramRB1_Put(*(uint8_t*)&pr);
  if(!Data.Busy) {
    OW1_i_action();
    OW1_i_recv_low();
    OW1_i_run();
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_ProgramEvent (component OneWire)
**     Description :
**         Used to add a program event
**     Parameters  :
**         NAME            - DESCRIPTION
**         key             - Key to identificate the event, applies
**                           only if OnProgramEvent is enabled. (Valid
**                           range 0 - 31)
**     Returns     :
**         ---             - Returns false if the program buffer is
**                           full.
** ===================================================================
*/
bool OW1_ProgramEvent(uint8_t key)
{
  PROG pr;

  if(ProgramRB1_NofFreeElements()==0) {
    return FALSE;
  }
  pr.Instr = I_EVENT;
  pr.Count = key;
  (void)ProgramRB1_Put(*(uint8_t*)&pr);
  if(!Data.Busy) {
    OW1_i_action();
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_i_wait (component OneWire)
**     Description :
**         wait instruction
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_i_wait(void)
{
  Data.WaitEvent = FALSE;
}

/*
** ===================================================================
**     Method      :  OW1_Waitms (component OneWire)
**     Description :
**         Programs a pause between instruccions.
**     Parameters  :
**         NAME            - DESCRIPTION
**         key             - Key to identify the source of the event.
**         time_ms         - Value of time to wait.
**     Returns     :
**         ---             - Returns false if the program buffer or the
**                           time buffer is full.
** ===================================================================
*/
bool OW1_Waitms(uint8_t key, uint8_t time_ms)
{
  PROG pr;
  CS1_CriticalVariable()

  if((ProgramRB1_NofFreeElements()==0) || (TimeRB1_NofFreeElements()==0)) {
    return FALSE;
  }
  CS1_EnterCritical();
  pr.Instr = I_WAIT;
  pr.Count = key;
  (void)ProgramRB1_Put(*(uint8_t*)&pr);
  (void)TimeRB1_Put(time_ms);
  CS1_ExitCritical();
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_Count (component OneWire)
**     Description :
**         Returns the number of elements stored on input buffer that
**         are ready to read.
**     Parameters  : None
**     Returns     :
**         ---             - number of elements
** ===================================================================
*/
uint8_t OW1_Count(void)
{
  return InputRB1_NofElements();
}

/*
** ===================================================================
**     Method      :  OW1_add_CRC (component OneWire)
**     Description :
**         Adds a bit to CRC accumulator.
**     Parameters  :
**         NAME            - DESCRIPTION
**         bitValue        - Bit to add to CRC accumulator
**     Returns     : Nothing
** ===================================================================
*/
void OW1_add_CRC(uint8_t bitValue)
{
  bool fb;

  fb = (Data.CRC & 0x01) ^ bitValue;
  Data.CRC >>= 1;
  if(fb) {
    Data.CRC ^= 0x8C;
  }
}

/*
** ===================================================================
**     Method      :  OW1_GetByte (component OneWire)
**     Description :
**         Get a single byte from the bus
**     Parameters  :
**         NAME            - DESCRIPTION
**       * data            - 
**     Returns     :
**         ---             - error or not
** ===================================================================
*/
bool OW1_GetByte(uint8_t *data)
{
  if (InputRB1_NofElements()==0) {
    return FALSE;
  }
  (void)InputRB1_Get(data);
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_GetBytes (component OneWire)
**     Description :
**         Gets multiple bytes from the bus
**     Parameters  :
**         NAME            - DESCRIPTION
**       * data            - 
**         count           - 
**     Returns     :
**         ---             - error or not
** ===================================================================
*/
bool OW1_GetBytes(uint8_t *data, uint8_t count)
{
  if(count > InputRB1_NofElements()) {
    return FALSE;
  }
  for(;count>0;count--) {
    (void)InputRB1_Get(data);
    data++;
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  OW1_isBusy (component OneWire)
**     Description :
**         Returns TRUE if the bus is busy, FALSE otherwise
**     Parameters  : None
**     Returns     :
**         ---             - TRUE if device is busy
** ===================================================================
*/
bool OW1_isBusy(void)
{
  return (Data.Busy!=0);
}


/*
** ===================================================================
**     Method      :  OW1_Init (component OneWire)
**     Description :
**         Initializes this device.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_Init(void)
{
#if MCUC1_CONFIG_NXP_SDK_USED
  /* using SDK, need to initialize inherited components */
  DQ_Init();
  DBG_Init();
  InputRB1_Init(); /* input ringbuffer */
  OutputRB1_Init(); /* output ringbuffer */
  ProgramRB1_Init(); /* program ringbuffer */
  TimeRB1_Init(); /* time ringbuffer */
#else
  Data.TUDeviceDataPtr = TU_Init(); /* timer init */
#endif
  Data.Ticks = TU_GetTimerFrequency()/1000000U;
  Data.Busy = FALSE;
  Data.WorkBitPos = 0;
  Data.WaitEvent = FALSE;
  Data.SkipWEvent = FALSE;
  Data.Step = TS_NOTHING;
  Data.CRC = 0;
  Data.Error = OWERR_OK;

  DQ_Floating(); /* input mode, let the pull-up take the signal high */
  /* load LOW to output register. We won't change that value afterwards, we only switch between output and input/float mode */
  DQ_SetLow();
}

/*
** ===================================================================
**     Method      :  OW1_Deinit (component OneWire)
**     Description :
**         Driver de-initialization
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void OW1_Deinit(void)
{
  TU_Deinit(); /* timer deinit */
#if !MCUC1_CONFIG_NXP_SDK_USED
  Data.TUDeviceDataPtr = NULL;
#endif
  Data.Ticks = 0;
  Data.Busy = FALSE;
  Data.WorkBitPos = 0;
  Data.WaitEvent = FALSE;
  Data.SkipWEvent = FALSE;
  Data.Step = TS_NOTHING;
  Data.CRC = 0;
  Data.Error = OWERR_OK;

  DQ_Floating(); /* input mode, tristate pin */
}


/*
** ===================================================================
**     Method      :  OW1_CalcCRC (component OneWire)
**     Description :
**         Calculates the CRC over a number of bytes
**     Parameters  :
**         NAME            - DESCRIPTION
**       * data            - Pointer to data
**         dataSize        - number of data bytes
**     Returns     :
**         ---             - calculated CRC
** ===================================================================
*/
uint8_t OW1_CalcCRC(uint8_t *data, uint8_t dataSize)
{
  uint8_t crc, i, x, y;

  crc = 0;
  for(x=0;x<dataSize;x++){
    y = data[x];
    for(i=0;i<8;i++) { /* go through all bits of the data byte */
      if((crc&0x01)^(y&0x01)) {
        crc >>= 1;
        crc ^= 0x8c;
      } else {
        crc >>= 1;
      }
      y >>= 1;
    }
  }
  return crc;
}

/* END OW1. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/