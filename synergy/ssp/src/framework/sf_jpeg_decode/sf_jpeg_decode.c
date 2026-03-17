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
 * File Name    : sf_jpeg_decode.c
 * Description  : ThreadX aware general JPEG Codec driver functions.
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "sf_jpeg_decode.h"
#include "r_jpeg_decode.h"
#include "sf_jpeg_decode_cfg.h"
#include "r_jpeg_decode_api.h"
#include "sf_jpeg_decode_private_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup     SF_JPEG_DECODE
 * @{
 **********************************************************************************************************************/

/** "JPEG" in ASCII, used to identify general JPEG control block */
#define SF_JPEG_DECODE_OPEN (0x4A504547U)

/** Macro for error logger. */
#ifndef SF_JPEG_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_JPEG_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &s_sf_jpeg_version)
#endif

#define JPEG_ALL_EVENTS ((ULONG) 0xFFFFFFFF)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
ssp_err_t sf_jpeg_initialize(sf_jpeg_decode_instance_ctrl_t* const p_ctrl,
                                      sf_jpeg_decode_cfg_t const * const  p_cfg);
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/


/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "sf_jpeg";
#endif

#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t s_sf_jpeg_version =
{
    .api_version_minor  = SF_JPEG_DECODE_API_VERSION_MINOR,
    .api_version_major  = SF_JPEG_DECODE_API_VERSION_MAJOR,
    .code_version_major = SF_JPEG_DECODE_CODE_VERSION_MAJOR,
    .code_version_minor = SF_JPEG_DECODE_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

static TX_EVENT_FLAGS_GROUP * p_event_flags = TX_NULL;
/***********************************************************************************************************************
* Implementation of Interface
- **********************************************************************************************************************/
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const sf_jpeg_decode_api_t g_sf_jpeg_decode_on_sf_jpeg_decode =
{
    .open                   = SF_JPEG_Decode_Open,
    .inputBufferSet         = SF_JPEG_Decode_InputBufferSet,
    .outputBufferSet        = SF_JPEG_Decode_OutputBufferSet,
    .linesDecodedGet        = SF_JPEG_Decode_LinesDecodedGet,
    .horizontalStrideSet    = SF_JPEG_Decode_HorizontalStrideSet,
    .imageSubsampleSet      = SF_JPEG_Decode_ImageSubsampleSet,
    .wait                   = SF_JPEG_Decode_Wait,
    .statusGet              = SF_JPEG_Decode_StatusGet,
    .imageSizeGet           = SF_JPEG_Decode_ImageSizeGet,
    .pixelFormatGet         = SF_JPEG_Decode_PixelFormatGet,
    .close                  = SF_JPEG_Decode_Close,
    .versionGet             = SF_JPEG_Decode_VersionGet
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 * @brief       JPEG decode SSP framework level callback
 * @param[in]   p_args                             Pointer to callback parameters
 **********************************************************************************************************************/
static void sf_jpeg_callback_function (jpeg_decode_callback_args_t *p_args)
{
    if (p_event_flags)
    {
        /** Update the event flag with the JPEG DECODE status. */
        /* The return code is not checked here because flag set cannot fail when called with created RTOS objects. These
         * objects were successfully created in open call */
        tx_event_flags_set(p_event_flags, (ULONG) p_args->status, TX_OR);
    }
}

/******************************************************************************************************************//**
 * @brief  Acquires mutex, then handles driver initialization at the HAL layer. This function releases the mutex
 * before returns to the caller.
 * @param[in,out]   p_ctrl              Control handle for JPEG framework context for a device.
 * @param[in]       p_cfg               Pointer to JPEG framework Configuration Structure.
 * @retval          SSP_SUCCESS         JPEG Decode driver is successfully opened.
 * @retval          SSP_ERR_INTERNAL    An internal ThreadX error has occurred.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::open
 *********************************************************************************************************************/
ssp_err_t sf_jpeg_initialize(sf_jpeg_decode_instance_ctrl_t* const p_ctrl,
                                      sf_jpeg_decode_cfg_t const * const  p_cfg)
{
    UINT tx_err = TX_SUCCESS;
    ssp_err_t err = SSP_SUCCESS;
    jpeg_decode_cfg_t hal_cfg;

    /** Create event flags for use with wait function */
    tx_err = tx_event_flags_create(&p_ctrl->events, (CHAR *) "jpeg_events");
    if(TX_SUCCESS != tx_err)
    {
        return SSP_ERR_INTERNAL;
    }

    /** Create the mutex for this instance, this mutex is used to protect access to lower level hardware */
    tx_err = tx_mutex_create(&p_ctrl->mutex, NULL, TX_INHERIT);
    if(TX_SUCCESS != tx_err)
    {
        /* Delete the RTOS services allocated before returning error, Return codes are not checked in error condition
         * processing because these objects were created during this function, hence this call will not fail. */
        tx_event_flags_delete(&p_ctrl->events);
        return SSP_ERR_INTERNAL;
    }

    /** Duplicate the content of p_cfg. */
    memcpy(&hal_cfg, p_cfg->p_lower_lvl_jpeg_decode->p_cfg, sizeof(jpeg_decode_cfg_t));
    /** Install framework callback and context. */
    hal_cfg.p_callback = sf_jpeg_callback_function;
    hal_cfg.p_context  = p_ctrl;

    /** Call the low level driver to configure the JPEG device. */
    err = p_cfg->p_lower_lvl_jpeg_decode->p_api->open(p_cfg->p_lower_lvl_jpeg_decode->p_ctrl, &hal_cfg);

    /** If the operation failed, delete the resources. */
    if(SSP_SUCCESS != err)
    {
        /* Delete the RTOS services allocated before returning error, Return codes are not checked in error condition
         * processing because these objects were created during this function, hence this call will not fail */
        tx_event_flags_delete(&p_ctrl->events);
        tx_mutex_delete(&p_ctrl->mutex);
    }

    return err;
}

/**********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 * @brief  Parameter checking and initialize JPEG decode with sf_jpeg_initialize helper function and marking the
 *         open flag in control block.
 * @retval SSP_SUCCESS           JPEG Decode framework is successfully opened.
 * @retval SSP_ERR_ASSERTION     One of the following parameters may be null: p_ctrl or p_cfg.
 * @retval SSP_ERR_ALREADY_OPEN  JPEG Decode framework is already open.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_Open       (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                      sf_jpeg_decode_cfg_t const * const  p_cfg)
{
    ssp_err_t err;
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_cfg);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN != p_ctrl->open, SSP_ERR_ALREADY_OPEN);
#endif

    /** Initialize the JPEG framework */
    err = sf_jpeg_initialize(p_ctrl, p_cfg);

    /* Validate return value from the open call. */
    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Save driver structure pointer for use in other framework layer functions. */
    p_ctrl->p_lower_lvl_jpeg_decode = p_cfg->p_lower_lvl_jpeg_decode;
    p_event_flags = &p_ctrl->events;

    /** Mark control block open so subsequent calls know the device is open. */
    p_ctrl->open = SF_JPEG_DECODE_OPEN;

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Configures JPEG coded input data.
 *
 * This API configures the decode input buffer address register.   After the input buffer address is set,
 * the driver checks whether the output buffer address is set, and verifies that the output buffer size is
 * large enough to hold at least eight output lines of data.  If both the input buffer and output buffer
 * are set properly, the driver automatically starts the decode process.
 *
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           Decode input buffer is successfully configured.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::inputBufferSet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_InputBufferSet       (sf_jpeg_decode_ctrl_t      * const p_api_ctrl,
                                                void                        * const p_buffer,
                                                uint32_t const                      buffer_size)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    /** Call the HAL driver layer inputBufferSet routine. */
    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->inputBufferSet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_buffer, buffer_size);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Obtain number of lines decoded by the codec.
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           Lines decoded value is successfully obtained.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::linesDecodedGet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_LinesDecodedGet   (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                             uint32_t                     * const p_lines)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->linesDecodedGet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_lines);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Configure the horizontal stride value.
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           Horizontal Stride value is successfully configured.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::horizontalStrideSet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_HorizontalStrideSet   (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                                uint32_t                            horizontal_stride)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->horizontalStrideSet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, horizontal_stride);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Configure the horizontal and vertical subsample values. This allows an application
 *         to reduce the size of the decoded image at runtime.
 *
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           Image subsample values are successfully configured.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::imageSubsampleSet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_ImageSubsampleSet    (sf_jpeg_decode_ctrl_t        * const p_api_ctrl,
                                               jpeg_decode_subsample_t              horizontal_subsample,
                                               jpeg_decode_subsample_t              vertical_subsample)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->imageSubsampleSet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl,
            horizontal_subsample, vertical_subsample);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Configure the decode output buffer.
 *
 * This API configures the decode output buffer address register.   After the output buffer address is set,
 * the driver computers the number of output lines the buffer is able to hold.  The hardware requires the
 * number out output lines to decode at a time is multiple of eight.  If both the input buffer and output buffer
 * are set properly, the driver automatically starts the decode process.
 *
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           Output buffer is successfully configured.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::outputBufferSet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_OutputBufferSet       (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                                void                                * p_buffer,
                                                uint32_t                            buffer_size)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->outputBufferSet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_buffer, buffer_size);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Wait for current JPEG codec operation to finish.
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           The wait function returns successfully.
 * @retval SSP_ERR_ASSERTION     p_ctrl or p_status is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_TIMEOUT       The wait operation timed out, the underlying driver did not response in time.
 * @retval SSP_ERR_WAIT_ABORTED  System internal error occurred.
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_Wait   (sf_jpeg_decode_ctrl_t           * const p_api_ctrl,
                                 jpeg_decode_status_t            * const p_status,
                                 uint32_t                                timeout)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_status);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT  tx_err = TX_SUCCESS;
    ULONG events = 0U;

    tx_err = tx_event_flags_get(&p_ctrl->events, JPEG_ALL_EVENTS, TX_OR_CLEAR, &events, timeout);

    SF_JPEG_ERROR_RETURN(TX_NO_EVENTS != tx_err, SSP_ERR_TIMEOUT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_WAIT_ABORTED);

    *p_status = (jpeg_decode_status_t) events;

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Obtain JPEG codec status.
 * This function can be used to poll the device instead of using SF_JPEG_Decode_Wait() to
 *         block on JPEG operations.
 *
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           The JPEG status information is obtained.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::statusGet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_StatusGet  (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                     jpeg_decode_status_t         * const p_status)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    err = p_ctrl->p_lower_lvl_jpeg_decode->p_api->statusGet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_status);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Obtain the format of the image.  This function is only useful for decoding a JPEG image.
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           The JPEG image size is obtained.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::pixelFormatGet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_PixelFormatGet  (sf_jpeg_decode_ctrl_t       * const   p_api_ctrl,
                                          jpeg_decode_color_space_t   * const   p_color_space)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->pixelFormatGet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_color_space);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Obtain the size of the image.  This function is only useful for decoding a JPEG image.
 * @pre  Call SF_JPEG_Decode_Open() to configure the JPEG codec block before using this function.
 *
 * @retval SSP_SUCCESS           The JPEG image size is obtained.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 * @retval SSP_ERR_IN_USE        The mutex may be unavailable for the the device.  See HAL driver for other
 *                               possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                               This function calls
 *                                   * jpeg_decode_api_t::imageSizeGet
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_ImageSizeGet  (sf_jpeg_decode_ctrl_t       * const p_api_ctrl,
                                         uint16_t                         * p_horizontal_size,
                                         uint16_t                         * p_vertical_size)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Obtain mutex before making HAL-level driver call. */
    UINT tx_err = TX_SUCCESS;

    tx_err = tx_mutex_get(&p_ctrl->mutex, TX_NO_WAIT);
    SF_JPEG_ERROR_RETURN(TX_SUCCESS == tx_err, SSP_ERR_IN_USE);

    ssp_err_t err;
    jpeg_decode_api_t const * p_jpeg_api = p_ctrl->p_lower_lvl_jpeg_decode->p_api;
    err = p_jpeg_api->imageSizeGet(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl, p_horizontal_size, p_vertical_size);

    /* Release the acquired mutex, The return code is not checked here because mutex put cannot fail when called with a
     * mutex owned by the current thread. The mutex is owned by the current thread because this call follows a
     * successful call to mutex get */
    tx_mutex_put(&p_ctrl->mutex);

    SF_JPEG_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Close JPEG codec device.  Un-finished codec operation is interrupted, and output data are discarded.
 * @pre  Call SF_JPEG_Decode_Open() to configure the timer before using this function.
 *
 * @retval SSP_SUCCESS           The JPEG decode device is successfully closed.
 * @retval SSP_ERR_ASSERTION     p_ctrl is null.
 * @retval SSP_ERR_NOT_OPEN      JPEG Decode Framework module is not yet initialized.
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_Close      (sf_jpeg_decode_ctrl_t       * const p_api_ctrl)
{
    sf_jpeg_decode_instance_ctrl_t * p_ctrl = (sf_jpeg_decode_instance_ctrl_t *) p_api_ctrl;

#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_JPEG_ERROR_RETURN(SF_JPEG_DECODE_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    /** More error checking is performed at the HAL driver level. */
#endif

    /** Call the low level driver to close the JPEG device. */
    /* Error codes of APIs called during close are not checked because returning error codes without completing the
     * close process leaves the module in an undefined and unrecoverable state. */
    p_ctrl->p_lower_lvl_jpeg_decode->p_api->close(p_ctrl->p_lower_lvl_jpeg_decode->p_ctrl);

    /** Clear information from control block so other functions know this block is closed */
    p_ctrl->open = 0U;

    /* Delete RTOS services allocated before returning error, The return codes are not checked here because mutex and
     * event_flag delete call cannot fail when called with created RTOS objects. This object was successfully
     * created in open function. */
    tx_mutex_delete(&p_ctrl->mutex);
    p_event_flags = TX_NULL;
    tx_event_flags_delete(&p_ctrl->events);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Get version and store it in provided pointer p_version.
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version is null.
 *********************************************************************************************************************/
ssp_err_t SF_JPEG_Decode_VersionGet (ssp_version_t         * const p_version)
{
#if SF_JPEG_DECODE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = s_sf_jpeg_version.version_id;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_JPEG_DECODE)
 **********************************************************************************************************************/
