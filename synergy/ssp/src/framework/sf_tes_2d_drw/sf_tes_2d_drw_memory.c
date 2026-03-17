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
 * File Name    : sf_tes_2d_drw_memory.c
 * Description  : This file defines the D/AVE D1 low-level driver memory management functions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "sf_tes_2d_drw_base.h"
#include "sf_tes_2d_drw_cfg.h"

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static bool             g_d1_init = false;
static TX_BYTE_POOL     g_d1_heap_control = {0};

/*LDRA_INSPECTED 293 S */
/*LDRA_INSPECTED 57 D - This global variable is initialized at it's declaration below. */
static int8_t g_d1_heap[SF_TES_2D_DRW_D1_HEAP_SIZE] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".heap_d1") = {0};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup SF_TES_2D_DRW
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Allocates memory in the driver heap.
 *
 * @param[in] size      Size of the memory to be allocated.
 * @retval Non-NULL     The function returns the pointer to memory chunk if memory allocation was successful.
 * @retval NULL         The function returns NULL if memory allocation was failed.
 **********************************************************************************************************************/
void * d1_allocmem(d1_uint_t size)
{
    void * ptr = NULL;

    if(false == g_d1_init)
    {
        /** Create a byte memory pool in the driver heap if this function call is the first time. */
        if(TX_SUCCESS == tx_byte_pool_create (&g_d1_heap_control, (CHAR *)"d1 heap", g_d1_heap, SF_TES_2D_DRW_D1_HEAP_SIZE))
        {
            g_d1_init = true;
        }
    }

    /** Allocate memory from a byte memory pool. */
    tx_byte_allocate(&g_d1_heap_control, &ptr, size, TX_NO_WAIT);

    return ptr;
}

/*******************************************************************************************************************//**
 * @brief  Free the specified memory area in the driver heap.
 *
 * @param[in] ptr       Pointer to the memory area to be freed.
 **********************************************************************************************************************/
void d1_freemem(void *ptr)
{
    /** Free specified memory allocated by d1_allocmem. */
    tx_byte_release(ptr);
}

/*******************************************************************************************************************//**
 * @brief  This function intends to return the size of the given memory block but we don't return valid value.
 * This function returns always 1.
 *
 * @param[in] ptr       Pointer to a memory block in Heap.
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_uint_t d1_memsize(void * ptr)
{
    SSP_PARAMETER_NOT_USED(ptr);

    /** Always return 1. */
    return (1U);
}

/*******************************************************************************************************************//**
 * @brief  Allocate video memory.
 * Synergy does not use the virtual memory space so this function exactly works same as d1_allocmem.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] memtype   Type of memory.
 * @param[in] size      Number of bytes.
 * @retval    Non-NULL  The function returns the pointer to memory chunk if memory allocation was successful.
 * @retval    NULL      The function returns Null if memory allocation was failed.
 **********************************************************************************************************************/
void * d1_allocvidmem(d1_device *handle, d1_int_t memtype, d1_uint_t size)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(memtype);

    return (d1_allocmem(size));
}

/*******************************************************************************************************************//**
 * @brief  Free video memory.
 * Synergy does not use the virtual memory space so this function exactly works same as d1_freemem.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] memtype   Type of memory.
 * @param[in] ptr       Address returned by d1_allocvidmem.
 **********************************************************************************************************************/
void d1_freevidmem(d1_device * handle, d1_int_t memtype, void * ptr)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(memtype);

    d1_freemem (ptr);
}

/*******************************************************************************************************************//**
 * @brief  Get current memory status. This function does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] memtype   Type of memory.
 * @param[in] query     Type of requested information.
 * @retval    0         The function always return 0.
 **********************************************************************************************************************/
d1_int_t d1_queryvidmem(d1_device *handle, d1_int_t memtype, d1_int_t query)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(memtype);
    SSP_PARAMETER_NOT_USED(query);

    return 0;
}

/*******************************************************************************************************************//**
 * @brief  Return hints about systems memory architecture.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @retval    d1_ma_unified  The function always return d1_ma_unified (Unified memory architecture).
 **********************************************************************************************************************/
d1_int_t d1_queryarchitecture(d1_device *handle)
{
    SSP_PARAMETER_NOT_USED(handle);

    /** Return d1_ma_unified as Memory architecture is such that CPU can directly access pointers
     * returned by allocvidmem. */
    return d1_ma_unified;
}

