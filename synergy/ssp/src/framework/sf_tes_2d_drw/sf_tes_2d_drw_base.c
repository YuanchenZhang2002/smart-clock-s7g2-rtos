/***********************************************************************************************************************
 * Copyright [2015-2025] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_tes_2d_drw_base.c
 * Description  : This file defines the D/AVE D1 low-level driver basic functions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdlib.h>
#include "sf_tes_2d_drw_base.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MSTP_D2W   (1U << 6)

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static d1_device_synergy device_d2d = {0};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup SF_Library
 * @addtogroup SF_TES_2D_DRW 2D Drawing Engine Support Framework
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Creates a device handle to access hardware.
 * This function initializes the D1 device handle, supply module clock to D/AVE 2D hardware and enables the D/AVE 2D
 * interrupt. It is called by the D/AVE 2D driver function d2_inithw() to initialize the D/AVE 2D hardware.
 *
 * @param[in] flags     Reserved. Not used in this function.
 * @retval    Non-NULL  The function returns the pointer to a d1_device object if the D1 device handle was successfully initialized.
 * @retval    NULL      The function returns NULL if failed to create the display list synchronization semaphore.
 **********************************************************************************************************************/
d1_device * d1_opendevice(d1_long_t flags)
{
    d1_device_synergy * handle;
    ssp_err_t err = SSP_SUCCESS;

    SSP_PARAMETER_NOT_USED(flags);

    /** Get new device handle. */
    handle = &device_d2d;

    /** Initialize device data. */
    handle->dlist_indirect = 0;

    /** Access to FMI through the productFeatureGet interface and get the D/AVE 2D hardware information. */
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = 0U;
    ssp_feature.unit    = 0U;
    ssp_feature.id      = SSP_IP_DRW;
    fmi_feature_info_t info = {0U};
    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    if (SSP_SUCCESS != err)
    {
        /** If failed to get the D/AVE 2D information from FMI, set NULL to handle and return. */
        handle = NULL;
        return (d1_device *) handle;
    }

    /** Initialize the D/AVE 2D hardware bass address in the device context. */
    handle->dave_reg = info.ptr;

    /** Supply clock to the D/AVE 2D hardware. */
    R_MSTP->MSTPCRC &= (~MSTP_D2W);

    /** Create the semaphore to notify the completion of display list execution. */
    if(TX_SUCCESS == tx_semaphore_create(&(handle->dlist_semaphore), (CHAR *)"g_d1_isr_semaphore", 0))
    {
        d1_int_t ret;

        /** Enable the D/AVE 2D interrupt if semaphore creation was successful. */
        ret = d1_initirq_intern(handle);
        if(!ret)
        {
            /** If failed to enable the D/AVE 2D interrupt, set NULL to handle. */
            handle = NULL;
        }
    }
    else
    {
        /** If failed to create the semaphore, set NULL to handle. */
        handle = NULL;
    }

    /** Returns the pointer to the d1_device object. */
    return (d1_device *) handle;
}

/*******************************************************************************************************************//**
 * @brief  Close a device handle.
 * It is called by the D/AVE 2D driver function d2_deinithw to de-initialize the D/AVE 2D hardware. Disables the
 * D/AVE 2D interrupt and stop the module clock supply.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @retval    0         The function returns 0 if error occurred, NULL is passed to the argument handle.
 * @retval    1         The function returns 1 if successfully device handle was closed.
 **********************************************************************************************************************/
d1_int_t d1_closedevice(d1_device * handle)
{
    d1_int_t        ret;
    d1_device_synergy  * dev = (d1_device_synergy *) handle;

    if (NULL == handle)
    {
        ret = 0;
    }
    else
    {
        /** Disable the D/AVE 2D interrupt. */
        ret =  d1_shutdownirq_intern(handle);

        /** Stop clock supply to the D/AVE 2D hardware. */
        R_MSTP->MSTPCRC |= MSTP_D2W;

        /** Delete used semaphore */
        /* Return code not checked here as delete call cannot fail when called with created
         * RTOS objects. This object was successfully created in d1_opendevice function. */
        tx_semaphore_delete(&(dev->dlist_semaphore));
    }

    return ret;
}

