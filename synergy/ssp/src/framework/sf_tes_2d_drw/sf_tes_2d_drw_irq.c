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
 * File Name    : sf_tes_2d_drw_irq.c
 * Description  : This file defines the D/AVE D1 low-level driver IRQ setting functions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdlib.h>
#include "sf_tes_2d_drw_base.h"
#include "sf_tes_2d_drw_cfg.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define IRQCTL_DLISTIRQ_ENABLE      (1U << 1)
#define IRQCTL_ENUMIRQ_CLEAR        (1U << 2)
#define IRQCTL_DLISTIRQ_CLEAR       (1U << 3)
#define IRQCTL_BUSIRQ_CLEAR         (1U << 5)
#define IRQCTL_ALLIRQ_DIABLE_N_CLEAR  (IRQCTL_BUSIRQ_CLEAR | IRQCTL_DLISTIRQ_CLEAR | IRQCTL_ENUMIRQ_CLEAR)
#define IRQCTL_ALLIRQ_CLEAR_AND_DLISTIRQ_ENABLE \
                        (IRQCTL_BUSIRQ_CLEAR | IRQCTL_DLISTIRQ_CLEAR | IRQCTL_ENUMIRQ_CLEAR | IRQCTL_DLISTIRQ_ENABLE)
#define STATUS_DLISTIRQ_TRIGGERED   (1U << 5)

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static d1_device_synergy * gp_context = NULL;

/*******************************************************************************************************************//**
 * @addtogroup SF_TES_2D_DRW
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

void drw_int_isr(void);

/*******************************************************************************************************************//**
 * @brief  Initialize IRQ for D/AVE 2D hardware.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @retval    0         The function returns 0 if failed in the IRQ initialization.
 * @retval    1         The function returns 1 if successfully IRQ is initialized.
 **********************************************************************************************************************/
d1_int_t d1_initirq_intern(d1_device_synergy * handle)
{
    d1_int_t ret;
    ssp_err_t err = SSP_SUCCESS;

    gp_context = handle;

    /** Clear all the D/AVE 2D IRQs and enable Display list IRQ. */
    handle->dave_reg->IRQCTL = IRQCTL_ALLIRQ_CLEAR_AND_DLISTIRQ_ENABLE;

    /** Access to FMI through the eventInfoGet interface and get the D/AVE 2D IRQ information. */
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = 0U;
    ssp_feature.unit    = 0U;
    ssp_feature.id      = SSP_IP_DRW;
    fmi_event_info_t event_info = {(IRQn_Type) 0U};
    err = g_fmi_on_fmi.eventInfoGet(&ssp_feature, SSP_SIGNAL_DRW_INT, &event_info);
    /** Check return value of eventInfoGet API, if SUCCESS Enable the D/AVE 2D IRQ Vector */
    if (SSP_SUCCESS == err)
    {
        /** Set D/AVE 2D interrupt priority in NVIC if the IRQ vector is valid. */
        NVIC_SetPriority(event_info.irq, SF_TES_2D_DRW_CFG_IRQ_IPL);

        /** Enable D/AVE 2D interrupt in NVIC if the IRQ vector is valid. */
        NVIC_EnableIRQ(event_info.irq);
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/*******************************************************************************************************************//**
 * @brief  De-initialize IRQ for D/AVE 2D hardware.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @retval    0         The function returns 0 if failed in the IRQ de-initialization.
 * @retval    1         The function returns 1 if successfully IRQ is de-initialized.
 **********************************************************************************************************************/
d1_int_t d1_shutdownirq_intern(d1_device_synergy *handle)
{
    d1_int_t ret;
    ssp_err_t err = SSP_SUCCESS;

    /** Access to FMI through the eventInfoGet interface and get the D/AVE 2D IRQ information. */
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = 0U;
    ssp_feature.unit    = 0U;
    ssp_feature.id      = SSP_IP_DRW;
    fmi_event_info_t event_info = {(IRQn_Type) 0U};
    err = g_fmi_on_fmi.eventInfoGet(&ssp_feature, SSP_SIGNAL_DRW_INT, &event_info);
    /** Check return value of eventInfoGet API, if SUCCESS disable the D/AVE 2D IRQ Vector */
    if (SSP_SUCCESS == err)
    {
        /** Disable D/AVE 2D interrupt in NVIC if the IRQ vector is valid. */
        NVIC_DisableIRQ(event_info.irq);
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    /** Clear all the D/AVE 2D IRQs and disable Display list IRQ. */
    handle->dave_reg->IRQCTL = IRQCTL_ALLIRQ_DIABLE_N_CLEAR;

    return ret;
}

/*******************************************************************************************************************//**
 * @brief  Wait for next IRQ being happened.
 *
 * @param[in] handle    Pointer to the d1_device object (Not used).
 * @param[in] irqmask   Interrupt ID (Not used. Synergy only uses Display list IRQ).
 * @param[in] timeout   Timeout value.
 * @retval    0         The function returns 0 if wait through semaphore is not successful
 * @retval    1         The function returns 1 if wait through semaphore is successful.
 **********************************************************************************************************************/
d1_int_t d1_queryirq( d1_device *handle, d1_int_t irqmask, d1_int_t timeout )
{
    d1_int_t ret = 1;
    d1_device_synergy * handle_synergy = (d1_device_synergy *) handle;

    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(irqmask);

    /** Wait for dlist processing to complete. */
    if(TX_SUCCESS != tx_semaphore_get(&handle_synergy->dlist_semaphore, (ULONG)timeout))
    {
    	ret = 0;
    }

    return ret;
}

/*******************************************************************************************************************//**
 * @}
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  DRW ISR.
 **********************************************************************************************************************/
void drw_int_isr(void)
{
    SF_CONTEXT_SAVE

    uint32_t intStatus;

    /** Just in case, check if the driver initialization done. */
    if (NULL != gp_context)
    {
        /** Get D/AVE 2D interrupt status. */
        intStatus = gp_context->dave_reg->STATUS;

        /** Clear all the D/AVE 2D interrupts (keep the Display list interrupt enable). */
        gp_context->dave_reg->IRQCTL = IRQCTL_ALLIRQ_CLEAR_AND_DLISTIRQ_ENABLE;

        /** Display list interrupt? */
        if (intStatus & STATUS_DLISTIRQ_TRIGGERED)
        {
#if SF_TES_2D_DRW_USE_DLIST_INDIRECT
            /** Display Indirect mode enabled? */
            if ((gp_context->dlist_indirect) && (0 != *(gp_context->dlist_start)))
            {
                /** Start D/AVE 2D. */
                gp_context->dave_reg->DLISTSTART = *(gp_context->dlist_start);

                /** Get next display list start address. */
                gp_context->dlist_start++;
            }
            else
#endif
            {
                /** Signal semaphore for driver. */
            	/* Return code not checked here as this RTOS object is called in an IRQ and
            	 * there is no return from an IRQ */
                tx_semaphore_ceiling_put(&gp_context->dlist_semaphore, 1UL);
            }
        }
        else
        {
            /* Do nothing. */
        }

        /** Clear IRQ status. */
        R_BSP_IrqStatusClear(R_SSP_CurrentIrqGet());
    }

    SF_CONTEXT_RESTORE
}
