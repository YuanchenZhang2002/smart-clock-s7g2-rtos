/******************** (C) COPYRIGHT 2018 University of South Wales *******************************
* File Name          : mpu6050.h
* Author             : SS
* Version            : V1.2.0
* SSP version        : 1.5.0
* Toolchain          : GCC ARM Embedded v4.9.3.20150529
* Date               : 21-March-2018
* Description        : Low level defines for MPU9050 6DoF IMU
***************************************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING STUDENTS
* WITH CODING INFORMATION REGARDING THEIR ASSIGNMENT IN ORDER FOR THEM DELIVER TO THE STANDARD.
* AS A RESULT, UNIVERSITY OF SOUTH WALES SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY STUDENTS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS,PROJECT OR ASSIGNMENTS.
****************************************************************************************************/

#ifndef MPU6050_H_
#define MPU6050_H_


/**************************************************
 * register defines
 * read     1
 * write    0
 * address  110100X RW
 * ************************************************/
#define mpu6050_add7bit 0x69
#define mpu6050_addR    0xD1
#define mpu6050_addW    0xD0


#define MPU6050_REG_SMPRT_DIV         (0x19)
#define MPU6050_REG_CONFIG            (0x1A)
#define MPU6050_REG_GYRO_CONFIG       (0x1B) // Gyroscope Configuration
#define MPU6050_REG_ACCEL_CONFIG      (0x1C) // Accelerometer Configuration
#define MPU6050_REG_FIFO_EN           (0x23)    // fifo enable
#define MPU6050_REG_I2C_MST_CTRL      (0x24)
#define MPU6050_REG_INT_PIN_CFG       (0x37) // INT Pin. Bypass Enable Configuration
#define MPU6050_REG_INT_ENABLE        (0x38) // INT Enable
#define MPU6050_REG_INT_STATUS        (0x3A)
#define MPU6050_REG_ACCEL_XOUT_H      (0x3B)
#define MPU6050_REG_ACCEL_XOUT_L      (0x3C)
#define MPU6050_REG_ACCEL_YOUT_H      (0x3D)
#define MPU6050_REG_ACCEL_YOUT_L      (0x3E)
#define MPU6050_REG_ACCEL_ZOUT_H      (0x3F)
#define MPU6050_REG_ACCEL_ZOUT_L      (0x40)
#define MPU6050_REG_TEMP_OUT_H        (0x41)
#define MPU6050_REG_TEMP_OUT_L        (0x42)
#define MPU6050_REG_GYRO_XOUT_H       (0x43)
#define MPU6050_REG_GYRO_XOUT_L       (0x44)
#define MPU6050_REG_GYRO_YOUT_H       (0x45)
#define MPU6050_REG_GYRO_YOUT_L       (0x46)
#define MPU6050_REG_GYRO_ZOUT_H       (0x47)
#define MPU6050_REG_GYRO_ZOUT_L       (0x48)
#define MPU6050_REG_USER_CTRL         (0x6A) // User Control
#define MPU6050_REG_PWR_MGMT_1         0x6B // Power Management 1
#define MPU6050_REG_PWR_MGMT_2        (0x6C) // Power Management 2
#define MPU6050_REG_WHO_AM_I          (0x75) // Who Am I


typedef enum{
    mpu6050_OK  = 0,
    mpu6050_wr_fail,
    mpu6050_rd_fail,
    mpu6050_timeout
}mpu6050_err_t;

typedef enum{

    mpu6050_IIC_default = 0,
    mpu6050_IIC_tx_compleet,
    mpu6050_IIC_rx_compleet

}mpu6050_flags_t;




typedef struct {
    int16_t     x;
    int16_t     y;
    int16_t     z;
}mpu6050_accel_typeDef;


typedef struct {
    int16_t     x;
    int16_t     y;
    int16_t     z;
}mpu6050_gyro_typeDef;




typedef struct{
    mpu6050_accel_typeDef acc;
    mpu6050_gyro_typeDef  gyro;
    int16_t temperature;
}mpu6050_data_typeDef;

/***********************************************************
 * function Prototype
 *
 * */

uint8_t mpu6050_Init(void);
mpu6050_err_t mpu6050_single_byte_write(uint8_t data, uint8_t ra);
uint8_t mpu6050_single_byte_read(uint8_t ra);
void mpu6050_Process(void);

extern mpu6050_data_typeDef mpu6050_data;
#endif /* MPU6050_H_ */
