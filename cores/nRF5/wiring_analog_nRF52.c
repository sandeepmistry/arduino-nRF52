/*
  Copyright (c) 2014 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef NRF52

#include "nrf.h"

#include "Arduino.h"
#include "wiring_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t saadcReference = SAADC_CH_CONFIG_REFSEL_Internal;
static uint32_t saadcGain      = SAADC_CH_CONFIG_GAIN_Gain1_5;

#define PWM_COUNT 3

static NRF_PWM_Type* pwms[PWM_COUNT] = {
  NRF_PWM0,
  NRF_PWM1,
  NRF_PWM2
};

#ifndef NEW_PWM
static uint32_t pwmChannelPins[PWM_COUNT] = {
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF
};
static uint16_t pwmChannelSequence[PWM_COUNT];

#else	// NEW-PWM ==============================

#define NRF_PWM_CHANNEL_COUNT 4    
#define NRF_PWM_PIN_NOT_CONNECTED 0xFFFFFFFF
static uint32_t pwmFrequency[PWM_COUNT] = { 0, 0, 0 };
static uint16_t pwmTopCount[PWM_COUNT]  = { 0, 0, 0 };
static uint32_t pwmChannelPins[PWM_COUNT][NRF_PWM_CHANNEL_COUNT] = {
  {NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED,NRF_PWM_PIN_NOT_CONNECTED},
  {NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED,NRF_PWM_PIN_NOT_CONNECTED},
  {NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED,NRF_PWM_PIN_NOT_CONNECTED}
};
static uint16_t pwmChannelSequence[PWM_COUNT][NRF_PWM_CHANNEL_COUNT] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};
#endif	// NEW-PWM ==============================

static int readResolution = 10;
static int writeResolution = 8;

void analogReadResolution( int res )
{
  if (res<8)  res = 8;
  else
  if (res>10) res = 10;
  readResolution = res;
}

void analogWriteResolution( int res )
{
  writeResolution = res;
}

static inline uint32_t mapResolution( uint32_t value, uint32_t from, uint32_t to )
{
  if ( from == to )
  {
    return value ;
  }

  if ( from > to )
  {
    return value >> (from-to) ;
  }
  else
  {
    return value << (to-from) ;
  }
}

/*
 * Internal Reference is at 0.6v!
 * External Reference should be between 1v and VDDANA-0.6v=2.7v
 *
 * Warning : On Arduino Zero board the input/output voltage for SAMD21G18 is 3.3 volts maximum
 */
void analogReference( eAnalogReference ulMode )
{
  switch ( ulMode ) {
    case AR_DEFAULT:
    case AR_INTERNAL:
    default:
      saadcReference = SAADC_CH_CONFIG_REFSEL_Internal;
	  saadcGain      = SAADC_CH_CONFIG_GAIN_Gain1_5;	//0...3,0V
      break;

    case AR_VDD4:
      saadcReference = SAADC_CH_CONFIG_REFSEL_VDD1_4;	//0...VDD(3.3V)
	  saadcGain      = SAADC_CH_CONFIG_GAIN_Gain1_4;
      break;
  }
}

