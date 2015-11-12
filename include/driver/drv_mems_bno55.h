/*
 * drv_mem_bno55.h
 *
 * Created: 16.04.2015 15:16:57
 *  Author: Tobias Rohde
 */ 


#ifndef DRV_MEM_BNO55_H_
#define DRV_MEM_BNO55_H_

typedef enum
{
  LOBARO_OK = 0,
  LOBARO_ERR_UNDEFINED,
  LOBARO_ERR_ARGUMENT,
  LOBARO_ERR_SOCKET,
  LOBARO_ERR_IO,
  LOBARO_ERR_TIMEOUT,
  LOBARO_ERR_OUT_OF_MEMORY,
  LOBARO_NOT_FOUND,
  LOBARO_NOT_RDY,
  LOBARO_NO_INIT
}RetVal_t;


#define BNO55_DEVICE_A_ADR (0x28)
#define BNO55_DEVICE_B_ADR (0x29)

extern uint8_t BNO_ADR;

#define QUAT_FREQ_HZ							(20)
#define QUAT_MS									(1000/QUAT_FREQ_HZ)
#define QUAT_BYTE_LENGTH						(8)
#define EULER_BYTE_LENGTH						(6)
#define DUAL_QUAT_JOYSTICK_COMPR_DATA_SIZE		(20)

typedef struct  {
	int8_t init_quat;
	uint8_t selftest_res;
	uint8_t calib_status;
}bno055_t;

typedef struct  {
	int8_t data[QUAT_BYTE_LENGTH];
}quat_t;

typedef struct {
	int8_t data[EULER_BYTE_LENGTH];
}eul_t;

typedef struct
{
	int8_t periphNum; //4B
	//int8_t sensNr1;
	quat_t Data1; //8B
	//int8_t sensNr2;
	quat_t Data2; //8B
	//int8_t sensNr3;
	quat_t Data3; //8B
	int8_t joystick; //4B
} DualQuatJoystick_t;

typedef struct {
	int8_t data[DUAL_QUAT_JOYSTICK_COMPR_DATA_SIZE];	// periphnum + 3* 6byte quat + joystick = 20 byte
} DualQuatJoystickCompr_t;

typedef struct
{
	int8_t periphNum;
	//int8_t sensNr1;
	eul_t Data1;
	//int8_t sensNr2;
	eul_t Data2;
	eul_t Data3;
	int8_t joystick;
}TrippleEulerJoystick_t;

/***************************************************/
/**     	REGISTER ADDRESS DEFINITION            */
/***************************************************/
/* Page id register definition*/
#define BNO055_PAGE_ID_ADDR				    	0x07

/* PAGE0 REGISTER DEFINITION START*/
#define BNO055_CHIP_ID_ADDR                 	0x00
#define BNO055_ACCEL_REV_ID_ADDR				0x01
#define BNO055_MAG_REV_ID_ADDR              	0x02
#define BNO055_GYRO_REV_ID_ADDR             	0x03
#define BNO055_SW_REV_ID_LSB_ADDR				0x04
#define BNO055_SW_REV_ID_MSB_ADDR				0x05
#define BNO055_BL_REV_ID_ADDR					0x06

/* Accel data register*/
#define BNO055_ACCEL_DATA_X_LSB_ADDR			0x08
#define BNO055_ACCEL_DATA_X_MSB_ADDR			0x09
#define BNO055_ACCEL_DATA_Y_LSB_ADDR			0x0A
#define BNO055_ACCEL_DATA_Y_MSB_ADDR			0x0B
#define BNO055_ACCEL_DATA_Z_LSB_ADDR			0x0C
#define BNO055_ACCEL_DATA_Z_MSB_ADDR			0x0D

/*Mag data register*/
#define BNO055_MAG_DATA_X_LSB_ADDR				0x0E
#define BNO055_MAG_DATA_X_MSB_ADDR				0x0F
#define BNO055_MAG_DATA_Y_LSB_ADDR				0x10
#define BNO055_MAG_DATA_Y_MSB_ADDR				0x11
#define BNO055_MAG_DATA_Z_LSB_ADDR				0x12
#define BNO055_MAG_DATA_Z_MSB_ADDR				0x13

/*Gyro data registers*/
#define BNO055_GYRO_DATA_X_LSB_ADDR				0x14
#define BNO055_GYRO_DATA_X_MSB_ADDR				0x15
#define BNO055_GYRO_DATA_Y_LSB_ADDR				0x16
#define BNO055_GYRO_DATA_Y_MSB_ADDR				0x17
#define BNO055_GYRO_DATA_Z_LSB_ADDR				0x18
#define BNO055_GYRO_DATA_Z_MSB_ADDR				0x19

/*Euler data registers*/
#define BNO055_EULER_H_LSB_ADDR					0x1A
#define BNO055_EULER_H_MSB_ADDR					0x1B

