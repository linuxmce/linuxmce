/*
 *  Driver for the TeVii S470 PCIe DVB-S card
 *  Igor M. Liplianin's remark:
 *  convert to i2c_algo_bit
 */

#include "cx23885.h"

// Direction bits are at offset 23:16
// This will create a mask value that needs to be ored with the old
// value to set the relevant bit.
#define Set_GPIO_Direction_Bit_To_OUTPUT(Bit)   ((1 << Bit) << 16)
// This will create a mask value that needs to be anded with the old
// value to clear the relevant bit.
#define Set_GPIO_Direction_Bit_To_INPUT(Bit)    (~((1 << Bit) << 16))
// This will create a mask value that needs to be ored with the old
// value to set the relevant bit.
#define Set_GPIO_Bit(Bit)                       (1 << Bit)
// This will create a mask value that needs to be anded with the old
// value to clear the relevant bit.
#define Clear_GPIO_Bit(Bit)                     (~(1 << Bit))

void SetGpioPinDirection(struct dvb_frontend *fe, int pinNumber, int pinLogicValue)
{
	u32 gpioRegister, value = 0;
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	gpioRegister = cx_read(GP0_IO);

	if (pinLogicValue == 0) {
		value = gpioRegister & Set_GPIO_Direction_Bit_To_INPUT(pinNumber) ;
	} else {
		value = gpioRegister | Set_GPIO_Direction_Bit_To_OUTPUT(pinNumber) ;
	}

	cx_write(GP0_IO, value);

}

void SetGpioPinLogicValue(struct dvb_frontend *fe, int pinNumber, int pinLogicValue)
{
	u32 gpioRegister, value = 0;
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	gpioRegister = cx_read(GP0_IO);
	// first do a sanity check - if the Pin is not output, make it output
	if ((gpioRegister & Set_GPIO_Direction_Bit_To_OUTPUT(pinNumber)) == 0x00) {
		// It is input.
		value = gpioRegister | Set_GPIO_Direction_Bit_To_OUTPUT(pinNumber) ;
		cx_write(GP0_IO, value);
		value = 0;
		//read the value back
		gpioRegister = cx_read(GP0_IO);
	}

	if (pinLogicValue == 0) {
		value = gpioRegister & Clear_GPIO_Bit(pinNumber);
	} else {
		value = gpioRegister | Set_GPIO_Bit(pinNumber);
	}

	cx_write(GP0_IO, value);
}

u32 GetGpioPinValue(struct dvb_frontend *fe, int pinNumber)
{
	u32 gpioRegister, value = 0;
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	gpioRegister = cx_read(GP0_IO);

	value = ((gpioRegister >> pinNumber) >> 8) & 1;

	return value;

}
void Set_IC_CLK(struct dvb_frontend *fe, unsigned char HL_Level)
{
	if (HL_Level == 1) {
		//cx_set(GP0_IO, 0x00040004);//set gpio 2
		SetGpioPinDirection(fe, 2, 1);
		SetGpioPinLogicValue(fe, 2, 1);
	} else {
		//cx_clear(GP0_IO, 0x00000004);//clear gpio 2
		SetGpioPinDirection(fe, 2, 1);
		SetGpioPinLogicValue(fe, 2, 0);
	}

	return;
}

void Set_IC_IO(struct dvb_frontend *fe, unsigned char HL_Level)
{
	if (HL_Level == 1) {
		//cx_set(GP0_IO, 0x00010001);//set gpio 0
		SetGpioPinDirection(fe, 0, 1);
		SetGpioPinLogicValue(fe, 0, 1);
	} else {

		//cx_clear(GP0_IO, 0x00000001);//clear gpio 0
		SetGpioPinDirection(fe, 0, 1);
		SetGpioPinLogicValue(fe, 0, 0);
	}

	return;
}