uint32_t analogRead( uint32_t ulPin )
{
  uint32_t pin = SAADC_CH_PSELP_PSELP_NC;
  uint32_t saadcResolution;
  uint32_t resolution;
  int16_t value;

  if (ulPin >= PINS_COUNT) {
    return 0;
  }

  ulPin = g_ADigitalPinMap[ulPin];

  switch ( ulPin ) {
    case 2:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput0;
      break;

    case 3:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput1;
      break;

    case 4:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput2;
      break;

    case 5:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput3;
      break;

    case 28:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput4;
      break;

    case 29:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput5;
      break;

    case 30:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput6;
      break;

    case 31:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput7;
      break;

    default:
      return 0;
  }

  if (readResolution <= 8) {
    resolution = 8;
    saadcResolution = SAADC_RESOLUTION_VAL_8bit;
  } else if (readResolution <= 10) {
    resolution = 10;
    saadcResolution = SAADC_RESOLUTION_VAL_10bit;
  } else if (readResolution <= 12) {
    resolution = 12;
    saadcResolution = SAADC_RESOLUTION_VAL_12bit;
  } else {
    resolution = 14;
    saadcResolution = SAADC_RESOLUTION_VAL_14bit;
  }

  NRF_SAADC->RESOLUTION = saadcResolution;

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);
  for (int i = 0; i < 8; i++) {
    NRF_SAADC->CH[i].PSELN = SAADC_CH_PSELP_PSELP_NC;
    NRF_SAADC->CH[i].PSELP = SAADC_CH_PSELP_PSELP_NC;
  }
  NRF_SAADC->CH[0].CONFIG =   ((SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESP_Pos)   & SAADC_CH_CONFIG_RESP_Msk)
                            | ((SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESN_Pos)   & SAADC_CH_CONFIG_RESN_Msk)
                            | ((saadcGain                     << SAADC_CH_CONFIG_GAIN_Pos)   & SAADC_CH_CONFIG_GAIN_Msk)
                            | ((saadcReference                << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk)
                            | ((SAADC_CH_CONFIG_TACQ_3us      << SAADC_CH_CONFIG_TACQ_Pos)   & SAADC_CH_CONFIG_TACQ_Msk)
                            | ((SAADC_CH_CONFIG_MODE_SE       << SAADC_CH_CONFIG_MODE_Pos)   & SAADC_CH_CONFIG_MODE_Msk);
  NRF_SAADC->CH[0].PSELN = pin;
  NRF_SAADC->CH[0].PSELP = pin;


  NRF_SAADC->RESULT.PTR = (uint32_t)&value;
  NRF_SAADC->RESULT.MAXCNT = 1; // One sample

  NRF_SAADC->TASKS_START = 0x01UL;

  while (!NRF_SAADC->EVENTS_STARTED);
  NRF_SAADC->EVENTS_STARTED = 0x00UL;

  NRF_SAADC->TASKS_SAMPLE = 0x01UL;

  while (!NRF_SAADC->EVENTS_END);
  NRF_SAADC->EVENTS_END = 0x00UL;

  NRF_SAADC->TASKS_STOP = 0x01UL;

  while (!NRF_SAADC->EVENTS_STOPPED);
  NRF_SAADC->EVENTS_STOPPED = 0x00UL;

  if (value < 0) {
    value = 0;
  }

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);

  return mapResolution(value, resolution, readResolution);
}


#ifndef NEW_PWM
// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue )
{
  if (ulPin >= PINS_COUNT) {
    return;
  }

  ulPin = g_ADigitalPinMap[ulPin];

  for (int i = 0; i < PWM_COUNT; i++) {
    if (pwmChannelPins[i] == 0xFFFFFFFF || pwmChannelPins[i] == ulPin) {
      pwmChannelPins[i] = ulPin;
      pwmChannelSequence[i] = ulValue;
	  pwmChannelSequence[i]|= 1<<15; //JH

      NRF_PWM_Type* pwm = pwms[i];

      pwm->PSEL.OUT[0] = ulPin;
      pwm->PSEL.OUT[1] = ulPin;
      pwm->PSEL.OUT[2] = ulPin;
      pwm->PSEL.OUT[3] = ulPin;
      pwm->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
      pwm->PRESCALER = PWM_PRESCALER_PRESCALER_DIV_2;
      pwm->MODE = PWM_MODE_UPDOWN_Up;
      pwm->COUNTERTOP = (1 << writeResolution) - 1;
      pwm->LOOP = 0;
      pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) 		//JH initPWM?
	               | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);	//JH initPWM?
      pwm->SEQ[0].PTR = (uint32_t)&pwmChannelSequence[i];
      pwm->SEQ[0].CNT = 1;
      pwm->SEQ[0].REFRESH  = 1;
      pwm->SEQ[0].ENDDELAY = 0;
      pwm->TASKS_SEQSTART[0] = 0x1UL;

      break;
    }
  }
}
#else
// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
extern  void a_printf(const char *fmt, ... );

