/*
 * drv__mems_bno55.c
 *
 * Created: 16.04.2015 15:16:35
 *  Author: Tobias Rohde
 */ 
#include "ets_sys.h"
#include "osapi.h"
#include "driver/drv_mems_bno55.h"
#include "driver/i2c_master.h"
#include "driver/TWI_master.h"
#include "driver/uart.h"

extern int ets_uart_printf(const char *fmt, ...);

#define hal_delay_ms(ms) os_delay_us(ms*1000)
#define INFO ets_uart_printf

static  bno055_t bno055 = {
		.init_quat = 0,
		.selftest_res = 0,
		.calib_status = 0
};

static quat_t curr_quat;

//#define BNO_ADR (0x50) //(0x28)<<1
//#define BNO_ADR (0x52) //(0x29)<<1
// volatile uint8_t BNO_ADR = 0x50;
 uint8_t BNO_ADR = 0x50;

// prototype i2c write
static uint8 ICACHE_FLASH_ATTR write_data(uint8_t register_addr, int8_t *data, uint8_t length) {
	// i2c write logic (i2c write call)
	// uart protocol (responses) muss hier nicht implementiert werden.. denke mal das l‰uft ¸ber ACK/NACK -> hier werden keine BUS_OVERRUN_ERRORs kommen
	
//		hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_TX, false);
//		hal_i2c_write(register_addr);
//		for(int i=0; i<length; i++)
//		{
//			hal_i2c_write(data[i]);
//		}
//
//		hal_i2c_stop();

	i2c_master_start();
    i2c_master_writeByte(BNO_ADR << 1);
    if (i2c_master_getAck())
    {
    	i2c_master_stop();
        return 1;
    }
    os_delay_us(1000);
    i2c_master_writeByte(register_addr);
    if (i2c_master_getAck())
    {
    	i2c_master_stop();
        return 2;
    }

    uint8 i;
    for (i = 0; i < length; i++)
    {
    	i2c_master_writeByte(data[i]);
        if (i2c_master_getAck())
        {
        	i2c_master_stop();
            return 3;
        }
    }

    i2c_master_stop();
    return 0;
}

// prototype i2c read
static uint8 ICACHE_FLASH_ATTR read_data(uint8_t register_addr, int8_t *data, uint8_t length) {
	
//	hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_TX, false);
//	hal_i2c_write(register_addr);
//	hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_RX, true);
//	for(int i=0; i<length-1; i++)
//	{
//		data[i]=hal_i2c_read_ack();
//	}
//	data[length-1]=hal_i2c_read_nack();
//	hal_i2c_stop();
	
	// i2c read logic (i2c read call)
	// move read data into int8_t *data
	uint8 write_address;

	i2c_master_start();
	i2c_master_writeByte(BNO_ADR << 1);
	    if (i2c_master_getAck())
	    {
	    	i2c_master_stop();
	        return 1;
	    }

	    i2c_master_writeByte(register_addr);
	    if (i2c_master_getAck())
	    {
	    	i2c_master_stop();
	        return 2;
	    }

	    i2c_master_start();
	    write_address = BNO_ADR << 1;
	    write_address |= 1;
	    i2c_master_writeByte(write_address);
	    if (i2c_master_getAck())
	    {
	    	i2c_master_stop();
	        return 3;
	    }

	    uint8 i;
	    for (i = 0; i < length; i++)
	    {
	        data[i] = i2c_master_readByte();
	        if(i != length-1)
	        	i2c_master_setAck(0); //ACK
	    }
	    i2c_master_setAck(1); // NOACK
	    i2c_master_stop();

	    return 0;
}



static void ICACHE_FLASH_ATTR softi2c_write_data(uint8_t register_addr, int8_t *data, uint8_t length) {
	// i2c write logic (i2c write call)
	// uart protocol (responses) muss hier nicht implementiert werden.. denke mal das l‰uft ¸ber ACK/NACK -> hier werden keine BUS_OVERRUN_ERRORs kommen
	/*
	hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_TX, false);
	hal_i2c_write(register_addr);
	for(int i=0; i<length; i++)
	{
		hal_i2c_write(data[i]);
	}
	
	hal_i2c_stop(); */
	
	uint8_t tmp[100];

	tmp[0] = register_addr;
	int i = 1;
	for(i=1; i<length+1; i++) {
		tmp[i]=data[i-1];
	}

	write_data_soft(tmp, length+1);


}

