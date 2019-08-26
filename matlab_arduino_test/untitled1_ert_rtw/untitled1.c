/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: untitled1.c
 *
 * Code generated for Simulink model 'untitled1'.
 *
 * Model version                  : 1.0
 * Simulink Coder version         : 8.9 (R2015b) 13-Aug-2015
 * C/C++ source code generated on : Fri Oct 21 14:39:34 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "untitled1.h"
#include "untitled1_private.h"

/* Block states (auto storage) */
DW_untitled1_T untitled1_DW;

/* Real-time model */
RT_MODEL_untitled1_T untitled1_M_;
RT_MODEL_untitled1_T *const untitled1_M = &untitled1_M_;

/* Model step function */
void untitled1_step(void)
{
  real_T rtb_PulseGenerator;
  uint8_T rtb_PulseGenerator_0;

  /* DiscretePulseGenerator: '<Root>/Pulse Generator' */
  rtb_PulseGenerator = (untitled1_DW.clockTickCounter <
                        untitled1_P.PulseGenerator_Duty) &&
    (untitled1_DW.clockTickCounter >= 0L) ? untitled1_P.PulseGenerator_Amp : 0.0;
  if (untitled1_DW.clockTickCounter >= untitled1_P.PulseGenerator_Period - 1.0)
  {
    untitled1_DW.clockTickCounter = 0L;
  } else {
    untitled1_DW.clockTickCounter++;
  }

  /* End of DiscretePulseGenerator: '<Root>/Pulse Generator' */

  /* DataTypeConversion: '<S1>/Data Type Conversion' */
  if (rtb_PulseGenerator < 256.0) {
    if (rtb_PulseGenerator >= 0.0) {
      rtb_PulseGenerator_0 = (uint8_T)rtb_PulseGenerator;
    } else {
      rtb_PulseGenerator_0 = 0U;
    }
  } else {
    rtb_PulseGenerator_0 = MAX_uint8_T;
  }

  /* End of DataTypeConversion: '<S1>/Data Type Conversion' */

  /* S-Function (arduinodigitaloutput_sfcn): '<S1>/Digital Output' */
  MW_digitalWrite(untitled1_P.DigitalOutput_pinNumber, rtb_PulseGenerator_0);
}

/* Model initialize function */
void untitled1_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(untitled1_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&untitled1_DW, 0,
                sizeof(DW_untitled1_T));

  /* Start for DiscretePulseGenerator: '<Root>/Pulse Generator' */
  untitled1_DW.clockTickCounter = 0L;

  /* Start for S-Function (arduinodigitaloutput_sfcn): '<S1>/Digital Output' */
  MW_pinModeOutput(untitled1_P.DigitalOutput_pinNumber);
}

/* Model terminate function */
void untitled1_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