void analogWrite(uint32_t ulPin, uint32_t ulValue )
{
	if (pwmFrequency[0]==0) initPwm(0, 500);
	analogWritePwm(ulPin, ulValue, 0);
}

void deinitPwm(uint8_t pwmId)
{
	if (pwmId>=PWM_COUNT) {
		return;
	}
	
	//nrf_pwm_disable(pwms[pwmId]);
	pwms[pwmId]->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
	pwmFrequency[pwmId] = 0;
	pwmTopCount[pwmId]  = 0;		
}
/*
 *
 */
void initPwm(uint8_t pwmId, uint32_t ulFreq)
{
	
	if (pwmId>=PWM_COUNT) {
		return;
	}
	
	if (ulFreq!=pwmFrequency[pwmId]) {
		uint32_t preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_1;
		uint32_t oFreq = ulFreq;
		if (ulFreq >512) {
			uint32_t maxFreq = 16000000/((1<<writeResolution)-1);
			if (ulFreq>maxFreq) ulFreq = maxFreq; //writeRes 8:62745Hz; 9:31331Hz; 10:15640Hz
			preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_1;
		}
		else if (ulFreq >256) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_2;
		else if (ulFreq >128) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_4;
		else if (ulFreq > 64) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_8;
		else if (ulFreq > 32) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_16;
		else if (ulFreq > 16) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_32;
		else if (ulFreq >  8) preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_64;
		else {
			if (ulFreq < 4) ulFreq = 4;
			preScaleDiv_X = PWM_PRESCALER_PRESCALER_DIV_128;
		}
		uint32_t topCntValue = 16000000/(ulFreq << preScaleDiv_X);	// max. 15bit! = 32767!

		//nrf_pwm_configure(pwms[pwmId], preScaleDiv_X, NRF_PWM_MODE_UP, (uint16_t)topCntValue);
		pwms[pwmId]->PRESCALER  = preScaleDiv_X;
        pwms[pwmId]->MODE       = PWM_MODE_UPDOWN_Up;
        pwms[pwmId]->COUNTERTOP = (uint16_t)topCntValue;
		
		NRF_PWM_Type* const pwm = pwms[pwmId];
		//nrf_pwm_enable(pwm);
		pwm->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

		//nrf_pwm_loop_set   (pwm, 0);
		pwm->LOOP = 0;
		//nrf_pwm_decoder_set(pwm, PWM_DECODER_LOAD_Individual, NRF_PWM_STEP_AUTO);	  
		pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Individual   << PWM_DECODER_LOAD_Pos)
                     | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
		//nrf_pwm_seq_ptr_set(pwm, 0, pwmChannelSequence[pwmId]);
		pwm->SEQ[0].PTR = (uint32_t)pwmChannelSequence[pwmId];
		//nrf_pwm_seq_cnt_set(pwm, 0, NRF_PWM_CHANNEL_COUNT);
		pwm->SEQ[0].CNT = NRF_PWM_CHANNEL_COUNT;
		//nrf_pwm_seq_refresh_set(pwm, 0, 1);
		pwm->SEQ[0].REFRESH  = 1;
		//nrf_pwm_seq_end_delay_set(pwm, 0, 0);
		pwm->SEQ[0].ENDDELAY = 0;
		//nrf_pwm_task_trigger(pwm, NRF_PWM_TASK_SEQSTART0);
		//*((volatile uint32_t *)((uint8_t *)pwm + (uint32_t)NRF_PWM_TASK_SEQSTART0)) = 0x1UL;
		pwm->TASKS_SEQSTART[0] = 0x1UL;
		
		pwmFrequency[pwmId] = ulFreq;
		pwmTopCount[pwmId]  = topCntValue;		
		//a_printf("pwmId:%d, of:%5dHz pf:%5dHz, T:%5d\n", pwmId, oFreq, ulFreq, topCntValue);
	}
}