// prototype i2c read
static void  ICACHE_FLASH_ATTR softi2c_read_data(uint8_t register_addr, int8_t *data, uint8_t length) {
	
	/*
	hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_TX, false);
	hal_i2c_write(register_addr);
	hal_i2c_start(BNO_ADR, I2C_DIR_MASTER_RX, true);
	for(int i=0; i<length-1; i++)
	{
		data[i]=hal_i2c_read_ack();
	}
	data[length-1]=hal_i2c_read_nack();
	hal_i2c_stop();
	
	*/
	
	if(write_data_soft(&register_addr,1) != 0)
	{
		read_bytes(data, length);
	}
	
	// i2c read logic (i2c read call)
	// move read data into int8_t *data
}


uint8_t ICACHE_FLASH_ATTR bno055_get_chipID()
{
	uint8_t retVal=0;
	read_data(0, &retVal, 1);
	return retVal;
}

uint8_t ICACHE_FLASH_ATTR softi2c_bno055_get_chipID()
{
	uint8_t retVal;
	softi2c_read_data(0, &retVal,1);
	return retVal;
}


/**
 * Check if quaternion mode initialized.
 */
int8_t bno055_get_quat_init(void) {
	return bno055.init_quat;
}

/**
 * Initialize bno055 for quaternion output.
 */

//void ICACHE_FLASH_ATTR bno055_init_amg_mode(void) {
//	int8_t data = OPERATION_MODE_CONFIG;
//
//
//	// Switch to configuration mode first, otherwise all write attempts will fail
//	write_data(BNO055_OPR_MODE_ADDR, &data, 1); 	// length of data is 1
//	hal_delay_ms(20); //<--- important (19ms switching time -> better 20ms) from config to other op_mode -> 7ms switching time
//
//	// Configure coordinate axis | res res z z y y x x | with x: 00, y: 01, z: 10
//	data = 0x24; // <-- 24 is standard axis configuration..
//	write_data(BNO055_AXIS_MAP_CONFIG_ADDR, &data, 1); // length of data is 1
//	hal_delay_ms(20);
//
//	// Configure coordinate axis signs (flip axis if wanted)
//	data = 0x00;	// no flip
//	write_data(BNO055_AXIS_MAP_SIGN_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	data = 0x01;
//	write_data(BNO055_PAGE_ID_ADDR, &data, 1);	// Change to page 1..
//	hal_delay_ms(20);
//
//	data = 0b00011011;	// op mode normal, range 16g, bw 500hz
//	write_data(ACCEL_CONFIG_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	data = 0b00010000;	// 2000dps, bw 116 Hz
//	write_data(GYRO_CONFIG_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	data = 0x00;	// normal mode
//	write_data(GYRO_MODE_CONFIG_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	data = 0x00;
//	write_data(BNO055_PAGE_ID_ADDR, &data, 1);	// Change back to page 0..
//	hal_delay_ms(20);
//
//	data = 0x01;
//	write_data(BNO055_UNIT_SEL_ADDR, &data, 1);	// Set Accelerometer output unit to mg
//	hal_delay_ms(20);
//
//	data = 0x00;
//	write_data(BNO055_PWR_MODE_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	data = OPERATION_MODE_AMG;
//	write_data(BNO055_OPR_MODE_ADDR, &data, 1);
//	hal_delay_ms(20);
//
//	// Read selftest status
//	// | res res res res ST_MCU ST_GYR ST_MAG ST_ACC |
//	// Should be read as 0x0F, otherwise something went wrong
//	read_data(BNO055_SELFTEST_RESULT_ADDR, &bno055.selftest_res, 1);
//	hal_delay_ms(200);
//
//	INFO("BNO Selftest res: 0x%x\r\n", bno055.selftest_res);
//}
//
//

