/* =================================================================================
* ==================   RmDvADT7410.c	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 *

* =================================================================================*/




#include "RmDv_ADT7410.h"


I2C_RecSendData_Typedef I2C_Data_Struct;

uint8_t data[4];

// on shot mode, 16bits data format
void ADT7410_Init(void)
{
	I2C_L031_Init(I2C1);

	I2C_Data_Struct.Nb_Data=4;
	I2C_Data_Struct.Ptr_Data=data;
	I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
	data[0]=0; // Word Adress
	data[1]=0xA1;
	data[2]=0xB2;
	data[3]=0xC3;

	uint8_t ConfRegVal;
	ConfRegVal=ConfReg_Reso_16;
	ConfRegVal|=ConfReg_Mode_Shutdown;

	//data[0]=ConfRegAdr;
	//data[1]=ConfRegVal;



	I2C_L031_PutString(I2C1,&I2C_Data_Struct);
	// relecture pour voir
	data[1]=0xDD;
	data[2]=0xEE;
	data[3]=0xAA;
	I2C_L031_GetString(I2C1, &I2C_Data_Struct);

}


short int ADT7410_GetTemp_fract_9_7(void)
{
	uint8_t ConfRegVal;
	short int ReturnValue;

	// Lancement one shot
	ConfRegVal=ConfReg_Reso_16;
	ConfRegVal|=ConfReg_Mode_OneShot;
	data[0]=ConfRegAdr;
	data[1]=ConfRegVal;

	//HAL_I2C_Master_Transmit(&hi2c1, ADT7410_Slave8bitsAdr, data,2, HAL_MAX_DELAY);


	// Wait at least 240ms
	//HAL_Delay(250);


	// Read temperature
	//HAL_I2C_Mem_Read(&hi2c1, ADT7410_Slave8bitsAdr, TempHighAdr,1,data,2,HAL_MAX_DELAY);
	ReturnValue=(data[0]<<8)+data[1];

	return ReturnValue;

	}







/***************************************************************
		I2C1
***************************************************************/
void MX_I2C1_Init(void)
{

  LL_I2C_InitTypeDef I2C_InitStruct = {0};
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /**I2C1 GPIO Configuration voir RmDv_IO.c
  PA9   ------> I2C1_SCL
  PA10   ------> I2C1_SDA
  */

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x00506682;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
}