#define BNO055_EULER_R_LSB_ADDR					0x1C
#define BNO055_EULER_R_MSB_ADDR					0x1D

#define BNO055_EULER_P_LSB_ADDR					0x1E
#define BNO055_EULER_P_MSB_ADDR					0x1F

/*Quaternion data registers*/
#define BNO055_QUATERNION_DATA_W_LSB_ADDR		0x20
#define BNO055_QUATERNION_DATA_W_MSB_ADDR		0x21
#define BNO055_QUATERNION_DATA_X_LSB_ADDR		0x22
#define BNO055_QUATERNION_DATA_X_MSB_ADDR		0x23
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR		0x24
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR		0x25
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR		0x26
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR		0x27

/* Linear acceleration data registers*/
#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR		0x28
#define BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR		0x29
#define BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR		0x2A
#define BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR		0x2B
#define BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR		0x2C
#define BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR		0x2D

/*Gravity data registers*/
#define BNO055_GRAVITY_DATA_X_LSB_ADDR			0x2E
#define BNO055_GRAVITY_DATA_X_MSB_ADDR			0x2F
#define BNO055_GRAVITY_DATA_Y_LSB_ADDR			0x30
#define BNO055_GRAVITY_DATA_Y_MSB_ADDR			0x31
#define BNO055_GRAVITY_DATA_Z_LSB_ADDR			0x32
#define BNO055_GRAVITY_DATA_Z_MSB_ADDR			0x33

/* Temperature data register*/
#define BNO055_TEMP_ADDR						0x34

/* Status registers*/
#define BNO055_CALIB_STAT_ADDR					0x35
#define BNO055_SELFTEST_RESULT_ADDR				0x36
#define BNO055_INTR_STAT_ADDR					0x37
#define BNO055_SYS_CLK_STAT_ADDR				0x38
#define BNO055_SYS_STAT_ADDR					0x39
#define BNO055_SYS_ERR_ADDR						0x3A

/* Unit selection register*/
#define BNO055_UNIT_SEL_ADDR					0x3B
#define BNO055_DATA_SELECT_ADDR					0x3C

/* Mode registers*/
#define BNO055_OPR_MODE_ADDR					0x3D
#define BNO055_PWR_MODE_ADDR					0x3E

#define BNO055_SYS_TRIGGER_ADDR					0x3F
#define BNO055_TEMP_SOURCE_ADDR					0x40
/* Axis remap registers*/
#define BNO055_AXIS_MAP_CONFIG_ADDR				0x41
#define BNO055_AXIS_MAP_SIGN_ADDR				0x42

/* SIC registers*/
#define BNO055_SIC_MATRIX_0_LSB_ADDR			0x43
#define BNO055_SIC_MATRIX_0_MSB_ADDR			0x44
#define BNO055_SIC_MATRIX_1_LSB_ADDR			0x45
#define BNO055_SIC_MATRIX_1_MSB_ADDR			0x46
#define BNO055_SIC_MATRIX_2_LSB_ADDR			0x47
#define BNO055_SIC_MATRIX_2_MSB_ADDR			0x48
#define BNO055_SIC_MATRIX_3_LSB_ADDR			0x49
#define BNO055_SIC_MATRIX_3_MSB_ADDR			0x4A
#define BNO055_SIC_MATRIX_4_LSB_ADDR			0x4B
#define BNO055_SIC_MATRIX_4_MSB_ADDR			0x4C
#define BNO055_SIC_MATRIX_5_LSB_ADDR			0x4D
#define BNO055_SIC_MATRIX_5_MSB_ADDR			0x4E
#define BNO055_SIC_MATRIX_6_LSB_ADDR			0x4F
#define BNO055_SIC_MATRIX_6_MSB_ADDR			0x50
#define BNO055_SIC_MATRIX_7_LSB_ADDR			0x51
#define BNO055_SIC_MATRIX_7_MSB_ADDR			0x52
#define BNO055_SIC_MATRIX_8_LSB_ADDR			0x53
#define BNO055_SIC_MATRIX_8_MSB_ADDR			0x54

/* Accelerometer Offset registers*/
#define ACCEL_OFFSET_X_LSB_ADDR					0x55
#define ACCEL_OFFSET_X_MSB_ADDR					0x56
#define ACCEL_OFFSET_Y_LSB_ADDR					0x57
#define ACCEL_OFFSET_Y_MSB_ADDR					0x58
#define ACCEL_OFFSET_Z_LSB_ADDR					0x59
#define ACCEL_OFFSET_Z_MSB_ADDR					0x5A