/*******************************************************************************************************************//**
 * @brief  Map video memory for direct CPU access.
 * Synergy does not use the virtual memory space so this function does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] ptr       Video memory address returned by d1_allocvidmem.
 * @param[in] flags     Memory mapping flags.
 * @retval    ptr       The function just returns ptr back since no mapping required for Synergy.
 **********************************************************************************************************************/
void * d1_mapvidmem(d1_device *handle, void *ptr, d1_int_t flags)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(flags);

    /* Nothing special necessary for Synergy. */
    return ptr;
}

/*******************************************************************************************************************//**
 * @brief  Release memory mapping.
 * Synergy does not use the virtual memory space so this function does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] ptr       Mapped video memory address returned by d1_mapvidmem.
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_int_t d1_unmapvidmem(d1_device *handle, void *ptr)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(ptr);

    /* No unmapping necessary for Synergy. */
    return 1;
}

/*******************************************************************************************************************//**
 * @brief  Map CPU accessible address of a video memory block back to video memory address.
 * Synergy does not use the virtual memory space so this function does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] ptr       CPU accessible address pointing to a video memory block originally allocated using d1_allocvidmem.
 * @retval    ptr       The function just returns ptr back since no mapping required for Synergy.
 **********************************************************************************************************************/
void * d1_maptovidmem(d1_device *handle, void *ptr)
{
    SSP_PARAMETER_NOT_USED(handle);

    /* Nothing special necessary for Synergy. */
    return ptr;
}

/*******************************************************************************************************************//**
 * @brief  Map already allocated video memory address to an address for direct CPU access.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] ptr       Video memory address returned by d1_allocvidmem.
 * @retval    ptr       The function just returns ptr back since no mapping required for Synergy.
 **********************************************************************************************************************/
void * d1_mapfromvidmem(d1_device *handle, void *ptr)
{
    SSP_PARAMETER_NOT_USED(handle);

    /* Nothing special necessary for Synergy. */
    return ptr;
}

/*******************************************************************************************************************//**
 * @brief  Copy data to video memory. Destination (video) memory area has to be allocated by d1_allocvidmem.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] dst       pointer into video memory (destination).
 * @param[in] src       Pointer into system memory (source).
 * @param[in] size      Number of bytes to copy.
 * @param[in] flags     Bitfield containing additional information on data to be copied.
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_int_t d1_copytovidmem(d1_device *handle, void *dst, const void *src, d1_uint_t size, d1_int_t flags)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(flags);

    /** Simply use C standard memcpy. */
    memcpy(dst, src, size);

    return 1;
}

/*******************************************************************************************************************//**
 * @brief  Copy data from video memory. Source (video) memory area has to be allocated by d1_allocvidmem.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] dst       pointer into system memory (destination).
 * @param[in] src       Pointer into video memory (source).
 * @param[in] size      Number of bytes to copy.
 * @param[in] flags     Reserved for future use.
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_int_t d1_copyfromvidmem(d1_device *handle, void *dst, const void *src, d1_uint_t size, d1_int_t flags)
{   
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(flags);

    /** Simply use C standard memcpy. */
    memcpy(dst, src, size);

    return 1;
}

/*******************************************************************************************************************//**
 * @brief  Flush CPU data caches. Synergy does not have a cache memory so does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] memtype   Memory pools to flush (can be ored together).
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_int_t d1_cacheflush(d1_device *handle, d1_int_t memtype)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(memtype);

    return 1;
}

/*******************************************************************************************************************//**
 * @brief  Flush part of CPU data caches. Synergy does not have a cache memory so does not do anything special.
 *
 * @param[in] handle    Pointer to the d1_device object.
 * @param[in] memtype   Memory pools to flush (can be ored together).
 * @param[in] ptr       Start address of memory to be flushed.
 * @param[in] size      Size of memory to be flushed.
 * @retval    1         The function always return 1.
 **********************************************************************************************************************/
d1_int_t d1_cacheblockflush(d1_device *handle, d1_int_t memtype, const void *ptr, d1_uint_t size)
{
    SSP_PARAMETER_NOT_USED(handle);
    SSP_PARAMETER_NOT_USED(memtype);
    SSP_PARAMETER_NOT_USED(ptr);
    SSP_PARAMETER_NOT_USED(size);

    return 1;
}

/*******************************************************************************************************************//**
 * @}
 **********************************************************************************************************************/
