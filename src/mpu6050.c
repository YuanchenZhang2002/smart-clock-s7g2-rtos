/******************** (C) COPYRIGHT 2018 University of South Wales *******************************
* File Name          : mpu6050.c
* Author             : SS
* Version            : V1.1.2
* SSP version        : 1.5.0
* Toolchain          : GCC ARM Embedded v4.9.3.20150529
* Date               : 21-March-2018
* Description        : Low level Driver for MPU9050 6DoF IMU for RTOS Application for Renesas Synergy Platform.
*                      this Example is based on the Renesas Synergy SK-S7G2
***************************************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING STUDENTS
* WITH CODING INFORMATION REGARDING THEIR ASSIGNMENT IN ORDER FOR THEM DELIVER TO THE STANDARD.
* AS A RESULT, UNIVERSITY OF SOUTH WALES SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY STUDENTS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS,PROJECT OR ASSIGNMENTS.
****************************************************************************************************/



    /**************************************************************************
     * MPU9050 I2C interface and the communication protocol
     * refer to the datasheet for further infomation. the
     *
     * P7_06    -   SCL2 (J23-17)
     * P7_07    -   SDA2 (J23-19)
     *
     * Note : Both Jumpers on J9 must be removed
     *
     * S    - Start Condition
     * AD   - Slave Address (7 bit)
     * W    - Write Bit (0)
     * R    - Read Bit (1)
     * ACK  - Acknowledge
     * NACK - Not-Acknowledge
     * RA   - Internal Register Address
     * DATA - Transmit/Receive Data
     * P    - Stop Condition
     *
     *      ****  Read Sequence  ****
     *
     * Single-Byte Read Sequence
     * Master   -   A AD+W     RA     S AD+R          NACK P
     * Slave    -          ACK    ACK        ACK DATA
     *
     * Burst Read Sequence
     * Master   -   S AD+W     RA     S AD+R          ACK      NACK P
     * Slave    -          ACK    ACK        ACK DATA     DATA
     *
     *      **** Write Sequence ****
     *
     * Single-Byte Write Sequence
     * Master   -   A AD+W     RA     DATA     P
     * Slave    -          ACK    ACK      ACK
     *
     * Burst Write Sequence
     * Master   -   S AD+W     RA     DATA     DATA     P
     * Slave    -          ACK    ACK      ACK      ACK
     *
     ********************************************************************************* */

#include "hal_data.h"
#include "mpu6050.h"

#define STP_COND_ISS     false      // Issue Stop Condition
#define STP_COND_NISS    true       // Not Issue Stop condition

mpu6050_flags_t event_flag;
mpu6050_data_typeDef mpu6050_data;
volatile uint8_t temp_data[2];


uint8_t mpu6050_Init(void)
{
    volatile uint8_t rtn = 0;
    uint8_t tx_data[5]= {0};

    uint8_t rx_data[5] = {0};

    /* Timer Setup */
   // g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
   // g_timer0.p_api->start(g_timer0.p_ctrl);

    /* External Interrupts */
//    rtn = g_external_irq3.p_api->open(g_external_irq3.p_ctrl,g_external_irq3.p_cfg);

    /* open I2C */
    g_i2c3.p_api->reset(g_i2c3.p_ctrl);

    rtn =  g_i2c3.p_api->open(g_i2c3.p_ctrl,g_i2c3.p_cfg);



    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);  // delay 10 ms for I2C bus setup

    tx_data[0] = MPU6050_REG_WHO_AM_I;          // who am I register
    rtn = g_i2c3.p_api->write (g_i2c3.p_ctrl,tx_data,1,STP_COND_NISS);        /*  Master  - S AD+W     RA             */
                                                                        /*  Slave   -        ACK    ACK         */
    rtn = 0xFF;

    rtn = g_i2c3.p_api->read(g_i2c3.p_ctrl,rx_data,1,STP_COND_ISS);           /* Master   - S AD+R          NACK P    */
                                                                        /* Slave   -         ACK DATA           */
    if (rtn != SSP_SUCCESS)
    {
        while(1);
    }

    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    if(rx_data[0] == 0x68)
    {

        if (mpu6050_single_byte_write(0x01, MPU6050_REG_PWR_MGMT_1)!=mpu6050_OK)       // Arming the Device
        {
            while(1);
        }

        if (mpu6050_single_byte_write(0x00, MPU6050_REG_PWR_MGMT_2)!=mpu6050_OK)       // Arming the Device
        {
            while(1);
        }

        if (mpu6050_single_byte_write(0x01, MPU6050_REG_CONFIG)!=mpu6050_OK)       // fs 1kHz , 184 BW
        {
            while(1);
        }

        R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave


        if (mpu6050_single_byte_write(0x18, MPU6050_REG_ACCEL_CONFIG)!=mpu6050_OK)      // accelerometer configuration 16g sensitivity
        {
            while(1);
        }
        R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave


        if (mpu6050_single_byte_write(0x1B, MPU6050_REG_GYRO_CONFIG)!=mpu6050_OK)       // gyroscope configuration 2000dps
        {
            while(1);
        }
        R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave


    }
    else
    {
        while(1); // MPU6050 communication failed
    }


    return (rtn);

}




