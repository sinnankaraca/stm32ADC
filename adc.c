/*******************************************************************************
  * File Name         : adc.c
  * Description       : ADC channel reading for Port C 10,11,12 pins
  * 
  * Author            : Sinan KARACA
  * Date              : September 28, 2021				 
  ******************************************************************************
*/

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "common.h"
ADC_HandleTypeDef hadc1;



// FUNCTION      : adcInit
// DESCRIPTION   : initializes gpio pins of port C and ADC 
// PARAMETERS    : none
// RETURNS       : none
void adcInit(void)
{
	//initializing gpio as analog
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_0|
	GPIO_PIN_1|GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	//initializing adc
	__HAL_RCC_ADC1_CLK_ENABLE();
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge =
	ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
}



// FUNCTION      : adcRead
// DESCRIPTION   : ADC read of specified channels 
// PARAMETERS    : channel number - 10, 11, 12
// RETURNS       : none
int adcRead(uint32_t channel)
{
	//read adc
	int adcVal = 0;
	double voltage = 0;
	int adcResolution = 4095;
	double systemVoltage = 3.3;

	HAL_StatusTypeDef rc;
	ADC_ChannelConfTypeDef sConfig;
	
	if (channel == 10){
	  sConfig.Channel = ADC_CHANNEL_10;
	} else if (channel == 11){
	  sConfig.Channel = ADC_CHANNEL_11;
	} else if (channel == 12){
	  sConfig.Channel = ADC_CHANNEL_12;
	}
	
	sConfig.Rank = 1; 
	//sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	
	rc = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	if ( rc != HAL_OK) {
	printf("ADC channel configure failed with rc=%u\n",(unsigned)rc);
	return 0;
	}

	rc = HAL_ADC_Start(&hadc1);
	if(rc != HAL_OK) {
	printf("ADC start failed with rc=%u\n",(unsigned)rc);
	return 0;
	}
	
	rc = HAL_ADC_PollForConversion(&hadc1, 100);
	if(rc != HAL_OK) {
	printf("ADC poll for conversion failed with ""rc=%u\n",(unsigned)rc);
	return 0;
	}	
	
	adcVal = HAL_ADC_GetValue(&hadc1);
	rc = HAL_ADC_Stop(&hadc1);
	if(rc != HAL_OK) {
	printf("ADC stop failed with ""rc=%u\n",(unsigned)rc);
	return 0;
	}
	
	voltage = (adcVal * systemVoltage) / adcResolution;

	
	printf("P%ld, Value = %d, V = %.2lf  ",channel, adcVal, voltage);
	HAL_Delay(500);
	
	//Preventing reset
	WDTFeed();
	
	return 0;
}


// FUNCTION      : adcAssignment
// DESCRIPTION   : Connection between terminal and STM32
//       Calls the function for reading ADC
// PARAMETERS    : int action 
// RETURNS       : CmdReturnOk

ParserReturnVal_t adcAssignment(int action)
{

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("testasm\n\n"
	   "This command tests a new command\n"
	   );

    return CmdReturnOk;
  }
  
  int rc,rc1,rc2,rc3;
  uint32_t channel1, channel2, channel3, times;
  rc = fetch_uint32_arg(&times);
  if(rc) {
  printf("Must specify the sampling amount for adc\n");
  return CmdReturnBadParameter1;
  }
  
  
  rc1 = fetch_uint32_arg(&channel1);
  rc2 = fetch_uint32_arg(&channel2);
  rc3 = fetch_uint32_arg(&channel3);
  
  if(rc) {
  printf("Must specify channel for adc\n");
  return CmdReturnBadParameter1;
  }
  
  if(rc1) {
  printf("Must specify channel for adc\n");
  return CmdReturnBadParameter1;
  }
  
  
  adcInit();
  
  for (int i = 0; i < times; i++){
  
    if(!rc1 && channel1 > 9 && channel1 < 13 ){
      adcRead(channel1);

    }
    
    if(!rc2 &&channel2 > 9 && channel2 < 13 && channel1 != channel2){
      adcRead(channel2);
    }
  
    if(!rc3 && channel3 > 9 && channel3 < 13 && channel2 != channel3 && channel1 != channel3){
      adcRead(channel3);
    }
    printf("\n");
      
  }
  

  return CmdReturnOk;
}

ADD_CMD("ADC",adcAssignment,"<Sampling Time> <Pin 10,11,12>")