int Get_IC_IO(struct dvb_frontend *fe)
{
	int HL_Level;
	// cx_clear(GP0_IO, 0x00010000);
	//HL_Level = (cx_read(GP0_IO)>>8)&1;//get gpio 0

	SetGpioPinDirection(fe, 0, 0);
	HL_Level = GetGpioPinValue(fe, 0);
	return HL_Level;
}

void Set_IC_RST(struct dvb_frontend *fe, unsigned char HL_Level)
{
	if (HL_Level == 1) {
		//cx_set(GP0_IO, 0x00020002);//set gpio 1
		SetGpioPinDirection(fe, 1, 1);
		SetGpioPinLogicValue(fe, 1, 1);
	} else {
		//cx_clear(GP0_IO, 0x00000002);//clear gpio 0
		SetGpioPinDirection(fe, 1, 1);
		SetGpioPinLogicValue(fe, 1, 0);
	}

	return;
}


static unsigned char Get_IC_BUSY(struct dvb_frontend *fe)
{
	unsigned char HL_Level;

	SetGpioPinDirection(fe, 3, 0);
	HL_Level = (unsigned char)GetGpioPinValue(fe, 3);

	return HL_Level;
}



static void Delay1mS(void)
{
	udelay(800);


}

static void Clk_Delay(void)
{
	udelay(30);

}


static void SetMCU_CLk(struct dvb_frontend *fe, unsigned char HL_level)
{

	if (HL_level != 0)
		HL_level = 1;

	Set_IC_CLK(fe, HL_level);

	Clk_Delay();

	return;
}



static void SetMCU_Data(struct dvb_frontend *fe, unsigned char HL_level)
{

	if (HL_level != 0)
		HL_level = 1;

	Set_IC_IO(fe, HL_level);

	return;

}
static void SetMCU_Cs(struct dvb_frontend *fe, unsigned char HL_level)
{

	if (HL_level != 0)
		HL_level = 1;

	Set_IC_RST(fe, HL_level);

	return;
}

static unsigned char GetMCU_Data(struct dvb_frontend *fe)
{
	unsigned char linedata;

	linedata = (unsigned char)Get_IC_IO(fe);

	if (linedata != 0)
		return 1;
	else
		return 0;
}


static unsigned char GetMCU_Busy(struct dvb_frontend *fe)
{
	unsigned char LineBusy;


	LineBusy = Get_IC_BUSY(fe);

	if (LineBusy != 0)
		return 1;
	else
		return 0;
}

static void SendOneByteData(struct dvb_frontend *fe, unsigned char OneByteData)
{
	unsigned char i;


	for (i = 0;i < 8;i++) {
		SetMCU_CLk(fe, 0);
		SetMCU_Data(fe, OneByteData & 0x80); //msb first;
		OneByteData *= 2;
		SetMCU_CLk(fe, 1);
	}
	return;
}


static unsigned char GetOneByteData(struct dvb_frontend *fe)
{
	unsigned char i, OneByteData = 0;


	for (i = 0;i < 8;i++) {
		SetMCU_CLk(fe, 0);
		OneByteData *= 2;
		SetMCU_CLk(fe, 1);
		OneByteData += GetMCU_Data(fe);  //msb first;
	}

	return OneByteData;
}