void mpu6050_Process(void)
{
    volatile int16_t tempe;


      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_XOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_XOUT_H);

      mpu6050_data.acc.x = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_YOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_YOUT_H);

      mpu6050_data.acc.y = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave


      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_ZOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_ACCEL_ZOUT_H);

      mpu6050_data.acc.z = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

         /* Read Gyroscope readings */
      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_GYRO_XOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_GYRO_XOUT_H);

      mpu6050_data.gyro.x = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_GYRO_YOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_GYRO_YOUT_H);

      mpu6050_data.gyro.y = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_GYRO_ZOUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_GYRO_ZOUT_H);

      mpu6050_data.gyro.z = (int16_t)(temp_data[1]<<8|temp_data[0]);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

      temp_data[0] = mpu6050_single_byte_read(MPU6050_REG_TEMP_OUT_L);
      temp_data[1] = mpu6050_single_byte_read(MPU6050_REG_TEMP_OUT_H);

      tempe = (int16_t)(temp_data[1]<<8|temp_data[0]);
      mpu6050_data.temperature = (int16_t)((float)(tempe)/340+36.53);

      R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);


}







/*
 * @brief   MPU 6050 single byte Write sequence
 * @param   data:   Data
 *          ra  :   MPU-60X0 internal Register address
 * @retval  mpu6050_err_t
 *                mpu6050_OK  = 0
 *                mpu6050_wr_fail
 *                mpu6050_rd_fail
 *                mpu6050_timeout
 */


mpu6050_err_t mpu6050_single_byte_write(uint8_t data, uint8_t ra)
{
    mpu6050_err_t err;
    uint8_t tmp_w[5];
    uint8_t tmp_r[5];

    tmp_w[0]= ra;
    tmp_w[1]= data;

    err = g_i2c3.p_api->write (g_i2c3.p_ctrl,tmp_w,2,STP_COND_ISS);           /* Master   -   A AD+W     RA     DATA     P   */
                                                                              /* Slave    -          ACK    ACK      ACK     */

    R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS);                   // delay 2 micro second for Slave

    tmp_w[0] = ra;
    g_i2c3.p_api->write (g_i2c3.p_ctrl,tmp_w,1,STP_COND_NISS);                /*  Master  - S AD+W     RA             */
                                                                              /*  Slave   -        ACK    ACK         */


    g_i2c3.p_api->read(g_i2c3.p_ctrl,tmp_r,1,STP_COND_ISS);                   /* Master   - S AD+R          NACK P    */
                                                                              /* Slave   -         ACK DATA           */

    if(tmp_r[0]==data)err = mpu6050_OK;
    else err = mpu6050_wr_fail;

    return(err) ;
}




/*
 * @brief   MPU 6050 single byte read sequence
 * @param   rdata:   read Data
 *          ra  :   MPU-60X0 internal Register address
 *          size: number of bytes
 * @retval  mpu6050_err_t
 *                mpu6050_OK  = 0
 *                mpu6050_wr_fail
 *                mpu6050_rd_fail
 *                mpu6050_timeout
 */

uint8_t mpu6050_single_byte_read(uint8_t ra)
{
//    mpu6050_err_t err;
    uint8_t tmp_w[2];
    uint8_t tmp_r[2];


    tmp_w[0] = ra;
    g_i2c3.p_api->write (g_i2c3.p_ctrl,tmp_w,1,STP_COND_NISS);                /*  Master  - S AD+W     RA             */
                                                                              /*  Slave   -        ACK    ACK         */

    g_i2c3.p_api->read(g_i2c3.p_ctrl,tmp_r,1,STP_COND_ISS);                   /* Master   - S AD+R          NACK P    */
                                                                              /* Slave   -         ACK DATA           */

    return(tmp_r[0]) ;
}




/*
 *  @brief   MPU 6050 external Interrupt handler
 *
*/

//void mpu6050_int_cb(external_irq_callback_args_t * p_args)
//{
//    switch (p_args->channel)
//    {
//
//    case 0:
//        break;
//
//    case 1:
//        break;
//
//    case 2:
//        break;
//
//    case 3:
//
//        break;
//
//    default:
//        break;
//
//    }
//}