void analogWritePwm( uint32_t ulPin, uint32_t ulValue, uint8_t pwmId)
{
	if (ulPin >= PINS_COUNT) {
		return;
	}
	if (pwmId >= PWM_COUNT) {
		return;
	}
	if (pwmFrequency[pwmId]==0) {	// frequency not set yet!
		initPwm(pwmId, 500); 		// also standard frequency
	}

	uint32_t aPin = ulPin;
	ulPin = g_ADigitalPinMap[ulPin];
	
	for (uint8_t ci = 0; ci < NRF_PWM_CHANNEL_COUNT; ci++) {
		if (pwmChannelPins[pwmId][ci] == NRF_PWM_PIN_NOT_CONNECTED || 
	        pwmChannelPins[pwmId][ci] == ulPin) {
			
			pwmChannelPins[pwmId][ci] = ulPin;
			NRF_PWM_Type* const pwm = pwms[pwmId];

			//nrf_pwm_pins_set(pwm, pwmChannelPins[pwmId]);
			for (uint8_t i = 0; i < NRF_PWM_CHANNEL_COUNT; ++i) {
				pwm->PSEL.OUT[i] = pwmChannelPins[pwmId][i];
			}

			//nrf_pwm_enable(pwm);
			//pwm->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

			uint32_t v = ((pwmTopCount[pwmId])*ulValue) / ((1<<writeResolution)-1);
			v = min(v, pwmTopCount[pwmId]);
			pwmChannelSequence[pwmId][ci] = (uint16_t)v;  // max 15bit!
			pwmChannelSequence[pwmId][ci]|= 1<<15;        // Arduino ON/OFF duty cycle (set invert bit)
	
			//a_printf("pwmId:%d, aP:%2d, nP:%2d, ci:%d, v:%4d f:%5dHz T:%5d V:%5d\n", pwmId, aPin, ulPin, ci, ulValue, pwmFrequency[pwmId], pwmTopCount[pwmId], v);
			pwm->SEQ[0].REFRESH  = 1;
			pwm->SEQ[0].ENDDELAY = 0;
			pwm->TASKS_SEQSTART[0] = 0x1UL;
			/* 
			//nrf_pwm_loop_set   (pwm, 0);
			pwm->LOOP = 0;
			//nrf_pwm_decoder_set(pwm, PWM_DECODER_LOAD_Individual, NRF_PWM_STEP_AUTO);	  
			pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Individual   << PWM_DECODER_LOAD_Pos)
                         | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
			//nrf_pwm_seq_ptr_set(pwm, 0, pwmChannelSequence[pwmId]);
			pwm->SEQ[0].PTR = (uint32_t)pwmChannelSequence[pwmId];
			//nrf_pwm_seq_cnt_set(pwm, 0, NRF_PWM_CHANNEL_COUNT);
			pwm->SEQ[0].CNT = NRF_PWM_CHANNEL_COUNT;
			//nrf_pwm_seq_refresh_set(pwm, 0, 1);
			pwm->SEQ[0].REFRESH  = 1;
			//nrf_pwm_seq_end_delay_set(pwm, 0, 0);
			pwm->SEQ[0].ENDDELAY = 0;
			//nrf_pwm_task_trigger(pwm, NRF_PWM_TASK_SEQSTART0);
			// *((volatile uint32_t *)((uint8_t *)pwm + (uint32_t)NRF_PWM_TASK_SEQSTART0)) = 0x1UL;
			pwm->TASKS_SEQSTART[0] = 0x1UL;
			*/
/* compare to origin:
      pwm->LOOP = 0;
      pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) 
	               | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
      pwm->SEQ[0].PTR = (uint32_t)&pwmChannelSequence[i];
      pwm->SEQ[0].CNT = 1;
      pwm->SEQ[0].REFRESH  = 1;
      pwm->SEQ[0].ENDDELAY = 0;
      pwm->TASKS_SEQSTART[0] = 0x1UL;
*/			
			break;
		}
	}
}
	
#endif

#ifdef __cplusplus
}
#endif

#endif