/* Magnetometer Offset registers*/
#define MAG_OFFSET_X_LSB_ADDR					0x5B
#define MAG_OFFSET_X_MSB_ADDR					0x5C
#define MAG_OFFSET_Y_LSB_ADDR					0x5D
#define MAG_OFFSET_Y_MSB_ADDR					0x5E
#define MAG_OFFSET_Z_LSB_ADDR					0x5F
#define MAG_OFFSET_Z_MSB_ADDR					0x60

/* Gyroscope Offset registers*/
#define GYRO_OFFSET_X_LSB_ADDR					0x61
#define GYRO_OFFSET_X_MSB_ADDR					0x62
#define GYRO_OFFSET_Y_LSB_ADDR					0x63
#define GYRO_OFFSET_Y_MSB_ADDR					0x64
#define GYRO_OFFSET_Z_LSB_ADDR					0x65
#define GYRO_OFFSET_Z_MSB_ADDR					0x66

/* Radius registers*/
#define	ACCEL_RADIUS_LSB_ADDR					0x67
#define	ACCEL_RADIUS_MSB_ADDR					0x68
#define	MAG_RADIUS_LSB_ADDR						0x69
#define	MAG_RADIUS_MSB_ADDR						0x6A
/* PAGE0 REGISTERS DEFINITION END*/

/* PAGE1 REGISTERS DEFINITION START*/
/* Configuration registers*/
#define ACCEL_CONFIG_ADDR						0x08
#define MAG_CONFIG_ADDR							0x09
#define GYRO_CONFIG_ADDR						0x0A
#define GYRO_MODE_CONFIG_ADDR					0x0B
#define ACCEL_SLEEP_CONFIG_ADDR					0x0C
#define GYRO_SLEEP_CONFIG_ADDR					0x0D
#define MAG_SLEEP_CONFIG_ADDR					0x0E

/* Interrupt registers*/
#define INT_MASK_ADDR							0x0F
#define INT_ADDR								0x10
#define ACCEL_ANY_MOTION_THRES_ADDR				0x11
#define ACCEL_INTR_SETTINGS_ADDR				0x12
#define ACCEL_HIGH_G_DURN_ADDR					0x13
#define ACCEL_HIGH_G_THRES_ADDR					0x14
#define ACCEL_NO_MOTION_THRES_ADDR				0x15
#define ACCEL_NO_MOTION_SET_ADDR				0x16
#define GYRO_INTR_SETING_ADDR					0x17
#define GYRO_HIGHRATE_X_SET_ADDR				0x18
#define GYRO_DURN_X_ADDR						0x19
#define GYRO_HIGHRATE_Y_SET_ADDR				0x1A
#define GYRO_DURN_Y_ADDR						0x1B
#define GYRO_HIGHRATE_Z_SET_ADDR				0x1C
#define GYRO_DURN_Z_ADDR						0x1D
#define GYRO_ANY_MOTION_THRES_ADDR				0x1E
#define GYRO_ANY_MOTION_SET_ADDR				0x1F
/* PAGE1 REGISTERS DEFINITION END*/


/* Page ID */
#define PAGE_ZERO								0x00
#define PAGE_ONE								0x01

/*Accel unit*/
#define ACCEL_UNIT_MSQ							0x00
#define ACCEL_UNIT_MG							0x01

/*Gyro unit*/
#define GYRO_UNIT_DPS							0x00
#define GYRO_UNIT_RPS							0x01

/* Euler unit*/
#define EULER_UNIT_DEG							0x00
#define EULER_UNIT_RAD							0x01

/*Temperature unit*/
#define TEMP_UNIT_CELSIUS						0x00
#define TEMP_UNIT_FAHRENHEIT					0x01

/*Accel division factor*/
#define	ACCEL_DIV_MSQ							(100.0f)
#define	ACCEL_DIV_MG							(1)

/*Mag division factor*/
#define MAG_DIV_UT								(16.0f)

/*Gyro division factor*/
#define GYRO_DIV_DPS							(16.0f)
#define GYRO_DIV_RPS							(900.0f)

/*Euler division factor*/
#define EULER_DIV_DEG							(16.0f)
#define EULER_DIV_RAD							(900.0f)

/*Linear accel division factor*/
#define	LINEAR_ACCEL_DIV_MSQ					(100.0f)

/*Gravity accel division factor*/
#define	GRAVITY_DIV_MSQ							(100.0f)

/* Temperature division factor*/
#define TEMP_DIV_FAHRENHEIT						(0.5f)
#define TEMP_DIV_CELSIUS						(1)

/* Operation mode settings*/
#define OPERATION_MODE_CONFIG					0x00
#define OPERATION_MODE_ACCONLY					0x01
#define OPERATION_MODE_MAGONLY					0x02
#define OPERATION_MODE_GYRONLY					0x03
#define OPERATION_MODE_ACCMAG					0x04
#define OPERATION_MODE_ACCGYRO					0x05
#define OPERATION_MODE_MAGGYRO					0x06
#define OPERATION_MODE_AMG						0x07
#define OPERATION_MODE_IMUPLUS					0x08
#define OPERATION_MODE_COMPASS					0x09
#define OPERATION_MODE_M4G						0x0A
#define OPERATION_MODE_NDOF_FMC_OFF				0x0B
#define OPERATION_MODE_NDOF						0x0C