/*******************************************************************************************************************//**
 * @brief  Write data to the D/AVE 2D hardware register.
 *
 * @param[in] handle    Pointer to a device handle.
 * @param[in] deviceid  D1_DAVE2D(Rendering core) or D1_DLISTINDIRECT(Lists of dlist support). The others are ignored.
 * @param[in] index     Register index (word offset from the D/AVE 2D base address).
 * @param[in] value     32-bit value to write.
 **********************************************************************************************************************/
void d1_setregister(d1_device * handle, d1_int_t deviceid, d1_int_t index, d1_long_t value)
{
    d1_device_synergy * handle_synergy = (d1_device_synergy *) handle;
  
    switch (deviceid)
    {
    case D1_DAVE2D:
#if SF_TES_2D_DRW_USE_DLIST_INDIRECT
        /** Is this access to DLISTSTART and does current driver configuration support lists of display list addresses? */
        if (((uint32_t)(&((uint32_t *)handle_synergy->dave_reg)[index]) == (uint32_t)(&(handle_synergy->dave_reg->DLISTSTART)))
                && (handle_synergy->dlist_indirect))
        {
            /** If yes, 'value' is the start address of pointer array for display lists. Set the first display list
             *  address to the hardware and set the pointer to next display list in the driver context. */
            uint32_t * dlistp = (uint32_t *)value;
            handle_synergy->dave_reg->DLISTSTART = (uint32_t)(*dlistp);
            handle_synergy->dlist_start = dlistp + 1U;
        }
        else
#endif
        {
            /** Write data to specified D/AVE 2D register. */
            ((uint32_t *)handle_synergy->dave_reg)[index] = (uint32_t)value;
        }
        break;

    case D1_DLISTINDIRECT:
#if SF_TES_2D_DRW_USE_DLIST_INDIRECT
        handle_synergy->dlist_indirect = value;
#else
        handle_synergy->dlist_indirect = 0;
#endif
      break;

    default:
        /* Not supported or Unknown device, do nothing. */
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief  Read data from hardware register.
 * Reading a register from an invalid or unsupported device ID will always return 0.
 *
 * @param[in] handle    Pointer to a device handle.
 * @param[in] deviceid  D1_DAVE2D(Rendering core) or D1_DLISTINDIRECT(Lists of dlist support). The others are ignored.
 * @param[in] index     Register index (starts with 0).
 * @retval    Value     The function returns 32-bit value of the register.
 **********************************************************************************************************************/
d1_long_t d1_getregister(d1_device * handle, d1_int_t deviceid, d1_int_t index)
{
    d1_device_synergy * handle_synergy = (d1_device_synergy *) handle;

    int32_t ret;

    /** Check the deviceid to see whether Register addressing or dlist mode is being used. */
    switch (deviceid)
    {
    case D1_DAVE2D:
        /** If Register addressing is used return the value stored in specified D/AVE 2D register. */
        ret = (int32_t)(((uint32_t *)handle_synergy->dave_reg)[index]);
        break;

    case D1_DLISTINDIRECT:
        /** If dlist mode is used return the dlist address. */
        ret = handle_synergy->dlist_indirect;
        break;

    default:
        /** If neither Register addressing nor dlist mode is used return 0. */
        ret = 0;
    }

    return (d1_long_t)ret;
}


/*******************************************************************************************************************//**
 * @brief  Check if the specified device ID is valid for the D/AVE 2D implemented for Synergy.
 * Use this function to verify that a specific hardware interface is available on the current host system.
 *
 * @param[in] handle    Pointer to a device handle.
 * @param[in] deviceid  D1_DAVE2D(Rendering core). The others are ignored.
 * @retval    0         The function returns 0 if specified device ID not supported.
 * @retval    1         The function returns 1 if specified device ID supported.
 **********************************************************************************************************************/
d1_int_t d1_devicesupported(d1_device * handle, d1_int_t deviceid)
{
    d1_int_t ret;

    SSP_PARAMETER_NOT_USED(handle);
  
    /** Check the deviceid. */
    switch (deviceid)
    {
    case D1_DAVE2D:
    /** Return 1 in case of valid deviceid. */
#if SF_TES_2D_DRW_USE_DLIST_INDIRECT
    case D1_DLISTINDIRECT:
#endif
        ret = 1;
        break;
      
    default:
        /** Return 0 in case of Unknown device ID. */
        ret = 0;
    }

    return ret;
}

/*******************************************************************************************************************//**
 * @}
 **********************************************************************************************************************/