static unsigned char QuestionAndAnswer(struct dvb_frontend *fe, unsigned char *QuestionBuffer)
{
	unsigned char i, temp, result;


	temp = QuestionBuffer[0];
	for (i = 0;i < QuestionBuffer[0]; i++)
		temp += QuestionBuffer[i+1];
	temp = (~temp + 1);  //get check sum;
	QuestionBuffer[1+ QuestionBuffer[0]] = temp;

	SetMCU_Cs(fe, 1);
	SetMCU_CLk(fe, 1);
	SetMCU_Data(fe, 1);
	Delay1mS();

	//question:
	SetMCU_Cs(fe, 0); //begain to send data;
	Delay1mS();
	SendOneByteData(fe, 0xe0);  // the slave address is 0xe0, write;
	Delay1mS();
	temp = QuestionBuffer[0];
	temp += 2;
	for (i = 0;i < temp;i++)
		SendOneByteData(fe, QuestionBuffer[i]);
	SetMCU_Cs(fe, 1); //sent data over;
	SetMCU_Data(fe, 1);

	//answer:
	temp = 0;
	for (i = 0;((i < 8)&(temp == 0));i++) {
		Delay1mS();
		if (GetMCU_Busy(fe) == 0)
			temp = 1;
	}
	if (i > 7) {
		// printk(" timeout, the slave no response\n");
		result = 0; // timeout, the slave no response;
	} else { //the slave not busy, prepare for getting data;
		SetMCU_Cs(fe, 0); //ready...
		Delay1mS();
		SendOneByteData(fe, 0xe1);  // 0xe1 is Read;
		Delay1mS();
		temp = GetOneByteData(fe); //get the data length;
		if (temp > 14)
			temp = 14;
		QuestionBuffer[0] = temp; // save data length;
		for (i = 0;i < (temp + 1);i++)
			QuestionBuffer[i+1] = GetOneByteData(fe);
		SetMCU_Cs(fe, 1); //received data over;
		SetMCU_Data(fe, 1);


		//check the received data:
		temp = 0;
		for (i = 0;i < (QuestionBuffer[0] + 2);i++)
			temp += QuestionBuffer[i];
		if (temp == 0) {
			/* printk("checksum ok %x %x %x %x %x %x %x %x %x %x\n",QuestionBuffer[0],
			QuestionBuffer[1],QuestionBuffer[2],QuestionBuffer[3],QuestionBuffer[4],
			QuestionBuffer[5],QuestionBuffer[6],QuestionBuffer[7],QuestionBuffer[8]
			,QuestionBuffer[9]); */
			result = 1;   //checksum ok;
		} else {
			/* printk("checksum fail %x %x %x %x %x %x %x %x %x %x\n",QuestionBuffer[0],
			QuestionBuffer[1],QuestionBuffer[2],QuestionBuffer[3],QuestionBuffer[4],
			QuestionBuffer[5],QuestionBuffer[6],QuestionBuffer[7],QuestionBuffer[8]
			,QuestionBuffer[9]); */
			result = 0;  // checksum fail;
		}
	}

	return result;
}

int tevii_dvbs2_set_voltage(struct dvb_frontend *fe,
                            fe_sec_voltage_t voltage)
{
	unsigned char QuestionBuffer[16];
	//printk("tevii_dvbs2_set_voltage\n");
	switch (voltage) {
	case SEC_VOLTAGE_13:

		QuestionBuffer[0] = 0x05;
		QuestionBuffer[1] = 0x38;   //write port;
		QuestionBuffer[2] = 0x01;   // A port, lnb power
		QuestionBuffer[3] = 0x01;
		QuestionBuffer[4] = 0x02;   // B port, H/V
		QuestionBuffer[5] = 0x00;       //13V   v

		break;
	case SEC_VOLTAGE_18:

		QuestionBuffer[0] = 0x05;
		QuestionBuffer[1] = 0x38;   //write port;
		QuestionBuffer[2] = 0x01;   // A port, lnb power
		QuestionBuffer[3] = 0x01;
		QuestionBuffer[4] = 0x02;   // B port, H/V
		QuestionBuffer[5] = 0x01;       //18V   h
		break;
	case SEC_VOLTAGE_OFF:

		QuestionBuffer[0] = 0x05;
		QuestionBuffer[1] = 0x38;   //write port;
		QuestionBuffer[2] = 0x01;   // A port, lnb power
		QuestionBuffer[3] = 0x00;
		QuestionBuffer[4] = 0x00;   // B port, H/V
		QuestionBuffer[5] = 0x00;       //18V
		break;
	}
	QuestionAndAnswer(fe, QuestionBuffer);

	return 0;
}