void ICACHE_FLASH_ATTR bno055_init_quat_mode(void) {
//	if (bno055_get_quat_init())
//		return;	// Already initialized..

	int8_t data = OPERATION_MODE_CONFIG;


	// Switch to configuration mode first, otherwise all write attempts will fail
	if(write_data(BNO055_OPR_MODE_ADDR, &data, 1))return; 	// length of data is 1
	os_delay_us(2000); //<--- important (19ms switching time -> better 20ms) from config to other op_mode -> 7ms switching time

	// Configure coordinate axis | res res z z y y x x | with x: 00, y: 01, z: 10
	data = 0x24; // <-- 24 is standard axis configuration..
	if(write_data(BNO055_AXIS_MAP_CONFIG_ADDR, &data, 1))return; // length of data is 1
	os_delay_us(2000);

	// Configure coordinate axis signs (flip axis if wanted)
	data = 0x00;	// no flip
	if(write_data(BNO055_AXIS_MAP_SIGN_ADDR, &data, 1))return;
	os_delay_us(2000);

	// Switch operation mode to NDOF -> activate sensors and start fusion
	data = OPERATION_MODE_NDOF;
	if(write_data(BNO055_OPR_MODE_ADDR, &data, 1))return;
	os_delay_us(2000);

	// Read selftest status
	// | res res res res ST_MCU ST_GYR ST_MAG ST_ACC |
	// Should be read as 0x0F, otherwise something went wrong
	if(read_data(BNO055_SELFTEST_RESULT_ADDR, &bno055.selftest_res, 1))return;
	os_delay_us(2000);

	ets_uart_printf("BNO adr 0x%x -> Selftest result: 0x%d\r\n", BNO_ADR, bno055.selftest_res);

	// Set initialization flag
	bno055.init_quat = 1;

	return;
}

void  ICACHE_FLASH_ATTR softi2c_bno055_init_quat_mode(void) {
	//	if (bno055_get_quat_init())
	//		return;	// Already initialized..

	int8_t data = OPERATION_MODE_CONFIG;

	INFO("softi2c_bno055_init_quat_mode: START\r\n");

	// Switch to configuration mode first, otherwise all write attempts will fail
	softi2c_write_data(BNO055_OPR_MODE_ADDR, &data, 1); 	// length of data is 1
	hal_delay_ms(20); //<--- important (19ms switching time -> better 20ms) from config to other op_mode -> 7ms switching time
	INFO("BNO055_OPR_MODE_ADDR\r\n");

	// Configure coordinate axis | res res z z y y x x | with x: 00, y: 01, z: 10
	data = 0x24; // <-- 24 is standard axis configuration..
	softi2c_write_data(BNO055_AXIS_MAP_CONFIG_ADDR, &data, 1); // length of data is 1
	hal_delay_ms(20);
	INFO("BNO055_AXIS_MAP_CONFIG_ADDR\r\n");

	// Configure coordinate axis signs (flip axis if wanted)
	data = 0x00;	// no flip
	softi2c_write_data(BNO055_AXIS_MAP_SIGN_ADDR, &data, 1);
	hal_delay_ms(20);
	INFO("BNO055_AXIS_MAP_SIGN_ADDR\r\n");

	// Switch operation mode to NDOF -> activate sensors and start fusion
	data = OPERATION_MODE_NDOF;
	softi2c_write_data(BNO055_OPR_MODE_ADDR, &data, 1);
	hal_delay_ms(20);
	INFO("BNO055_OPR_MODE_ADDR\r\n");

	// Read selftest status
	// | res res res res ST_MCU ST_GYR ST_MAG ST_ACC |
	// Should be read as 0x0F, otherwise something went wrong
	softi2c_read_data(BNO055_SELFTEST_RESULT_ADDR, &bno055.selftest_res, 1);
	hal_delay_ms(200);

	INFO("Soft BNO Selftest (%x) res: 0x%x\r\n",    SLAVE_ADDRESS,bno055.selftest_res);

	// Set initialization flag
	bno055.init_quat = 1;

	INFO("softi2c_bno055_init_quat_mode: END\r\n");

	return;
}


/**
 * @return quat_t 'object'
 */
RetVal_t ICACHE_FLASH_ATTR bno055_read_quat(quat_t* pQuat) {
	// Check if quaternion mode initialized
	if (!bno055.init_quat)
		return LOBARO_NO_INIT;

	// Read quaternion
	read_data(BNO055_QUATERNION_DATA_W_LSB_ADDR, &(pQuat->data[0]), QUAT_BYTE_LENGTH);
	
	// Quaternion data is now stored in data as
	// data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
	// W_LSB  , W_MSB  , X_LSB  , X_MSB  , Y_LSB  , Y_MSB  , Z_LSB  , Z_MSB

	return LOBARO_OK;
}

RetVal_t ICACHE_FLASH_ATTR softi2c_bno055_read_quat(quat_t* pQuat) {
	// Check if quaternion mode initialized
//	if (!bno055.init_quat)
//	return LOBARO_NO_INIT;

	// Read quaternion
	softi2c_read_data(BNO055_QUATERNION_DATA_W_LSB_ADDR, &(pQuat->data[0]), QUAT_BYTE_LENGTH);
	
	// Quaternion data is now stored in data as
	// data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
	// W_LSB  , W_MSB  , X_LSB  , X_MSB  , Y_LSB  , Y_MSB  , Z_LSB  , Z_MSB

	return LOBARO_OK;
}