/* Power mode*/
#define POWER_MODE_NORMAL						0x00
#define POWER_MODE_LOWPOWER						0x01
#define POWER_MODE_SUSPEND						0x02

/* PAGE-1 definitions*/
/* Accel Range */

#define ACCEL_RANGE_2G							0x00
#define ACCEL_RANGE_4G							0x01
#define ACCEL_RANGE_8G							0x02
#define ACCEL_RANGE_16G							0x03

/* Accel Bandwidth*/
#define ACCEL_BW_7_81HZ							0x00
#define ACCEL_BW_15_63HZ						0x01
#define ACCEL_BW_31_25HZ						0x02
#define ACCEL_BW_62_5HZ							0x03
#define ACCEL_BW_125HZ							0x04
#define ACCEL_BW_250HZ							0x05
#define ACCEL_BW_500HZ							0x06
#define ACCEL_BW_1000HZ							0x07

/* Accel Power mode*/
#define ACCEL_NORMAL							0x00
#define ACCEL_SUSPEND							0x01
#define ACCEL_LOWPOWER_1						0x02
#define ACCEL_STANDBY							0x03
#define ACCEL_LOWPOWER_2						0x04
#define ACCEL_DEEPSUSPEND						0x05

/* Mag data output rate*/
#define MAG_DATA_OUTRATE_2HZ					0x00
#define MAG_DATA_OUTRATE_6HZ					0x01
#define MAG_DATA_OUTRATE_8HZ					0x02
#define MAG_DATA_OUTRATE_10HZ					0x03
#define MAG_DATA_OUTRATE_15HZ					0x04
#define MAG_DATA_OUTRATE_20HZ					0x05
#define MAG_DATA_OUTRATE_25HZ					0x06
#define MAG_DATA_OUTRATE_30HZ					0x07

/* Mag Operation mode*/
#define MAG_OPERATION_MODE_LOWPOWER				0x00
#define MAG_OPERATION_MODE_REGULAR				0x01
#define MAG_OPERATION_MODE_ENHANCED_REGULAR		0x02
#define MAG_OPERATION_MODE_HIGH_ACCURACY		0x03

/* Mag power mode*/
#define MAG_POWER_MODE_NORMAL					0x00
#define MAG_POWER_MODE_SLEEP					0x01
#define MAG_POWER_MODE_SUSPEND					0x02
#define MAG_POWER_MODE_FORCE_MODE				0x03

/* Gyro range*/
#define GYRO_RANGE_2000DPS						0x00
#define GYRO_RANGE_1000DPS						0x01
#define GYRO_RANGE_500DPS						0x02
#define GYRO_RANGE_250DPS						0x03
#define GYRO_RANGE_125DPS						0x04

/* Gyro Bandwidth*/
#define GYRO_BW_523HZ							0x00
#define GYRO_BW_230HZ							0x01
#define GYRO_BW_116HZ							0x02
#define GYRO_BW_47HZ							0x03
#define GYRO_BW_23HZ							0x04
#define GYRO_BW_12HZ							0x05
#define GYRO_BW_64HZ							0x06
#define GYRO_BW_32HZ							0x07

/* Gyro power mode*/
#define GYRO_POWER_MODE_NORMAL					0x00
#define GYRO_POWER_MODE_FASTPOWERUP				0x01
#define GYRO_POWER_MODE_DEEPSUSPEND				0x02
#define GYRO_POWER_MODE_SUSPEND					0x03
#define GYRO_POWER_MODE_ADVANCE_POWERSAVE		0x04

/***************************************************/
/**     	         PROTOTYPES 		           */
/***************************************************/
int8_t bno055_get_quat_init(void);

void bno055_init_quat_mode(void);
RetVal_t bno055_read_quat(quat_t* pQuat) ;
uint8_t bno055_read_calib(void);

void do_something_with_quat(void);
RetVal_t bno055_disp_quat(quat_t* pQuat);
uint8_t bno055_get_chipID();

void bno055_init_amg_mode(void);
RetVal_t bno055_read_amg();


uint8_t softi2c_bno055_get_chipID();
void softi2c_bno055_init_quat_mode(void);
RetVal_t softi2c_bno055_read_quat(quat_t* pQuat);


RetVal_t bno055_read_euler(eul_t* pEul);
RetVal_t softi2c_bno055_read_euler(eul_t* pEul);

#endif /* DRV_MEM_BNO55_H_ */