RetVal_t ICACHE_FLASH_ATTR bno055_read_euler(eul_t* pEul) {
	// Check if quaternion mode initialized
	if (!bno055.init_quat)
	return LOBARO_NO_INIT;

	// Read quaternion
	read_data(BNO055_QUATERNION_DATA_W_LSB_ADDR, &(pEul->data[0]), EULER_BYTE_LENGTH);
	
	// Quaternion data is now stored in data as
	// data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
	// W_LSB  , W_MSB  , X_LSB  , X_MSB  , Y_LSB  , Y_MSB  , Z_LSB  , Z_MSB

	return LOBARO_OK;
}

RetVal_t ICACHE_FLASH_ATTR softi2c_bno055_read_euler(eul_t* pEul) {
	// Check if quaternion mode initialized
	if (!bno055.init_quat)
	return LOBARO_NO_INIT;

	// Read quaternion
	softi2c_read_data(BNO055_EULER_H_LSB_ADDR, &(pEul->data[0]), EULER_BYTE_LENGTH);
	
	// Quaternion data is now stored in data as
	// data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
	// W_LSB  , W_MSB  , X_LSB  , X_MSB  , Y_LSB  , Y_MSB  , Z_LSB  , Z_MSB

	return LOBARO_OK;
}






uint8_t dummy[18];
RetVal_t ICACHE_FLASH_ATTR bno055_read_amg() {
	read_data(BNO055_ACCEL_DATA_X_LSB_ADDR, dummy, 6);
	
	return LOBARO_OK;
}



RetVal_t ICACHE_FLASH_ATTR bno055_disp_quat(quat_t* pQuat) {
	// Quaternion data is now stored in data as
	// data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
	// W_LSB  , W_MSB  , X_LSB  , X_MSB  , Y_LSB  , Y_MSB  , Z_LSB  , Z_MSB
	
	
	/*
	float qtw,qtx,qty,qtz, sum;
	
	qtw = (float) (( ((int16_t)pQuat->data[1]) << 8 ) | ((int16_t)(pQuat->data[0])));
	qtx = (float) (( ((int16_t)pQuat->data[3]) << 8 ) | ((int16_t)(pQuat->data[2])));
	qty = (float) (( ((int16_t)pQuat->data[5]) << 8 ) | ((int16_t)(pQuat->data[4])));
	qtz	= (float) (( ((int16_t)pQuat->data[7]) << 8 ) | ((int16_t)(pQuat->data[6])));
	

	qtw/=(float)(1<<14);
	qtx/=(float)(1<<14);
	qty/=(float)(1<<14);
	qtz/=(float)(1<<14);
	
	qtw*=qtw;
	qtx*=qtx;
	qty*=qty;
	qtz*=qtz;
	
	
	sum = qtw+qtx+qty+qtz;
	
	sum=sum*1000;
	
	
	INFO("\"Norm\":%ld\r\n", (uint32_t)sum);
	*/
	
	int32_t	qtw = 1;
	int32_t	qtx = 2;
	int32_t	qty = 3;
	int32_t	qtz	=4;
	
	qtw = (( ((int16_t)pQuat->data[1]) << 8 ) | ((int16_t)(pQuat->data[0])));
	qtx =  (( ((int16_t)pQuat->data[3]) << 8 ) | ((int16_t)(pQuat->data[2])));
	qty =  (( ((int16_t)pQuat->data[5]) << 8 ) | ((int16_t)(pQuat->data[4])));
	qtz	=  (( ((int16_t)pQuat->data[7]) << 8 ) | ((int16_t)(pQuat->data[6])));
	
	ets_uart_printf("w:%ld x:%ld y:%ld z:%ld\r\n", qtw,qtx,qty,qtz);
}



/**
 * @return calibration status
 */
uint8_t ICACHE_FLASH_ATTR bno055_read_calib(void) {
	// Check if quaternion mode initialized
	if (!bno055.init_quat)
		return 0;

	// Read calibration status
	// | SYS_CAL<0:1> GYR_CAL<0:1> ACC_CAL<0:1> MAG_CAL<0:1> |
	// Fully calibrated means at least 0x3F (system calib can vary independently of sensor calib)
	read_data(BNO055_CALIB_STAT_ADDR, &bno055.calib_status, 1);

	return bno055.calib_status;
}

void ICACHE_FLASH_ATTR do_something_with_quat(void) {
	// schmeiﬂ curr_quat.data in bt le device..
}
