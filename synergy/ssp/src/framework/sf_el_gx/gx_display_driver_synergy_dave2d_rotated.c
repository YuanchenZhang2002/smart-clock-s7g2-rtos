/*LDRA_INSPECTED 119 S */
/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/***********************************************************************************************************************
 * Copyright [2025] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Display Driver component                                         */
/**************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include    <stdio.h>
#include    <string.h>

#include    "gx_api.h"
#include    "gx_display.h"
#include    "gx_utility.h"

#if (GX_USE_SYNERGY_DRW == 1)
#include    "dave_driver.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#ifndef GX_DISABLE_ERROR_CHECKING
#define LOG_DAVE_ERRORS
#endif

#if defined(LOG_DAVE_ERRORS)
/** Macro to check for and log status code from Dave2D engine. */
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define CHECK_DAVE_STATUS(a)    gx_log_dave_error(a);
#else
/* here is error logging not enabled */
#define CHECK_DAVE_STATUS(a)    a;
#endif

/** Space used to store int to fixed point polygon vertices. */
#if (GX_USE_SYNERGY_DRW == 1)
#define MAX_POLYGON_VERTICES (GX_POLYGON_MAX_EDGE_NUM)
/*LDRA_INSPECTED 340 S GUIX uses function like macros. */
#define BYTE_RANGE(_c)       (((GX_COLOR)(_c)) > 255 ? 255UL : (GX_COLOR)(_c))
#endif

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** indicator for the number of visible frame buffer */
static GX_UBYTE   *visible_frame = NULL;
static GX_UBYTE   *working_frame = NULL;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if (GX_USE_SYNERGY_DRW == 1)
static VOID     gx_dave2d_set_rotated_texture(GX_DRAW_CONTEXT *context, d2_device *dave, INT xpos, INT ypos,
                                                                                    GX_PIXELMAP * map);
static VOID     gx_dave2d_rotated_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                  GX_POINT *map_offset, const GX_GLYPH *glyph, d2_u32 mode);
static VOID     gx_dave2d_rotated_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                  GX_POINT *map_offset, const GX_GLYPH *glyph, d2_u32 mode);
static VOID     gx_dave2d_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                  GX_POINT *map_offset, const GX_GLYPH *glyph, d2_u32 mode);
static VOID     gx_dave2d_rotated_copy_to_working(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src);
#endif
static VOID     gx_synergy_rotated_copy_to_working_32bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src);
static VOID     gx_synergy_rotated_copy_to_working_16bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src);

/** functions shared in Synergy GUIX display driver files */
#if (GX_USE_SYNERGY_DRW == 1)

#if defined(LOG_DAVE_ERRORS)
extern VOID    gx_log_dave_error(d2_s32 status);
extern INT     gx_get_dave_error(INT get_index);
#endif

extern VOID    gx_dave2d_display_list_count(GX_DISPLAY *display);
extern GX_BOOL gx_dave2d_convex_polygon_test(GX_POINT *vertex, INT num);
extern GX_BOOL gx_dave2d_power_of_two_test(GX_VALUE value);
extern VOID    gx_display_list_flush(GX_DISPLAY *display);
extern VOID    gx_display_list_open(GX_DISPLAY *display);
extern VOID    gx_dave2d_cliprect_set(d2_device *p_dave, GX_RECTANGLE *clip);
d2_device     *gx_dave2d_rotated_context_clip_set(GX_DRAW_CONTEXT *context);
extern VOID    gx_dave2d_outline_width_set(d2_device *dave, GX_FIXED_VAL width);
extern VOID    gx_dave2d_line_join_set(d2_device *dave, d2_u32 mode);
extern VOID    gx_dave2d_line_cap_set(d2_device *dave, d2_u32 mode);
extern VOID    gx_dave2d_color0_set(d2_device *dave, GX_COLOR color);
extern VOID    gx_dave2d_color1_set(d2_device *dave, GX_COLOR color);
extern VOID    gx_dave2d_alpha_set(d2_device *dave, UCHAR alpha);
extern VOID    gx_dave2d_fill_mode_set(d2_device *dave, d2_u32 mode);
extern VOID    gx_dave2d_blend_mode_set(d2_device *dave, d2_u32 srcmode, d2_u32 dstmode);
extern VOID    gx_dave2d_render_mode_set(d2_device *dave, d2_u32 mode);
extern VOID    gx_dave2d_anti_aliasing_set(d2_device *dave, d2_s32 mode);
extern VOID    gx_dave2d_glyph_palette_set(d2_device *dave, INT font_format);
extern VOID    gx_dave2d_pixelmap_palette_set(d2_device *dave, GX_PIXELMAP *map);
extern d2_u32  gx_dave2d_format_set(GX_PIXELMAP *map);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
extern VOID    _gx_dave2d_drawing_initiate(GX_DISPLAY *display, GX_CANVAS * canvas);
#endif

/** functions provided by sf_el_gx.c */
extern void sf_el_gx_frame_toggle (ULONG display_handle,  GX_UBYTE **visible);
extern void sf_el_gx_frame_pointers_get(ULONG display_handle, GX_UBYTE **visible, GX_UBYTE **working);

/***********************************************************************************************************************
 * Synergy GUIX display driver function prototypes (called by GUIX)
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_synergy_rotated_buffer_toggle(GX_CANVAS * canvas, GX_RECTANGLE * dirty);
#if (GX_USE_SYNERGY_DRW == 1)
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_drawing_initiate(GX_DISPLAY *display, GX_CANVAS *canvas);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_drawing_complete(GX_DISPLAY *display, GX_CANVAS *canvas);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_line(GX_DRAW_CONTEXT *context,
                                INT xstart, INT xend, INT ypos, INT width, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_line(GX_DRAW_CONTEXT *context,
                              INT ystart, INT yend, INT xpos, INT width, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_simple_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_rotate_16bpp(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                      INT angle, INT rot_cx, INT rot_cy);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos,
                                          GX_PIXELMAP *pixelmap, GX_UBYTE alpha);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context, INT xstart,
                                              INT xend, INT y, GX_FILL_PIXELMAP_INFO * info);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_polygon_draw(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_polygon_fill(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_write_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_write_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_blend_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_blend_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_block_move(GX_DRAW_CONTEXT *context, GX_RECTANGLE *block, INT xshift, INT yshift);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_alphamap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
        GX_POINT *map_offset,const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                            GX_POINT *map_offset, const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                   GX_POINT *map_offset, const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                            GX_POINT *map_offset, const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                   GX_POINT *map_offset, const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                            GX_POINT *map_offset, const GX_GLYPH *glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_circle_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pie_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                             INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r,
                                                                    INT start_angle, INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                              INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_arc_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                              INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_ellipse_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE * dirty);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context,
                                            INT xpos, INT ypos, GX_PIXELMAP *pixelmap, INT angle, INT cx, INT cy);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos,
                                                    INT ypos, GX_PIXELMAP *pixelmap, INT angle, INT cx, INT cy);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT x, INT y,
                      INT xx, INT yy, INT newxpos, INT newypos, GX_PIXELMAP *pixelmap, INT xdiff, INT ydiff);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate_handle_edge(INT xx, INT yy, GX_PIXELMAP *pixelmap,
                                INT xdiff, INT ydiff, GX_COLOR * red, GX_COLOR * green, GX_COLOR * blue, INT * alpha);
#endif

/***********************************************************************************************************************
 * Functions
 ***********************************************************************************************************************/
#if (GX_USE_SYNERGY_DRW == 1)

/*******************************************************************************************************************//**
 * @addtogroup SF_EL_GX
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, close previous frame and set new canvas drawing address.
 * This function is called by GUIX to initiate canvas drawing.
 * @param   display[in]         Pointer to a GUIX display context
 * @param   canvas[in]          Pointer to a GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_drawing_initiate(GX_DISPLAY *display, GX_CANVAS * canvas)
{
    _gx_dave2d_drawing_initiate(display, canvas);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, complete drawing. This function does nothing special. The buffer toggle
 * function takes care of toggling the display lists.
 * @param   display[in]         Pointer to a GUIX display context
 * @param   canvas[in]          Pointer to a GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_drawing_complete(GX_DISPLAY *display, GX_CANVAS * canvas)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(display);
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(canvas);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, copy canvas function with D/AVE 2D acceleration enabled 
 * for 90 and 270 degree screen rotation.
 * This function is called by GUIX to copy canvas data from one canvas to another.
 * @param   canvas[in]          Pointer to a source GUIX canvas
 * @param   composite[in]       Pointer to a destination GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
    d2_u32 mode;
    GX_DISPLAY *display = composite->gx_canvas_display;
    d2_device  *dave = display -> gx_display_accelerator;
    GX_RECTANGLE dirty;
    GX_RECTANGLE rotated_dirty;
    INT destx;
    INT desty;
    INT flag = d2_bf_no_blitctxbackup;

    dirty.gx_rectangle_bottom = (GX_VALUE) 0;
    dirty.gx_rectangle_left = (GX_VALUE) 0;
    dirty.gx_rectangle_right = (GX_VALUE) 0;
    dirty.gx_rectangle_top = (GX_VALUE) 0;

    _gx_utility_rectangle_define(&dirty, 0, 0,
            (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
            (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));
    _gx_utility_rectangle_shift(&dirty, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    if(!_gx_utility_rectangle_overlap_detect(&dirty, &composite->gx_canvas_dirty_area, &dirty))
    {
        return;
    }

    if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
    {
        rotated_dirty.gx_rectangle_left = dirty.gx_rectangle_top;
        rotated_dirty.gx_rectangle_top = (GX_VALUE)(composite->gx_canvas_x_resolution - dirty.gx_rectangle_right -
                                                                                                        (UCHAR)1);
        rotated_dirty.gx_rectangle_right = dirty.gx_rectangle_bottom;
        rotated_dirty.gx_rectangle_bottom = (GX_VALUE)(composite->gx_canvas_x_resolution - dirty.gx_rectangle_left -
                                                                                                         (UCHAR)1);
        
        destx = canvas->gx_canvas_display_offset_y;
        desty = composite->gx_canvas_x_resolution - canvas->gx_canvas_x_resolution - canvas->gx_canvas_display_offset_x;
    }
    else
    {
        rotated_dirty.gx_rectangle_top = dirty.gx_rectangle_left;
        rotated_dirty.gx_rectangle_bottom = dirty.gx_rectangle_right;
        rotated_dirty.gx_rectangle_left = (GX_VALUE)(composite->gx_canvas_y_resolution - dirty.gx_rectangle_bottom - 1);
        rotated_dirty.gx_rectangle_right = (GX_VALUE)(composite->gx_canvas_y_resolution - dirty.gx_rectangle_top - 1);
        
        destx = composite->gx_canvas_y_resolution - canvas->gx_canvas_y_resolution - canvas->gx_canvas_display_offset_y;
        desty = canvas->gx_canvas_display_offset_x;
    }

    gx_dave2d_cliprect_set(dave, &rotated_dirty);

    switch ((INT)display->gx_display_color_format)
    {
        case GX_COLOR_FORMAT_565RGB:
            mode = d2_mode_rgb565;
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            mode = d2_mode_argb8888;
            break;

        case GX_COLOR_FORMAT_8BIT_PALETTE:
            /*LDRA_INSPECTED 120 S */
            mode = d2_mode_alpha8;
            flag |= (INT) d2_bf_usealpha;
            CHECK_DAVE_STATUS(d2_setalpha(dave, 0xff))
            break;

        default:
            return;
    }

    /** Set the alpha blend value to opaque. */
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    CHECK_DAVE_STATUS(d2_framebuffer(display -> gx_display_accelerator, composite -> gx_canvas_memory,
                     (d2_s32)(composite -> gx_canvas_y_resolution), (d2_u32)(composite -> gx_canvas_y_resolution),
                     (d2_u32)(composite -> gx_canvas_x_resolution), (d2_s32)mode))


    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) canvas->gx_canvas_memory,
                           canvas->gx_canvas_y_resolution,
                           canvas->gx_canvas_y_resolution,
                           canvas->gx_canvas_x_resolution, mode))

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                canvas->gx_canvas_y_resolution,
                canvas->gx_canvas_x_resolution,
                0, 0,
                /*LDRA_INSPECTED 50 S */
                (d2_width)(D2_FIX4(canvas->gx_canvas_y_resolution)),
                /*LDRA_INSPECTED 50 S */
                (d2_width)(D2_FIX4(canvas->gx_canvas_x_resolution)),
                /*LDRA_INSPECTED 50 S */
                (d2_point)(D2_FIX4(destx)),
                /*LDRA_INSPECTED 50 S */
                (d2_point)(D2_FIX4(desty)),
                /*LDRA_INSPECTED 96 S D/AVE 2D uses mixed mode arithmetic for the macro d2_bf_no_blitctxbackup. */
                (UINT)flag))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, blend canvas function with D/AVE 2D acceleration enabled  
 * for 90 and 270 degree screen rotation.
 * This function is called by GUIX to blend source canvas with the existing data in the destination canvas.
 * @param   canvas[in]          Pointer to a source GUIX canvas
 * @param   composite[in]       Pointer to a destination GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_canvas_blend(GX_CANVAS * canvas, GX_CANVAS *composite)
{
    d2_u32 mode;
    GX_DISPLAY *display = canvas->gx_canvas_display;
    d2_device *dave = display -> gx_display_accelerator;
    GX_RECTANGLE dirty;
    GX_RECTANGLE rotated_dirty;
    INT destx;
    INT desty;

    dirty.gx_rectangle_bottom = (GX_VALUE) 0;
    dirty.gx_rectangle_left = (GX_VALUE) 0;;
    dirty.gx_rectangle_right = (GX_VALUE) 0;;
    dirty.gx_rectangle_top = (GX_VALUE) 0;

    /** Define a rectangle based on canvas X and Y coordinate values. */
    _gx_utility_rectangle_define(&dirty, 0, 0,
            (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
            (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));
    _gx_utility_rectangle_shift(&dirty, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    if(!_gx_utility_rectangle_overlap_detect(&dirty, &composite->gx_canvas_dirty_area, &dirty))
    {
        return;
    }

    /** Set the rectanngle coordinates based on the rotation angle. */
    if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
    {
        rotated_dirty.gx_rectangle_left = dirty.gx_rectangle_top;
        rotated_dirty.gx_rectangle_top = (GX_VALUE)(composite->gx_canvas_x_resolution - dirty.gx_rectangle_right -
                                                                                                         (UCHAR)1);
        rotated_dirty.gx_rectangle_right = dirty.gx_rectangle_bottom;
        rotated_dirty.gx_rectangle_bottom = (GX_VALUE)(composite->gx_canvas_x_resolution - dirty.gx_rectangle_left -
                                                                                                         (UCHAR)1);

        destx = canvas->gx_canvas_display_offset_y;
        desty = composite->gx_canvas_x_resolution - canvas->gx_canvas_x_resolution - canvas->gx_canvas_display_offset_x;
    }
    else
    {
        rotated_dirty.gx_rectangle_top = dirty.gx_rectangle_left;
        rotated_dirty.gx_rectangle_bottom = dirty.gx_rectangle_right;
        rotated_dirty.gx_rectangle_left = (GX_VALUE)(composite->gx_canvas_y_resolution - dirty.gx_rectangle_bottom - 1);
        rotated_dirty.gx_rectangle_right = (GX_VALUE)(composite->gx_canvas_y_resolution - dirty.gx_rectangle_top - 1);

        destx = composite->gx_canvas_y_resolution - canvas->gx_canvas_y_resolution - canvas->gx_canvas_display_offset_y;
        desty = canvas->gx_canvas_display_offset_x;
    }

    gx_dave2d_cliprect_set(dave, &rotated_dirty);

    switch ((INT)display->gx_display_color_format)
    {
        case GX_COLOR_FORMAT_565RGB:
            mode = d2_mode_rgb565;
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            mode = d2_mode_argb8888;
            break;

        case GX_COLOR_FORMAT_8BIT_PALETTE:
            mode = d2_mode_alpha8;
            break;

        default:
            return;
    }

    CHECK_DAVE_STATUS(d2_framebuffer(display -> gx_display_accelerator, composite -> gx_canvas_memory,
                      (d2_s32)(composite -> gx_canvas_y_resolution), (d2_u32)(composite -> gx_canvas_y_resolution),
                      (d2_u32)(composite -> gx_canvas_x_resolution), (d2_s32)mode))

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) canvas->gx_canvas_memory,
                           canvas->gx_canvas_y_resolution,
                           canvas->gx_canvas_y_resolution,
                           canvas->gx_canvas_x_resolution, mode))

    /** Set the alpha blend value as specified. */
    gx_dave2d_alpha_set(dave, canvas->gx_canvas_alpha);

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                canvas->gx_canvas_y_resolution,
                canvas->gx_canvas_x_resolution,
                0, 0,
                /*LDRA_INSPECTED 50 S */
                (d2_width)(D2_FIX4(canvas->gx_canvas_y_resolution)),
                /*LDRA_INSPECTED 50 S */
                (d2_width)(D2_FIX4(canvas->gx_canvas_x_resolution)),
                /*LDRA_INSPECTED 50 S */
                (d2_point)(D2_FIX4(destx)),
                /*LDRA_INSPECTED 50 S */
                (d2_point)(D2_FIX4(desty)),
                d2_bf_no_blitctxbackup))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, horizontal line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a horizontal line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a horizontal line in pixel unit
 * @param   xend[in]            x axis end position of a horizontal line in pixel unit
 * @param   ypos[in]            y position of a horizontal line
 * @param   width[in]           Line width in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_line(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width,
                                                                                             GX_COLOR color)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    INT rotated_xpos;
    INT rotated_ypos;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_fill_mode_set(dave, d2_fm_color);
    gx_dave2d_color0_set(dave, color);
    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    /** Define x and y positions based on rotation angle */
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        rotated_xpos = ypos;
        rotated_ypos = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - xend - 1;
    }
    else
    {
        rotated_xpos = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - ypos - width;
        rotated_ypos = xstart;
    }

    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point)(D2_FIX4((USHORT)rotated_xpos)),
                                         (d2_point)(D2_FIX4((USHORT)rotated_ypos)),
                                         (d2_point)(D2_FIX4((USHORT)width)),
                                         (d2_point)(D2_FIX4((USHORT)((xend - xstart) + 1)))))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, vertical line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a vertical line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   ystart[in]          y axis start position of a vertical line in pixel unit
 * @param   yend[in]            y axis end position of a vertical line in pixel unit
 * @param   xpos[in]            x position of a vertical line
 * @param   width[in]           Line width in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_line(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width,
                                                                                           GX_COLOR color)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    INT height = (yend - ystart) + 1;
    INT rotated_xpos;
    INT rotated_ypos;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_fill_mode_set(dave, d2_fm_color);
    gx_dave2d_color0_set(dave, color);
    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    /** Define x and y positions based on rotation angle */
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        rotated_xpos = ystart;
        rotated_ypos = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - xpos - width;
    }
    else
    {
        rotated_xpos = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - yend - 1;
        rotated_ypos = xpos;
    }

    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point)(D2_FIX4((USHORT)rotated_xpos)),
                                          (d2_point)(D2_FIX4((USHORT)rotated_ypos)),
                                          (d2_point)(D2_FIX4((USHORT)height)),
                                          (d2_point)(D2_FIX4((USHORT)width))))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, aliased simple line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a aliased simple line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a simple line in pixel unit
 * @param   ystart[in]          y axis start position of a simple line in pixel unit
 * @param   xend[in]            x axis end position of a simple line in pixel unit
 * @param   yend[in]            y axis end position of a simple line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_line_color);

    /** Swap xstart and ystart coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xstart, ystart);

    /** Swap xend and yend coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xend, yend);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new ystart and yend coordinates */
        ystart = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ystart - 1;
        yend = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - yend - 1;
    }
    else
    {
        /** Else, define new xstart and xend coordinates */
        xstart = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xstart - 1;
        xend = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xend - 1;
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, aliased wide line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a aliased wide line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a wide line in pixel unit
 * @param   ystart[in]          y axis start position of a wide line in pixel unit
 * @param   xend[in]            x axis end position of a wide line in pixel unit
 * @param   yend[in]            y axis end position of a wide line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_simple_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    GX_BRUSH *brush = &context->gx_draw_context_brush;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    /** Set a rendering mode to solid. */
    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);

    /** Set the line end style based on brush style. */
    if(brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_cap_set(dave, d2_lc_round);
    }
    else
    {
        gx_dave2d_line_cap_set(dave, d2_lc_butt);
    }

    /** Swap xstart and ystart coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xstart, ystart);

    /** Swap xend and yend coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xend, yend);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new ystart and yend coordinates */
        ystart = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ystart - 1;
        yend = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - yend - 1;
    }
    else
    {
        /** Else, define new xstart and xend coordinates */
        xstart = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xstart - 1;
        xend = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xend - 1;
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)brush->gx_brush_width)),
                                          d2_le_exclude_none))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, anti-aliased line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw an anti-aliased wide line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of an anti-aliased line in pixel unit
 * @param   ystart[in]          y axis start position of an anti-aliased line in pixel unit
 * @param   xend[in]            x axis end position of an anti-aliased line in pixel unit
 * @param   yend[in]            y axis end position of an anti-aliased line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    /** Set a rendering mode to solid. */
    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_color0_set(dave, context -> gx_draw_context_brush.gx_brush_line_color);

    /** Swap xstart and ystart coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xstart, ystart);

    /** Swap xend and yend coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xend, yend);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new ystart and yend coordinates */
        ystart = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ystart - 1;
        yend = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - yend - 1;
    }
    else
    {
        /** Else, define new xstart and xend coordinates */
        xstart = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xstart - 1;
        xend = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xend - 1;
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, horizontal pattern draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a horizontal pattern line for RGB565 color format for 90 and 270 
 * degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a horizontal pattern line in pixel unit
 * @param   xend[in]            x axis end position of a horizontal pattern line in pixel unit
 * @param   ypos[in]            y position of a horizontal pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 16bpp horizontal pattern line draw routine. */
    _gx_display_driver_16bpp_rotated_horizontal_pattern_line_draw(context, xstart, xend, ypos);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, horizontal pattern draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a horizontal pattern line for RGB888 color format for 90 and 270 
 * degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a horizontal pattern line in pixel unit
 * @param   xend[in]            x axis end position of a horizontal pattern line in pixel unit
 * @param   ypos[in]            y position of a horizontal pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 32bpp horizontal pattern line draw routine. */
    _gx_display_driver_32bpp_rotated_horizontal_pattern_line_draw(context, xstart, xend, ypos);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, vertical pattern draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a vertical pattern line for RGB565 color format for 90 and 270 
 * degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   ystart[in]          y axis start position of a vertical pattern line in pixel unit
 * @param   yend[in]            y axis end position of a vertical pattern line in pixel unit
 * @param   xpos[in]            x position of a vertical pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 16bpp vertical pattern line draw routine. */
    _gx_display_driver_16bpp_rotated_vertical_pattern_line_draw(context, ystart, yend, xpos);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, vertical pattern draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a vertical pattern line for RGB888 color format for 90 and 270 
 * degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   ystart[in]          y axis start position of a vertical pattern line in pixel unit
 * @param   yend[in]            y axis end position of a vertical pattern line in pixel unit
 * @param   xpos[in]            x position of a vertical pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_vertical_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 32bpp vertical pattern line draw routine. */
    _gx_display_driver_32bpp_rotated_vertical_pattern_line_draw(context, ystart, yend, xpos);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, anti-aliased wide line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw an anti-aliased wide line for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of an anti-aliased wide line in pixel unit
 * @param   ystart[in]          y axis start position of an anti-aliased wide line in pixel unit
 * @param   xend[in]            x axis end position of an anti-aliased wide line in pixel unit
 * @param   yend[in]            y axis end position of an anti-aliased wide line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    GX_BRUSH *brush = &context->gx_draw_context_brush;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);

    if(brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_cap_set(dave, d2_lc_round);
    }
    else
    {
        gx_dave2d_line_cap_set(dave, d2_lc_butt);
    }

    /** Swap xstart and ystart coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xstart, ystart);

    /** Swap xend and yend coordinates */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xend, yend);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new ystart and yend coordinates */
        ystart = (INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution) - ystart - 1;
        yend = (INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution) - yend - 1;
    }
    else
    {
        /** Else, define new xstart and xend coordinates */
        xstart = (INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution) - xstart - 1;
        xend = (INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution) - xend - 1;
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)brush->gx_brush_width)),
                                          d2_le_exclude_none))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 16bpp pixelmap rorate function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a pixelmap for 90 and 270 degree screen rotation. 
 * Currently 8bpp, 16bpp, and 32 bpp source formats are supported.
 * RLE compression is available as an option.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x-coordinate of top-left draw point in pixel unit
 * @param   ypos[in]            y-coordinate of top-left draw point in pixel unit
 * @param   pixelmap[in]        Pointer to a GX_PIXELMAP structure
 * @param   angle[in]           The angle to rotate
 * @param   rot_cx[in]          x-coordinate of rotating center
 * @param   rot_cy[in]          y-coordinate of rotating center
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_rotate_16bpp(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                                INT angle, INT rot_cx, INT rot_cy)
{
    GX_DISPLAY *display;

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        return;
    }

    display = context -> gx_draw_context_display;

    angle = angle % 360;

    if (angle < 0)
    {
        angle += 360;
    }
    
    /** Draw Pixelmap based on the its angle and format. */
    if (0 == angle)
    {
        if (display -> gx_display_driver_pixelmap_draw)
        {
            display -> gx_display_driver_pixelmap_draw(context, xpos, ypos, pixelmap);
            return;
        }
    }

    switch((INT)pixelmap -> gx_pixelmap_format)
    {
        case GX_COLOR_FORMAT_565RGB:
            _gx_display_driver_565rgb_rotated_pixelmap_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_32ARGB:
            if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
            {

                if ((angle % 90) == 0)
                {
                    /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
                    _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_simple_rotate(context, xpos, ypos,
                                                                                           pixelmap,
                                                                                           angle, rot_cx, rot_cy);
                }
                else
                {
                    _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_rotate(context, xpos, ypos,
                                                                                    pixelmap, angle,
                                                                                    rot_cx, rot_cy);
                }
            }
            break;

        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixelmap draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a pixelmap for 90 and 270 degree screen rotation. 
 * Currently 8bpp, 16bpp, and 32 bpp source formats are supported.
 * RLE compression is available as an option.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x axis position of a pixelmap in pixel unit
 * @param   ypos[in]            y axis position of a pixelmap in pixel unit
 * @param   pixelmap[in]        Pointer to a pixelmap
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
    d2_u32  mode;
    d2_device *dave;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;

    if(0U == brush_alpha)
    {
        return;
    }
    if(brush_alpha != (GX_UBYTE)GX_ALPHA_VALUE_OPAQUE)
    {
        _gx_dave2d_rotated_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
        return;
    }
    else
    {
        dave = gx_dave2d_rotated_context_clip_set(context);
        gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
    }
#else
    dave = gx_dave2d_rotated_context_clip_set(context);
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    mode = gx_dave2d_format_set(pixelmap);

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= d2_mode_rle;
    }

    if ((mode & d2_mode_clut) == d2_mode_clut)
    {
        gx_dave2d_pixelmap_palette_set(dave, pixelmap);
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)pixelmap -> gx_pixelmap_data,
                           pixelmap -> gx_pixelmap_height, pixelmap -> gx_pixelmap_height,
                           pixelmap -> gx_pixelmap_width, mode))

    /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_no_blitctxbackup. */
    mode = (d2_u32)d2_bf_no_blitctxbackup;

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_usealpha. */
        mode |= (d2_u32)d2_bf_usealpha;
    }

    if(pixelmap->gx_pixelmap_flags & GX_PIXELMAP_TRANSPARENT)
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_usealpha. */
        mode |= (d2_u32)d2_bf_usealpha;
    }

    /** Swap x-axis and y-axis positions of a pixelmap */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xpos, ypos);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new y axis position on a pixelmap */
        ypos = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ypos -
                                                        (INT)pixelmap->gx_pixelmap_width;
    }
    else
    {
        /** Else, define new x axis position on a pixelmap */
        xpos = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xpos -
                                                      (INT)pixelmap->gx_pixelmap_height;
    }

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         pixelmap -> gx_pixelmap_height,
                         pixelmap -> gx_pixelmap_width,
                         0, 0,
                         (d2_width)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_height)),
                         (d2_width)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_width)),
                         (d2_point)(D2_FIX4((USHORT)xpos)),
                         (d2_point)(D2_FIX4((USHORT)ypos)),
                         mode))

    if(pixelmap->gx_pixelmap_flags & GX_PIXELMAP_DYNAMICALLY_ALLOCATED)
    {
        /** Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context->gx_draw_context_display);

        /** Open next display list before we go. */
        gx_display_list_open(context->gx_draw_context_display);
    }
    else
    {
        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixelmap alpha-blend function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to alpha-blend a pixelmap for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x axis position of a pixelmap in pixel unit
 * @param   ypos[in]            y axis position of a pixelmap in pixel unit
 * @param   pixelmap[in]        Pointer to a pixelmap
 * @param   alpha[in]           GUIX alpha value
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixelmap_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos,
                                      GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if ((GX_UBYTE)0 == alpha)
    {
        return;
    }
    d2_u32  mode;
    mode = (d2_u32) 0;
    mode = gx_dave2d_format_set(pixelmap);


    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= (d2_u32)d2_mode_rle;
    }

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

    if ((mode & (d2_u32)d2_mode_clut) == (d2_u32)d2_mode_clut)
    {
        gx_dave2d_pixelmap_palette_set(dave, pixelmap);
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)pixelmap -> gx_pixelmap_data,
                           pixelmap -> gx_pixelmap_height, pixelmap -> gx_pixelmap_height,
                           pixelmap -> gx_pixelmap_width, mode))

    /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_no_blitctxbackup. */
    mode = (d2_u32)d2_bf_no_blitctxbackup;

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_usealpha. */
        mode |= (d2_u32)d2_bf_usealpha;
    }

    /** Set the alpha blend value as specified. */
    gx_dave2d_alpha_set(dave, alpha);

    /** Swap x-axis and y-axis positions of a pixelmap */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xpos, ypos);
    
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new y axis position on a pixelmap */
        ypos = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ypos -
                                                       (INT)pixelmap->gx_pixelmap_width;
    }
    else
    {
        /** Else, define new x axis position on a pixelmap */
        xpos = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xpos -
                                                      (INT)pixelmap->gx_pixelmap_height;
    }

    /** Do the bitmap drawing. */
    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         pixelmap -> gx_pixelmap_height,
                         pixelmap -> gx_pixelmap_width,
                         0, 0,
                         (d2_width)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_height)),
                         (d2_width)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_width)),
                         (d2_point)(D2_FIX4((USHORT)xpos)),
                         (d2_point)(D2_FIX4((USHORT)ypos)),
                         (mode)))

    /** Set the alpha value. */
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, horizontal pixelmap line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a horizontal pixelmap for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context

 * @param   ystart[in]          y axis start position of a horizontal pixelmap line in pixel unit
 * @param   yend[in]            y axis end position of a horizontal pixelmap line in pixel unit
 * @param   x[in]               x axis position of a horizontal pixelmap line in pixel unit
 * @param   info[in]            Pointer to a pixelmap info
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend,
                                              INT x, GX_FILL_PIXELMAP_INFO *info)
{
    GX_RECTANGLE   *old_clip;
    GX_RECTANGLE    clip;
    GX_PIXELMAP    *pixelmap = info->pixelmap;

    if ((info -> draw) && (ystart <= yend))
    {
        old_clip = context->gx_draw_context_clip;
        context->gx_draw_context_clip = &clip;

        /** Set rectangle coordinates. */
        clip.gx_rectangle_left = (GX_VALUE)x;
        clip.gx_rectangle_right = (GX_VALUE)x;
        clip.gx_rectangle_top = (GX_VALUE)ystart;
        clip.gx_rectangle_bottom = (GX_VALUE)yend;

        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            ystart = (ystart - ((info -> x_offset) % (pixelmap -> gx_pixelmap_height)));
            x = ((x + (info -> y_offset)) - ((pixelmap->gx_pixelmap_width) + 1));

            while(ystart <= yend)
            {
                /** Draw pixelmap. */
                _gx_dave2d_rotated_pixelmap_draw(context, x, ystart, pixelmap);

                ystart += pixelmap->gx_pixelmap_height;
            }
        }
        else
        {
            info->x_offset = ((yend - ystart) + info->x_offset) % info->pixelmap->gx_pixelmap_height;

            if (info->x_offset)
            {
                info->x_offset = ((INT)(info->pixelmap->gx_pixelmap_height)) - info->x_offset;
            }

            yend = yend + info->x_offset + 1;
            x = x - info->y_offset;

            while(yend >= ystart)
            {
                yend -= pixelmap->gx_pixelmap_height;
                _gx_dave2d_rotated_pixelmap_draw(context, x, yend, pixelmap);
            }
        }
        context -> gx_draw_context_clip = old_clip;
    }

    info -> y_offset ++;
    if(info -> y_offset >= pixelmap -> gx_pixelmap_width)
    {
        info -> y_offset = 0;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, polygon draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a polygon for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   vertex[in]          Pointer to GUIX point data
 * @param   num[in]             Number of points
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_polygon_draw(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
    INT index = 0;
    GX_VALUE    val;
    d2_point    data[MAX_POLYGON_VERTICES * 2] = { 0 };
    GX_BRUSH   *brush = &context->gx_draw_context_brush;
    GX_UBYTE    brush_alpha = brush->gx_brush_alpha;

    /** Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    /** Convert incoming point data to d2_point type. */
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        for (INT loop = 0; loop < num; loop++)
        {
            val = vertex[loop].gx_point_y;
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
            val = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution - 1 - vertex[loop].gx_point_x);
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        }
    }
    else
    {
        for (INT loop = 0; loop < num; loop++)
        {
            val = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution - 1 - vertex[loop].gx_point_y);
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
            val = vertex[loop].gx_point_x;
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        }
    }

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S */
    gx_dave2d_outline_width_set(dave, GX_FIXED_VAL_MAKE(brush->gx_brush_width));
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    /** Enable or Disable anti-aliasing based on the brush style set. */
    if(brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_join_set(dave, d2_lj_round);
    }
    else
    {
        gx_dave2d_line_join_set(dave, d2_lj_miter);
    }
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    CHECK_DAVE_STATUS(d2_renderpolygon(dave, (d2_point *)data, (d2_u32)num, 0))
    
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, polygon fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill a polygon for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   vertex[in]          Pointer to GUIX point data
 * @param   num[in]             Number of points
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_polygon_fill(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
    /** Check if polygon to be render is convex polygon. */
    if(!gx_dave2d_convex_polygon_test(vertex, num))
    {
        /** If convex polygon, Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context -> gx_draw_context_display);
        /** If convex polygon, Open next display list before we go. */
        gx_display_list_open(context -> gx_draw_context_display);

        /** If convex polygon, Call the GUIX generic polygon fill function. */
        _gx_display_driver_generic_rotated_polygon_fill(context, vertex, num);

       /** If convex polygon, return from the function. */
        return;
    }

    INT         index = 0;
    GX_VALUE    val;
    d2_point    data[MAX_POLYGON_VERTICES * 2] = { 0 };
    GX_BRUSH   *brush = &context->gx_draw_context_brush;
    d2_device  *dave = gx_dave2d_rotated_context_clip_set(context);
    INT         skip_line;
    GX_PIXELMAP *map = brush->gx_brush_pixelmap;
    INT         xmin;
    INT         xmax;
    INT         ymin;
    INT         ymax;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE    brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    xmin = vertex[0].gx_point_x;
    xmax = vertex[0].gx_point_x;
    ymin = vertex[0].gx_point_y;
    ymax = vertex[0].gx_point_y;

    /** Convert incoming point data to d2_point type. */
    for (INT loop = 0; loop < num; loop++)
    {
        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            val = vertex[loop].gx_point_y;
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
            val = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution - 1 - vertex[loop].gx_point_x);
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        }
        else
        {
            val = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution - 1 - vertex[loop].gx_point_y);
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
            val = vertex[loop].gx_point_x;
            data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        }

        if(vertex[loop].gx_point_x < xmin)
        {
            xmin = vertex[loop].gx_point_x;
        }

        if(vertex[loop].gx_point_x > xmax)
        {
            xmax = vertex[loop].gx_point_x;
        }

        if(vertex[loop].gx_point_y < ymin)
        {
            ymin = vertex[loop].gx_point_y;
        }

        if(vertex[loop].gx_point_y > ymax)
        {
            ymax = vertex[loop].gx_point_y;
        }
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            skip_line = ((xmax - xmin) + 1) % (map->gx_pixelmap_width);
            if(skip_line)
            {
                skip_line = ((INT)(map->gx_pixelmap_width)) - skip_line;
            }

            xmin = ymin;
            ymin = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution)) - xmax - 1 - skip_line;
        }
        else
        {
            skip_line = ((ymax - ymin) + 1) % (map->gx_pixelmap_height);
            if(skip_line)
            {
                skip_line = ((INT)(map->gx_pixelmap_height)) - skip_line;
            }
            ymin = xmin;
            xmin = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution)) - ymax - 1 - skip_line;
        }
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);

        gx_dave2d_set_rotated_texture(context, dave,
                xmin,
                ymin,
                brush->gx_brush_pixelmap);

    }
    else
    {
        gx_dave2d_fill_mode_set(dave, (d2_u32)d2_fm_color);
        gx_dave2d_color0_set(dave, brush->gx_brush_fill_color);
    }
    d2_renderpolygon(dave, (d2_point *)data, (d2_u32)num, 0);
    
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixel write function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to write a pixel for RGB565 color formats and for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, the GUIX generic pixel write routine is used since there is no performance 
 * benefit for single pixel write operation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   x[in]               x position in pixel unit
 * @param   y[in]               y position in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_write_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 16bpp pixel write routine. */
    _gx_display_driver_16bpp_rotated_pixel_write(context, x, y, color);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixel write function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to write a pixel for RGB888 color formats and for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, the GUIX generic pixel write routine is used since there is no performance 
 * benefit for single pixel write operation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   x[in]               x position in pixel unit
 * @param   y[in]               y position in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_write_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 32bpp pixel write routine. */
    _gx_display_driver_32bpp_rotated_pixel_write(context, x, y, color);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixel blend function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to blend a pixel for RGB565 color formats and for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, the GUIX generic pixel blend routine is used since there is no performance 
 * benefit for single pixel blend operation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   x[in]               x position in pixel unit
 * @param   y[in]               y position in pixel unit
 * @param   fcolor[in]          GUIX color data
 * @param   alpha[in]           Alpha value
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_blend_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic RGB565 pixel blend routine. */
    _gx_display_driver_565rgb_rotated_pixel_blend(context, x, y, fcolor, alpha);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pixel blend function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to blend a pixel for RGB888 color formats and for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, the GUIX generic pixel blend routine is used since there is no performance 
 * benefit for single pixel blend operation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   x[in]               x position in pixel unit
 * @param   y[in]               y position in pixel unit
 * @param   fcolor[in]          GUIX color data
 * @param   alpha[in]           Alpha value
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pixel_blend_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic RGB888 pixel blend routine. */
    _gx_display_driver_24xrgb_rotated_pixel_blend(context, x, y, fcolor, alpha);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}


/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, block move function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to move a block of pixels within a working canvas memory and for 90 and 270 
 * degree screen rotation. Mainly used for fast scrolling.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   block[in]           Pointer to a block to be moved
 * @param   xshift[in]          x axis shift in pixel unit
 * @param   yshift[in]          y axis shift in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_block_move(GX_DRAW_CONTEXT *context, GX_RECTANGLE *block, INT xshift, INT yshift)
{
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    GX_VALUE rotated_left;
    GX_VALUE rotated_top;
    INT width  = (block->gx_rectangle_right - block->gx_rectangle_left) + 1;
    INT height = (block->gx_rectangle_bottom - block->gx_rectangle_top) + 1;

    /** Define rotated left and top positions based on rotation angle */
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        rotated_left = block->gx_rectangle_top;
        rotated_top = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                            block->gx_rectangle_right - 1);
    }
    else
    {
        rotated_left = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                            block->gx_rectangle_bottom - 1);
        rotated_top = block->gx_rectangle_left;

        yshift = -yshift;
        xshift = -xshift;
    }

    CHECK_DAVE_STATUS(d2_utility_fbblitcopy(dave, (d2_u16)height, (d2_u16)width,
                      (d2_blitpos)(rotated_left),
                      (d2_blitpos)(rotated_top),
                      (d2_blitpos)(rotated_left + yshift),
                      (d2_blitpos)(rotated_top - xshift),
                      d2_bf_no_blitctxbackup))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, alpha-map draw draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw alpha-map for 90 and 270 degree screen rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x axis position in pixel unit
 * @param   ypos[in]            y axis position in pixel unit
 * @param   pixelmap[in]        Pointer to pixelmap data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_alphamap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
    d2_u32  mode;

    mode = d2_mode_alpha8;

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= d2_mode_rle;
    }

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)pixelmap -> gx_pixelmap_data,
                           pixelmap -> gx_pixelmap_height, pixelmap -> gx_pixelmap_height,
                           pixelmap -> gx_pixelmap_width, mode))

    /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_bf_no_blitctxbackup. */
    mode = d2_bf_no_blitctxbackup;

    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_fill_color);

    /** Swap x-axis and y-axis positions of a pixelmap */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xpos, ypos);
    
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new y axis position on a pixelmap */
        ypos = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ypos -
                                                       (INT)pixelmap->gx_pixelmap_width;
    }
    else
    {
        /** Else, define new x axis position on a pixelmap */
        xpos = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xpos -
                                                       (INT)pixelmap->gx_pixelmap_height;
    }

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         pixelmap -> gx_pixelmap_height,
                         pixelmap -> gx_pixelmap_width,
                         0, 0,
                         (d2_width)(D2_FIX4((USHORT)(pixelmap -> gx_pixelmap_height))),
                         (d2_width)(D2_FIX4((USHORT)(pixelmap -> gx_pixelmap_width))),
                         (d2_point)(D2_FIX4((USHORT)xpos)),
                         (d2_point)(D2_FIX4((USHORT)ypos)),
                         ((d2_u32)d2_bf_usealpha|(d2_u32)d2_bf_colorize)))

    /** Check if pixelmap is dynamically allocated. */
    if(pixelmap->gx_pixelmap_flags & GX_PIXELMAP_DYNAMICALLY_ALLOCATED)
    {
        /** Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context->gx_draw_context_display);

        /** Open next display list before we go. */
        gx_display_list_open(context->gx_draw_context_display);
    }
    else
    {
        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8-bit compressed glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 8-bit compressed glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area,
                                                       GX_POINT * map_offset, const GX_GLYPH * glyph)
{
    d2_u32      mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *)glyph;
    if (compressed_glyph -> gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_rotated_glyph_8bit_draw(context, draw_area, map_offset, glyph, mode);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 8-bit raw glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph)
{
    gx_dave2d_rotated_glyph_8bit_draw(context, draw_area, map_offset, glyph, 0);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 4-bit compressed glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 4-bit compressed glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area,
                                                        GX_POINT * map_offset, const GX_GLYPH * glyph)
{
    d2_u32      mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *)glyph;
    if(compressed_glyph -> gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_rotated_glyph_4bit_draw(context, draw_area, map_offset, glyph, mode);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 4-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 4-bit raw glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph)
{
    gx_dave2d_rotated_glyph_4bit_draw(context, draw_area, map_offset, glyph, 0);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 1-bit compressed glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 1-bit compressed glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_compressed_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area,
                                                        GX_POINT * map_offset, const GX_GLYPH * glyph)
{
    d2_u32      mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *)glyph;
    if(compressed_glyph -> gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_rotated_glyph_1bit_draw(context, draw_area, map_offset, glyph, mode);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 1-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 1-bit raw glyph for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_raw_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph)
{
    gx_dave2d_rotated_glyph_1bit_draw(context, draw_area, map_offset, glyph, 0);
}

#if defined(GX_ARC_DRAWING_SUPPORT)
/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, anti-aliased circle outline draw function with D/AVE 2D acceleration
 * enabled. This function is called by GUIX to render anti-aliased circle outline for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    /** Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    if (r < (UINT) ((brush->gx_brush_width + 1)/2))
    {
        r = 0U;
    }
    else
    {
        r = (UINT)(r - (UINT)((brush->gx_brush_width + 1) / 2));
    }

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S */
    gx_dave2d_outline_width_set(dave,GX_FIXED_VAL_MAKE(brush->gx_brush_width));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S */ /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
        /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                            (d2_point)(D2_FIX4((USHORT)ycenter)),
                                            (d2_width)(D2_FIX4((USHORT)r)),
                                            0))
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, aliased circle outline draw function with D/AVE 2D acceleration
 * enabled. This function is called by GUIX to render aliased circle outline for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    /** Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    if (r < (UINT)((brush->gx_brush_width + 1) / 2))
     {
        r = 0U;
    }
    else
    {
        r = (UINT)(r - (UINT)((brush->gx_brush_width + 1) / 2));
    }

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S */
    gx_dave2d_outline_width_set(dave, GX_FIXED_VAL_MAKE(brush->gx_brush_width));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);
 
    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
        /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                            (d2_point)(D2_FIX4((USHORT)ycenter)),
                                            (d2_width)(D2_FIX4((USHORT)r)),
                                            0))
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, circle fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill circle for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_circle_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;
    GX_PIXELMAP *map = brush->gx_brush_pixelmap;
    GX_COLOR brush_color = brush->gx_brush_fill_color;

    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);

    INT xoffset = 0;
    INT yoffset = 0;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    /** Enable or Disable anti-aliasing based on the brush style set. */
    if(brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);
    
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
        /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            /** If rotation angle is Clock-Wise, define yoffset */
            /*LDRA_INSPECTED 330 S *//*LDRA_INSPECTED 96 S */
            yoffset = (INT)(((2 * (INT) r) + 1) % ((INT)(map->gx_pixelmap_width)));
            if(yoffset)
            {
                yoffset = ((INT)(map->gx_pixelmap_width)) - yoffset;
            }
        }
        else
        {
            /** Else, define xoffset */
            /*LDRA_INSPECTED 330 S *//*LDRA_INSPECTED 96 S */
            xoffset = (INT)(((2 * (INT) r) + 1) % ((INT)(map->gx_pixelmap_height)));
            if(xoffset)
            {
                xoffset = ((INT)(map->gx_pixelmap_height)) - xoffset;
            }
        }
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);
        gx_dave2d_set_rotated_texture(context, dave,
                xcenter - (INT)r - xoffset,
                ycenter - (INT)r - yoffset,
                map);
    }
    else
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_color);
        gx_dave2d_color0_set(dave, brush_color);
    }
    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                            (d2_point)(D2_FIX4((USHORT)ycenter)),
                                            (d2_width)(D2_FIX4((USHORT)r)),
                                            0))
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, pie fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill pie for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_pie_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                               INT end_angle)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    INT sin1;
    INT cos1;
    INT sin2;
    INT cos2;
    d2_u32 flags;
    d2_device *dave = gx_dave2d_rotated_context_clip_set(context);
    INT xoffset = 0;
    INT yoffset = 0;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    INT s_angle =  - (start_angle + (INT)(context->gx_draw_context_display->gx_display_rotation_angle));
    INT e_angle =  - (end_angle + (INT)(context->gx_draw_context_display->gx_display_rotation_angle));

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_wf_concave. */
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    /** Enable or Disable anti-aliasing based on the brush style set. */
    if(brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT)(context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
        /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT)(context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            /** If rotation angle is Clock-Wise, define yoffset */
            yoffset = ((2 * (INT) r) + 1) % (brush->gx_brush_pixelmap->gx_pixelmap_width);
            if(yoffset)
            {
                yoffset = ((INT)(brush->gx_brush_pixelmap->gx_pixelmap_width)) - yoffset;
            }
        }
        else
        {
            /** Else, define xoffset */
            xoffset = ((2 * (INT) r) + 1) % (brush->gx_brush_pixelmap->gx_pixelmap_height);
            if(xoffset)
            {
                xoffset = ((INT)(brush->gx_brush_pixelmap->gx_pixelmap_height)) - xoffset;
            }
        }
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);
        gx_dave2d_set_rotated_texture(context, dave,
                xcenter - (INT)r - xoffset,
                ycenter - (INT)r - yoffset,
                brush->gx_brush_pixelmap);
    }
    else
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_color);
        gx_dave2d_color0_set(dave, brush->gx_brush_fill_color);
    }
    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                           (d2_point)(D2_FIX4((USHORT)ycenter)),
                                           (d2_width)(D2_FIX4((USHORT)(r + 1))),
                                           0,
                                           (d2_s32)((UINT)cos1 << 6),
                                           (d2_s32)((UINT)sin1 << 6),
                                           (d2_s32)((UINT)cos2 << 6),
                                           (d2_s32)((UINT)sin2 << 6),
                                           flags))
    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, anti-aliased arc draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw anti-aliased arc for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                                        INT end_angle)
{
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    GX_BRUSH   *brush;
    d2_device  *dave;


    brush = &context->gx_draw_context_brush;

    if(brush->gx_brush_width < 1)
    {
        return;
    }

    dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = 0U;
    brush_alpha = brush->gx_brush_alpha;

    if(0U == brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    start_angle += (INT)(context->gx_draw_context_display->gx_display_rotation_angle);
    end_angle += (INT)(context->gx_draw_context_display->gx_display_rotation_angle);

    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);
    
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
        /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    INT s_angle =  - start_angle;
    INT e_angle =  - end_angle;

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_wf_concave. */
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave,
                               (GX_FIXED_VAL) ((ULONG) ((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT) >> 1));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                           (d2_point)(D2_FIX4((USHORT)ycenter)),
                                           (d2_width)(D2_FIX4((USHORT)r)),
                                           0,
                                           (d2_s32)((UINT)cos1 << 6),
                                           (d2_s32)((UINT)sin1 << 6),
                                           (d2_s32)((UINT)cos2 << 6),
                                           (d2_s32)((UINT)sin2 << 6),
                                           flags))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /** Check if line connection style is circle segment. */
    if (brush -> gx_brush_style & GX_BRUSH_ROUND)
    {
        UINT brush_width = (UINT)brush->gx_brush_width;
        GX_POINT  startp;
        GX_POINT  endp;

        endp.gx_point_x = (GX_VALUE) 0;
        endp.gx_point_y = (GX_VALUE) 0;

        startp.gx_point_x = (GX_VALUE) 0;
        startp.gx_point_y = (GX_VALUE) 0;

        /** Set a rendering mode to solid. */
        gx_dave2d_render_mode_set(dave, d2_rm_solid);

        r = (UINT)(r - (UINT)(brush_width >> 1));
        
        /** Get the point on circle with specified angle and radius. */
        gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &startp);
        gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)brush_width, start_angle, &endp);

        /** Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width)(D2_FIX4(brush_width) >> 1),
                                          0))


        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);

        /** Get the point on circle with specified angle and radius. */
        gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &startp);
        gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)brush_width, end_angle, &endp);

        /** Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width)(D2_FIX4(brush_width) >> 1),
                                          0))

        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, arc draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw arc for 90 and 270 degree screen rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                              INT end_angle)
{
    GX_BRUSH   *brush = &context->gx_draw_context_brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    d2_device  *dave;

    if(brush->gx_brush_width < 1)
    {
        return;
    }
    dave = gx_dave2d_rotated_context_clip_set(context);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = 0U;

    brush_alpha = brush->gx_brush_alpha;
    if(0 == (INT) brush_alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, brush_alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    start_angle += (INT) (context->gx_draw_context_display->gx_display_rotation_angle);
    end_angle += (INT) (context->gx_draw_context_display->gx_display_rotation_angle);

    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xcenter, ycenter);
    
    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        /** If rotation angle is Clock-Wise, define new center pixel position in vertical axis */
        ycenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_x_resolution)) - ycenter - 1;
    }
    else
    {
       /** Else, define new center pixel position in horizontal axis */
        xcenter = ((INT) (context->gx_draw_context_canvas->gx_canvas_y_resolution)) - xcenter - 1;
    }

    INT s_angle =  - start_angle;
    INT e_angle =  - end_angle;

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL)((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D have use of mixed mode arithmetic for the macro d2_wf_concave. */
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave,
                               (GX_FIXED_VAL) ((ULONG) ((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT) >> 1));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                           (d2_point)(D2_FIX4((USHORT)ycenter)),
                                           (d2_width)(D2_FIX4((USHORT)r)),
                                           0,
                                           (d2_s32)((UINT)cos1 << 6),
                                           (d2_s32)((UINT)sin1 << 6),
                                           (d2_s32)((UINT)cos2 << 6),
                                           (d2_s32)((UINT)sin2 << 6),
                                           flags))

    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /** Check if line connection style is circle segment. */
    if (brush -> gx_brush_style & GX_BRUSH_ROUND)
    {
        UINT brush_width = (UINT)brush->gx_brush_width;
        GX_POINT  startp;
        GX_POINT  endp;

        startp.gx_point_x = (GX_VALUE) 0;
        startp.gx_point_y = (GX_VALUE) 0;

        endp.gx_point_x = (GX_VALUE) 0;
        endp.gx_point_y = (GX_VALUE) 0;

        gx_dave2d_render_mode_set(dave, d2_rm_solid);

        r = (UINT)(r - (UINT)(brush_width >> 1));

        /** Get the point on circle with specified angle and radius. */
        gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &startp);
        gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)brush_width, start_angle, &endp);

        /** Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width)(D2_FIX4(brush_width) >> 1),
                                          0))

        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);

        /** Get the point on circle with specified angle and radius. */
        gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &startp);
        gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)brush_width, end_angle, &endp);

        /** Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point)(D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width)(D2_FIX4(brush_width) >> 1),
                                          0))

        /** Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, arc fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill arc for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, arc fill is done by GUIX generic software draw routine because chord 
 * (filled arc) operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_arc_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                               INT end_angle)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);

    /** Call the GUIX generic arc fill routine. */
    _gx_display_driver_generic_rotated_arc_fill(context, xcenter, ycenter, r, start_angle, end_angle);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, anti-aliased ellipse draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw aliased ellipse for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, ellipse draw is done by GUIX generic software draw routine because the 
 * operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_aliased_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    if(context->gx_draw_context_brush.gx_brush_width > 1)
    {
        /** Call the GUIX generic anti-aliased ellipse draw routine. */
        _gx_display_driver_generic_aliased_wide_ellipse_draw(context, xcenter, ycenter, a, b);
    }
    else
    {
        /** Call the GUIX generic anti-aliased ellipse draw routine. */
        _gx_display_driver_generic_aliased_ellipse_draw(context, xcenter, ycenter, a, b);
    }

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, aliased ellipse draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw aliased ellipse for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, ellipse draw is done by GUIX generic software draw routine because the 
 * operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    if(context->gx_draw_context_brush.gx_brush_width > 1)
    {
        /** Call the GUIX generic aliased ellipse draw routine if brush width is more than 1. */
        _gx_display_driver_generic_wide_ellipse_draw(context, xcenter, ycenter, a, b);
    }
    else
    {
        /** Call the GUIX generic aliased ellipse draw routine if brush width is not more than 1. */
        _gx_display_driver_generic_ellipse_draw(context, xcenter, ycenter, a, b);
    }

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, ellipse fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to perform ellipse fill for 90 and 270 degree screen rotation. 
 * Although D/AVE 2D acceleration enabled, ellipse fill is done by GUIX generic software draw routine.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_ellipse_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);

    /** Call the GUIX generic ellipse fill routine. */
    _gx_display_driver_generic_rotated_ellipse_fill(context, xcenter, ycenter, a, b);
}

#endif  /* is GUIX arc drawing support enabled? */

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, D/AVE 2D accelerated draw function to toggle frame buffer.
 *  This function performs copies canvas memory to working frame buffer if a canvas is used, performs sequence of canvas
 *  refresh drawing commands, toggles frame buffer, and finally copies visible frame buffer to working frame buffer or
 *  copies canvas to working buffer if a canvas is used. This function is called by GUIX if D/AVE 2D hardware rendering
 *  acceleration is enabled and screen rotation is 90 or 270 degree.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   dirty[in]          Pointer to a dirty rectangle area
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_rotated_buffer_toggle(GX_CANVAS * canvas, GX_RECTANGLE * dirty)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE Copy;
    GX_DISPLAY *display;
    R_G2D_Type *p_reg;

    Copy.gx_rectangle_bottom = (GX_VALUE) 0;
    Copy.gx_rectangle_left = (GX_VALUE) 0;
    Copy.gx_rectangle_right = (GX_VALUE) 0;
    Copy.gx_rectangle_top = (GX_VALUE) 0;

    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = 0U;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_DRW;
    fmi_feature_info_t info = {0U};
    g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    p_reg = (R_G2D_Type *) info.ptr;

    display = canvas->gx_canvas_display;

    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    _gx_utility_rectangle_define(&Limit, 0, 0,
                                (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
                                (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    gx_display_list_flush(display);
    gx_display_list_open(display);

    /* Wait till framebuffer writeback is busy. */
    while ((1U == p_reg->STATUS_b.BUSYWRITE) || (1U == p_reg->STATUS_b.CACHEDIRTY))
    {
        ;
    }

    sf_el_gx_frame_toggle(display->gx_display_handle, (GX_UBYTE **) &visible_frame);
    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    /** If canvas memory is pointing directly to frame buffer, toggle canvas memory. */
    if (canvas->gx_canvas_memory == (GX_COLOR *) visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        /** Copies canvas memory or visible frame buffer to working frame buffer. */
        gx_dave2d_rotated_copy_to_working(canvas, &Copy, (ULONG *)visible_frame);
    }
}
#endif  /* GX_USE_SYNERGY_DRW */

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_EL_GX)
 **********************************************************************************************************************/

#if (GX_USE_SYNERGY_DRW == 1)

d2_device *gx_dave2d_rotated_context_clip_set(GX_DRAW_CONTEXT *context)
{
    d2_device *dave = context -> gx_draw_context_display -> gx_display_accelerator;
    GX_RECTANGLE *clip;
    GX_CANVAS *canvas;

    if (context -> gx_draw_context_clip)
    {
        clip = context->gx_draw_context_clip;
        canvas = context -> gx_draw_context_canvas;

        if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
        {
            CHECK_DAVE_STATUS(d2_cliprect(dave,
                        clip -> gx_rectangle_top,
                        (d2_border)(canvas -> gx_canvas_x_resolution - clip -> gx_rectangle_right - 1),
                        clip -> gx_rectangle_bottom,
                        (d2_border)(canvas -> gx_canvas_x_resolution - clip -> gx_rectangle_left - 1)))
        }
        else
        {
            CHECK_DAVE_STATUS(d2_cliprect(dave,
                        (d2_border)(canvas -> gx_canvas_y_resolution - clip -> gx_rectangle_bottom - 1),
                        clip->gx_rectangle_left,
                        (d2_border)(canvas -> gx_canvas_y_resolution - clip -> gx_rectangle_top - 1),
                        clip -> gx_rectangle_right))
        }
    }
    else
    {
        CHECK_DAVE_STATUS(d2_cliprect(dave, 0, 0,
                                      (d2_border)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution - 1),
                                      (d2_border)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution - 1)))
    }
    return dave;
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Support function used to apply texture source for all shape drawing.
 * This function is called by GUIX to draw a polygon for 90 and 270 degree rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   dave[in]            Pointer to D/AVE 2D device context
 * @param   xpos[in]            X position in pixel unit
 * @param   ypos[in]            y position in pixel unit
 * @param   map[in]             Pointer to GUIX pixelmap
 **********************************************************************************************************************/
static VOID gx_dave2d_set_rotated_texture(GX_DRAW_CONTEXT *context, d2_device *dave, INT xpos, INT ypos,
                                                                                      GX_PIXELMAP * map)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(context);

    d2_u32 format = gx_dave2d_format_set(map);
    d2_u8 amode = d2_to_one;

    if ((format & (d2_u32)d2_mode_clut) == (d2_u32)d2_mode_clut)
    {
        gx_dave2d_pixelmap_palette_set(dave, map);
    }

    if (map->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        format |= d2_mode_rle;
    }

    if(map->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        amode = d2_to_copy;
    }

    CHECK_DAVE_STATUS(d2_settexture(dave,
            (void *) map->gx_pixelmap_data,
            map->gx_pixelmap_height,
            map->gx_pixelmap_height,
            map->gx_pixelmap_width,
            format))

    /* Texture wrapping will work with texture dimensions that are integer powers of two only. */
    /** Set texture mode. */
    CHECK_DAVE_STATUS(d2_settexturemode(dave, d2_tm_wrapu|d2_tm_wrapv))

    CHECK_DAVE_STATUS(d2_settextureoperation(dave, amode, d2_to_copy, d2_to_copy, d2_to_copy))
    CHECK_DAVE_STATUS(d2_settexelcenter(dave, 0, 0))
    CHECK_DAVE_STATUS(d2_settexturemapping(dave,
                                           (d2_point)((USHORT)xpos << 4),
                                           (d2_point)((USHORT)ypos << 4),
                                           0,
                                           0,
                                           (d2_s32)(1U << 16),
                                           0,
                                           0,
                                           (d2_s32)(1U << 16)))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8-bit glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by _gx_dave2d_compressed_glyph_8bit_draw() or _gx_dave2d_raw_glyph_8bit_draw() for 90 
 * and 270 degree rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 * @param   mode[in]           D/AVE 2D render mode option (none('0') or d2_mode_rle)
 **********************************************************************************************************************/
static VOID gx_dave2d_rotated_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT * map_offset,
                                                                                const GX_GLYPH * glyph, d2_u32 mode)
{
    d2_device  *dave;
    GX_COLOR    text_color;
    GX_RECTANGLE clip;
    GX_POINT rotated_offset;
    INT width;

    /* pickup pointer to current display driver */

    dave = context -> gx_draw_context_display -> gx_display_accelerator;

    text_color =  context -> gx_draw_context_brush.gx_brush_line_color;

#if defined (GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE    alpha = 0U;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        clip.gx_rectangle_left = draw_area->gx_rectangle_top;
        clip.gx_rectangle_right = draw_area->gx_rectangle_bottom;
        clip.gx_rectangle_top = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                  draw_area->gx_rectangle_right - 1);
        clip.gx_rectangle_bottom = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                   draw_area->gx_rectangle_left - 1);

        width = ((draw_area->gx_rectangle_right) - (draw_area->gx_rectangle_left)) + 1;

        rotated_offset.gx_point_x = map_offset->gx_point_y;
        rotated_offset.gx_point_y = (GX_VALUE)(glyph -> gx_glyph_width - map_offset -> gx_point_x - (UCHAR)width);
    }
    else
    {
        clip.gx_rectangle_left = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                  draw_area->gx_rectangle_bottom - 1);
        clip.gx_rectangle_right = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                     draw_area->gx_rectangle_top - 1);
        clip.gx_rectangle_top = draw_area->gx_rectangle_left;
        clip.gx_rectangle_bottom = draw_area->gx_rectangle_right;

        width = (draw_area->gx_rectangle_bottom - draw_area->gx_rectangle_top) + 1;
        rotated_offset.gx_point_x = (GX_VALUE)(glyph -> gx_glyph_height - map_offset -> gx_point_y - (UCHAR)width);
        rotated_offset.gx_point_y = map_offset->gx_point_x;
    }

    gx_dave2d_cliprect_set(dave, &clip);
    gx_dave2d_color0_set(dave, text_color);
    if(alpha < 255U)
    {
        /* Enable alpha blend mode */
        gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
    }
    else
    {
        /* Disable alpha blend mode */
        gx_dave2d_blend_mode_set(dave, d2_bm_one, d2_bm_one_minus_alpha);
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)glyph -> gx_glyph_map,
                           glyph -> gx_glyph_height, glyph -> gx_glyph_height,
                           glyph -> gx_glyph_width,
                           /*LDRA_INSPECTED 120 S */
                           mode|d2_mode_alpha8))

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         glyph -> gx_glyph_height,
                         glyph -> gx_glyph_width,
                         (d2_blitpos)(rotated_offset.gx_point_x),
                         (d2_blitpos)(rotated_offset.gx_point_y),
                         (d2_width)(D2_FIX4((USHORT)(glyph -> gx_glyph_height))),
                         (d2_width)(D2_FIX4((USHORT)(glyph -> gx_glyph_width))),
                         (d2_point)(D2_FIX4((USHORT)(clip.gx_rectangle_left))),
                         (d2_point)(D2_FIX4((USHORT)(clip.gx_rectangle_top))),
                         /*LDRA_INSPECTED 120 S */
                         d2_bf_usealpha | d2_bf_colorize))

     /** Count the used display list size. */
     gx_dave2d_display_list_count(context->gx_draw_context_display);

    /* Return back alpha blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 4-bit glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by _gx_dave2d_compressed_glyph_4bit_draw() or _gx_dave2d_raw_glyph_4bit_draw() for 90 and 
 * 270 degree rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 * @param   mode[in]           D/AVE 2D render mode option (none('0') or d2_mode_rle)
 **********************************************************************************************************************/
static VOID gx_dave2d_rotated_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT * map_offset,
                                                                                const GX_GLYPH * glyph, d2_u32 mode)
{
    d2_device  *dave;
    GX_COLOR    text_color;
    GX_RECTANGLE clip;
    GX_POINT     rotated_offset;
    INT          width;

    text_color =  context -> gx_draw_context_brush.gx_brush_line_color;
    dave = context -> gx_draw_context_display -> gx_display_accelerator;

#if defined (GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE    alpha = 0U;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        clip.gx_rectangle_left = draw_area->gx_rectangle_top;
        clip.gx_rectangle_right = draw_area->gx_rectangle_bottom;
        clip.gx_rectangle_top = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                  draw_area->gx_rectangle_right - 1);
        clip.gx_rectangle_bottom = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                     draw_area->gx_rectangle_left - 1);

        width = ((draw_area->gx_rectangle_right) - (draw_area->gx_rectangle_left)) + 1;
        rotated_offset.gx_point_x = map_offset->gx_point_y;
        rotated_offset.gx_point_y = (GX_VALUE)(glyph -> gx_glyph_width - map_offset -> gx_point_x - (SHORT)width);
    }
    else
    {
        clip.gx_rectangle_left = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                 draw_area->gx_rectangle_bottom - 1);
        clip.gx_rectangle_right = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                     draw_area->gx_rectangle_top - 1);
        clip.gx_rectangle_top = draw_area->gx_rectangle_left;
        clip.gx_rectangle_bottom = draw_area->gx_rectangle_right;

        width = ((draw_area->gx_rectangle_bottom) - (draw_area->gx_rectangle_top)) + 1;

        rotated_offset.gx_point_x = (GX_VALUE)(glyph -> gx_glyph_height - map_offset -> gx_point_y - (SHORT)width);
        rotated_offset.gx_point_y = map_offset->gx_point_x;
    }

    gx_dave2d_cliprect_set(dave, &clip);

#if 1
    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
    gx_dave2d_glyph_palette_set(dave, GX_FONT_FORMAT_4BPP);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)glyph -> gx_glyph_map,
                           (d2_s32)(((USHORT)glyph -> gx_glyph_height + 1U) & 0xfffeU),
                           (d2_s32)glyph -> gx_glyph_height,
                           (d2_s32)glyph -> gx_glyph_width,
                           (mode | d2_mode_i4 | d2_mode_clut)))

#else
    /* This version uses d2_mode_alpha4 instead of setting up a 16-entry palette,
     * however this blitsrc mode doesn't seem to be supported by D/AVE2D, even
     * though it is documented. Leave this in place in case this supported is
     * added in future versions.
     */

    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_one, d2_bm_one_minus_alpha);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)glyph -> gx_glyph_map,
                           (d2_s32)(((USHORT)glyph -> gx_glyph_width + 1U) & 0xfffeU),
                           glyph -> gx_glyph_width,
                           glyph -> gx_glyph_height,
                           mode|d2_mode_alpha4))
 #endif

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         glyph -> gx_glyph_height,
                         glyph -> gx_glyph_width,
                         0,
                         0,
                         (d2_width)(D2_FIX4((USHORT)(glyph -> gx_glyph_height))),
                         (d2_width)(D2_FIX4((USHORT)(glyph -> gx_glyph_width))),
                         (d2_point)(D2_FIX4((USHORT)clip.gx_rectangle_left - (USHORT)rotated_offset.gx_point_x)),
                         (d2_point)(D2_FIX4((USHORT)clip.gx_rectangle_top - (USHORT)rotated_offset.gx_point_y)),
                         (d2_u32)d2_bf_colorize | (d2_u32)d2_bf_usealpha))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /** Set blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 1-bit glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by _gx_dave2d_compressed_glyph_1bit_draw() or _gx_dave2d_raw_glyph_1bit_draw() for 90 and 
 * 270 degree rotation.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 * @param   mode[in]           D/AVE 2D render mode option (none('0') or d2_mode_rle)
 **********************************************************************************************************************/
static VOID gx_dave2d_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset,
                                       const GX_GLYPH *glyph, d2_u32 mode)
{
    d2_device  *dave;
    GX_COLOR    text_color;
    GX_RECTANGLE clip;
    GX_POINT     rotated_offset;
    INT          width;

    /** Pickup pointer to current display driver */
    dave = context -> gx_draw_context_display -> gx_display_accelerator;
    text_color =  context -> gx_draw_context_brush.gx_brush_line_color;

#if defined (GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE    alpha;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if(0U == alpha)
    {
        return;
    }
    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        clip.gx_rectangle_left = draw_area->gx_rectangle_top;
        clip.gx_rectangle_right = draw_area->gx_rectangle_bottom;
        clip.gx_rectangle_top = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                  draw_area->gx_rectangle_right - 1);
        clip.gx_rectangle_bottom = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_x_resolution -
                                                                      draw_area->gx_rectangle_left - 1);

        width = ((draw_area->gx_rectangle_right) - (draw_area->gx_rectangle_left)) + 1;
        rotated_offset.gx_point_x = map_offset->gx_point_y;
        rotated_offset.gx_point_y = (GX_VALUE)(glyph -> gx_glyph_width - map_offset -> gx_point_x - (SHORT)width);
    }
    else
    {
        clip.gx_rectangle_left = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                  draw_area->gx_rectangle_bottom - 1);
        clip.gx_rectangle_right = (GX_VALUE)(context->gx_draw_context_canvas->gx_canvas_y_resolution -
                                                                     draw_area->gx_rectangle_top - 1);
        clip.gx_rectangle_top = draw_area->gx_rectangle_left;
        clip.gx_rectangle_bottom = draw_area->gx_rectangle_right;

        width = ((draw_area->gx_rectangle_bottom) - (draw_area->gx_rectangle_top)) + 1;
        rotated_offset.gx_point_x = (GX_VALUE)(glyph -> gx_glyph_height - map_offset -> gx_point_y - (SHORT)width);
        rotated_offset.gx_point_y = map_offset->gx_point_x;
    }

    gx_dave2d_cliprect_set(dave, &clip);

#if 1
    gx_dave2d_glyph_palette_set(dave, GX_FONT_FORMAT_1BPP);
    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)glyph -> gx_glyph_map,
                          (d2_s32)(((USHORT)glyph -> gx_glyph_height + 7U) & 0xfff8U),
                          (d2_s32)glyph -> gx_glyph_height,
                          (d2_s32)glyph -> gx_glyph_width,
                          (mode | d2_mode_i1 | d2_mode_clut)))
#else
    gx_dave2d_color0_set(dave, text_color);
    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)glyph -> gx_glyph_map,
                           glyph -> gx_glyph_width, glyph -> gx_glyph_width,
                           glyph -> gx_glyph_height,
                           mode|d2_mode_alpha1))
#endif

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         glyph -> gx_glyph_height,
                         glyph -> gx_glyph_width,
                         0,
                         0,
                         (d2_width)(D2_FIX4((USHORT)glyph -> gx_glyph_height)),
                         (d2_width)(D2_FIX4((USHORT)glyph -> gx_glyph_width)),
                         (d2_point)(D2_FIX4((USHORT)clip.gx_rectangle_left - (USHORT)rotated_offset.gx_point_x)),
                         (d2_point)(D2_FIX4((USHORT)clip.gx_rectangle_top - (USHORT)rotated_offset.gx_point_y)),
                         /*LDRA_INSPECTED 120 S */
                         d2_bf_usealpha | d2_bf_colorize))

    /** Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /** Set blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, D/AVE 2D draw function sub routine to copy visible frame buffer to working
 * frame buffer. This function is called by _gx_dave2d_buffer_toggle() to perform image data copy between frame buffers
 * after buffer toggle operation for 90 and 270 degree rotation.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   copy[in]           Pointer to a rectangle area to be copied
 * @param   src[in]            Pointer to canvas memory
 **********************************************************************************************************************/
static VOID gx_dave2d_rotated_copy_to_working(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;
    d2_u32 mode;

    ULONG        *pGetRow;
    ULONG        *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          srcx;
    INT          srcy;
    INT          offsetx;
    INT          offsety;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    d2_device *dave = (d2_device *) display->gx_display_accelerator;

    copy_clip = *copy;

    if (GX_COLOR_FORMAT_565RGB == (INT) (display->gx_display_color_format))
    {
        mode = d2_mode_rgb565;
    }
    else
    {
        mode = d2_mode_argb8888;
    }

    if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
    {
        srcx = copy_clip.gx_rectangle_top;
        srcy = canvas->gx_canvas_x_resolution - copy_clip.gx_rectangle_right - 1;

        offsetx = canvas->gx_canvas_display_offset_y;
        offsety = display->gx_display_width - canvas->gx_canvas_x_resolution - canvas->gx_canvas_display_offset_x;

        copy_clip.gx_rectangle_left = copy->gx_rectangle_top;
        copy_clip.gx_rectangle_top = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_right - 1);
        copy_clip.gx_rectangle_right = copy->gx_rectangle_bottom;
        copy_clip.gx_rectangle_bottom = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_left - 1);
    }
    else
    {
        srcx = canvas->gx_canvas_y_resolution - copy_clip.gx_rectangle_bottom - 1;
        srcy = copy_clip.gx_rectangle_left;

        offsetx = display->gx_display_height - canvas->gx_canvas_y_resolution - canvas->gx_canvas_display_offset_y;
        offsety = canvas->gx_canvas_display_offset_x;

        copy_clip.gx_rectangle_top = copy->gx_rectangle_left;
        copy_clip.gx_rectangle_bottom = copy->gx_rectangle_right;
        copy_clip.gx_rectangle_left = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_bottom - 1);
        copy_clip.gx_rectangle_right = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_top - 1);
    }

    /** Offset canvas within frame buffer */
    _gx_utility_rectangle_define(&display_size, 0, 0,
            (GX_VALUE)(display->gx_display_height - 1),
            (GX_VALUE)(display->gx_display_width - 1));

    _gx_utility_rectangle_shift(&copy_clip, (GX_VALUE)offsetx, (GX_VALUE)offsety);
    _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);

    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) ||
        (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) src;
    pPutRow = (ULONG *) working_frame;

    CHECK_DAVE_STATUS(d2_framebuffer(dave, pPutRow,
                                     (d2_s32)(display->gx_display_height),
                                     (d2_u32)(display->gx_display_height),
                                     (d2_u32)(display->gx_display_width),
                                     (d2_s32)mode))

    gx_dave2d_cliprect_set(dave, &copy_clip);
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pGetRow,
                  canvas->gx_canvas_y_resolution,
                  canvas->gx_canvas_y_resolution,
                  canvas->gx_canvas_x_resolution,
                  mode))

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                copy_width,
                copy_height,
                (d2_blitpos)(srcx),
                (d2_blitpos)(srcy),
                (d2_width)(D2_FIX4((UINT)copy_width)),
                (d2_width)(D2_FIX4((UINT)copy_height)),
                (d2_point)(D2_FIX4((USHORT)copy_clip.gx_rectangle_left)),
                (d2_point)(D2_FIX4((USHORT)copy_clip.gx_rectangle_top)),
                d2_bf_no_blitctxbackup))

    CHECK_DAVE_STATUS(d2_endframe(display -> gx_display_accelerator))
    CHECK_DAVE_STATUS(d2_startframe(display -> gx_display_accelerator))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 16bpp pixelmap rorate function with 90/180/270 degree.
 * This function is called by _gx_dave2d_rotated_pixelmap_rotate_16bpp() for 90 and 270 degree rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x-coordinate of top-left draw point in pixel unit
 * @param   ypos[in]            y-coordinate of top-left draw point in pixel unit
 * @param   pixelmap[in]        Pointer to a GX_PIXELMAP structure
 * @param   angle[in]           The angle to rotate (90, 180, or 270)
 * @param   cx[in]              x-coordinate of rotating center
 * @param   cy[in]              y-coordinate of rotating center
 * @note    This function can treat simple rotation case but ignores any other angle described above is specified.
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context,
                                            INT xpos, INT ypos, GX_PIXELMAP *pixelmap, INT angle, INT cx, INT cy)
{
    GX_COLOR     *putrow;
    GX_COLOR     *get;
    INT           width;
    INT           height;
    GX_COLOR      pixel;
    GX_UBYTE      alpha;
    INT           x;
    INT           y;
    GX_RECTANGLE *clip;
    GX_RECTANGLE  rotated_clip;
    INT           newxpos;
    INT           newypos;
    VOID(*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = _gx_display_driver_565rgb_pixel_blend;

    clip = context->gx_draw_context_clip;

    /** Swap x-axis and y-axis positions of a pixelmap */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xpos, ypos);
    
    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(cx, cy);

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        ypos = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution)) - ypos -
                                                         (INT)pixelmap -> gx_pixelmap_width;
        cy = ((INT)(pixelmap -> gx_pixelmap_width)) - cy - 1;

        rotated_clip.gx_rectangle_left = clip -> gx_rectangle_top;
        rotated_clip.gx_rectangle_right = clip -> gx_rectangle_bottom;
        rotated_clip.gx_rectangle_top = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution - 1 -
                                                                                         clip -> gx_rectangle_right);
        rotated_clip.gx_rectangle_bottom = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution - 1 -
                                                                                           clip -> gx_rectangle_left);
    }
    else
    {
        xpos = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution)) - xpos -
                                                         (INT)pixelmap -> gx_pixelmap_height;
        cx = ((INT)(pixelmap -> gx_pixelmap_height)) - cx - 1;

        rotated_clip.gx_rectangle_left = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution - 1 -
                                                                                         clip -> gx_rectangle_bottom);
        rotated_clip.gx_rectangle_right = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution - 1 -
                                                                                            clip -> gx_rectangle_top);
        rotated_clip.gx_rectangle_top = clip -> gx_rectangle_left;
        rotated_clip.gx_rectangle_bottom = clip -> gx_rectangle_right;
    }

    if (90 == angle)
    {
        width = pixelmap -> gx_pixelmap_width;
        height = pixelmap -> gx_pixelmap_height;

        newxpos = (xpos + cx) - ((width - 1) - cy);
        newypos = (ypos + cy) - cx;

        for (y = ((INT)rotated_clip.gx_rectangle_top - newypos); y <= ((INT)rotated_clip.gx_rectangle_bottom - newypos);
                                                                                                                   y++)
        {
            for (x = ((INT)rotated_clip.gx_rectangle_left - newxpos); x <= ((INT)rotated_clip.gx_rectangle_right -
                                                                                                     newxpos); x++)
            {
                get = (GX_COLOR *)pixelmap->gx_pixelmap_data;
                get += (width - 1 - x) * height;
                get += y;
                alpha = (GX_UBYTE)((*get) >> 24);
                pixel = ((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3);
                blend_func(context, x + newxpos, y + newypos, pixel, alpha);
            }
        }
    }
    else if (180 == angle)
    {

        width = pixelmap->gx_pixelmap_height;
        height = pixelmap->gx_pixelmap_width;

        newxpos = (xpos + cx) - ((width - 1) - cx);
        newypos = (ypos + cy) - ((height - 1) - cy);

        putrow = context->gx_draw_context_memory;
        putrow += rotated_clip.gx_rectangle_top * context->gx_draw_context_pitch;
        putrow += rotated_clip.gx_rectangle_left;

        for (y = ((INT)rotated_clip.gx_rectangle_top - newypos); y <= ((INT)rotated_clip.gx_rectangle_bottom -
                                                                                                newypos); y++)
        {
            for (x = ((INT)rotated_clip.gx_rectangle_left - newxpos); x <= ((INT)rotated_clip.gx_rectangle_right -
                                                                                                    newxpos); x++)
            {
                get = (GX_COLOR *)pixelmap->gx_pixelmap_data;
                get += (height - 1 - y) * width;
                get += (width - 1) - x;
                alpha = (GX_UBYTE)((*get) >> 24);
                pixel = ((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3);
                blend_func(context, x + newxpos, y + newypos, pixel, alpha);
            }
        }
    }
    else if (270 == angle)
    {

        width = pixelmap->gx_pixelmap_width;
        height = pixelmap->gx_pixelmap_height;

        newxpos = (xpos + cx) - cy;
        newypos = (ypos + cx) - ((height - 1) - cy);

        putrow = context->gx_draw_context_memory;
        putrow += rotated_clip.gx_rectangle_top * context->gx_draw_context_pitch;
        putrow += rotated_clip.gx_rectangle_left;

        for (y = ((INT)rotated_clip.gx_rectangle_top - newypos); y <= ((INT)rotated_clip.gx_rectangle_bottom -
                                                                                                newypos); y++)
        {
            for (x = ((INT)rotated_clip.gx_rectangle_left - newxpos); x <= ((INT)rotated_clip.gx_rectangle_right -
                                                                                                   newxpos); x++)
            {
                get = (GX_COLOR *)pixelmap->gx_pixelmap_data;
                get += x * height;
                get += (height - 1) - y;
                alpha = (GX_UBYTE)((*get) >> 24);
                pixel = ((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3);
                blend_func(context, x + newxpos, y + newypos, pixel, alpha);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief  Subroutine of _gx_dave2d_rotated_pixelmap_rotate_16bpp(). This function rotates 32ARGB pixelmap 
 * with arbitrary degree for 16bpp pixelmap draw routine and with 90 or 270 degree rotation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x-coordinate of top-left draw point in pixel unit
 * @param   ypos[in]            y-coordinate of top-left draw point in pixel unit
 * @param   pixelmap[in]        Pointer to a GX_PIXELMAP structure
 * @param   angle[in]           The angle to rotate
 * @param   cx[in]              x-coordinate of rotating center
 * @param   cy[in]              y-coordinate of rotating center
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos,
                                                    INT ypos, GX_PIXELMAP *pixelmap, INT angle, INT cx, INT cy)
{
    INT             srcxres;
    INT             srcyres;
    INT             newxpos;
    INT             newypos;
    INT             cosv;
    INT             sinv;
    INT             xres;
    INT             yres;
    INT             idxminx;
    INT             idxmaxx;
    INT             idxmaxy;
    INT             mx[4] = {-1, 1, 1, -1};
    INT             my[4] = {1, 1, -1, -1};
    INT             x;
    INT             y;
    INT             xx;
    INT             yy;
    INT             xdiff;
    INT             ydiff;


    GX_RECTANGLE   *clip;
    GX_RECTANGLE    rotated_clip;

    idxminx = (INT)((UINT)(angle / 90) & 0x3U);
    idxmaxx = (INT)((UINT)(idxminx + 2) & 0x3U);
    idxmaxy = (INT)((UINT)(idxminx + 1) & 0x3U);

    /* Calculate the x and y center of pixelmap source. */
    srcxres = (INT)((UINT)pixelmap -> gx_pixelmap_height >> 1);
    srcyres = (INT)((UINT)pixelmap -> gx_pixelmap_width >> 1);

    /** Swap x-axis and y-axis positions of a pixelmap */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(xpos, ypos);
    
    /** Swap xcenter and ycenter positions */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    /*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S *//*LDRA_INSPECTED 120 S */
    GX_SWAP_VALS(cx, cy);

    clip = context -> gx_draw_context_clip;

    if(GX_SCREEN_ROTATION_CW == (INT) (context->gx_draw_context_display->gx_display_rotation_angle))
    {
        srcyres = ((INT)(pixelmap -> gx_pixelmap_width)) - 1 - srcyres;

        ypos = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution)) - (INT)ypos -
                                                                    pixelmap -> gx_pixelmap_width;
        cy = ((INT)(pixelmap -> gx_pixelmap_width)) - cy - 1;

        rotated_clip.gx_rectangle_left = clip -> gx_rectangle_top;
        rotated_clip.gx_rectangle_right = clip -> gx_rectangle_bottom;
        rotated_clip.gx_rectangle_top = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution - 1 -
                                                                                       clip -> gx_rectangle_right);
        rotated_clip.gx_rectangle_bottom = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_x_resolution - 1 -
                                                                                            clip -> gx_rectangle_left);
    }
    else
    {
        srcxres = ((INT)(pixelmap -> gx_pixelmap_height)) - 1 - srcxres;

        xpos = ((INT)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution)) - xpos -
                                                        (INT)pixelmap -> gx_pixelmap_height;
        cx = (INT)((pixelmap -> gx_pixelmap_height)) - cx - 1;

        rotated_clip.gx_rectangle_left = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution - 1 -
                                                                                        clip -> gx_rectangle_bottom);
        rotated_clip.gx_rectangle_right = (GX_VALUE)(context -> gx_draw_context_canvas -> gx_canvas_y_resolution - 1 -
                                                                                           clip -> gx_rectangle_top);
        rotated_clip.gx_rectangle_top = clip -> gx_rectangle_left;
        rotated_clip.gx_rectangle_bottom = clip -> gx_rectangle_right;
    }

    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S */
    cosv = (INT)(_gx_utility_math_cos(GX_FIXED_VAL_MAKE((GX_FIXED_VAL)angle)));
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S */
    sinv = (INT)(_gx_utility_math_sin(GX_FIXED_VAL_MAKE((GX_FIXED_VAL)angle)));

    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
    xres = GX_FIXED_VAL_TO_INT((mx[idxmaxx] * srcxres * cosv) - (INT)(my[idxmaxx] * srcyres * sinv));
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
    yres = GX_FIXED_VAL_TO_INT((my[idxmaxy] * srcyres * cosv) + (INT)(mx[idxmaxy] * srcxres * sinv));

    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
    x = GX_FIXED_VAL_TO_INT(((cx - srcxres) * cosv) - ((cy - srcyres) * sinv));
    /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
    y = GX_FIXED_VAL_TO_INT(((cy - srcyres) * cosv) + ((cx - srcxres) * sinv));

    x += xres;
    y += yres;

    newxpos = (xpos + cx) - x;
    newypos = (ypos + cy) - y;

    /* Loop through the source's pixels.  */
    for (y = ((INT)rotated_clip.gx_rectangle_top - newypos); y < ((INT)rotated_clip.gx_rectangle_bottom - newypos); y++)
    {
        for (x = ((INT)rotated_clip.gx_rectangle_left - newxpos); x < ((INT)rotated_clip.gx_rectangle_right - newxpos);
                                                                                                                 x++)
        {
            xx = ((x - xres) * cosv) + ((y - yres) * sinv);
            yy = ((y - yres) * cosv) - ((x - xres) * sinv);

            /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 50 S */
            xdiff = (INT)((UINT)GX_FIXED_VAL_TO_INT(xx << 8) & 0xffU);
            /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 50 S */
            ydiff = (INT)((UINT)GX_FIXED_VAL_TO_INT(yy << 8) & 0xffU);

            /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
            xx = GX_FIXED_VAL_TO_INT(xx) + srcxres;
            /*LDRA_INSPECTED 50 S *//*LDRA_INSPECTED 90 S *//*LDRA_INSPECTED 90 S */
            yy = GX_FIXED_VAL_TO_INT(yy) + srcyres;

            if ((xx >= -1) && (xx < (INT)pixelmap -> gx_pixelmap_height) &&
                (yy >= -1) && (yy < (INT)pixelmap -> gx_pixelmap_width))
            {
                _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate(context, x, y, xx, yy, newxpos, newypos,
                                                                                        pixelmap, xdiff, ydiff);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief  Subroutine of _gx_synergy_display_driver_16bpp_32argb_rotated_pixelmap_rotate().
 * @param   context[in]          Pointer to a GUIX draw context
 * @param   x[in]                x - coordinate
 * @param   y[in]                y - coordinate
 * @param   xx[in]               x coordinate of rotated target pixel 
 * @param   yy[in]               y coordinate of rotated target pixel
 * @param   newxpos[in]          new x-coordinate of top-left draw point in pixel unit for rotation
 * @param   newypos[in]          new y-coordinate of top-left draw point in pixel unit
 * @param   pixelmap[in]         Pointer to a GX_PIXELMAP structure
 * @param   xdiff[in]            Shifted x coordinate of rotated target pixel 
 * @param   ydiff[in]            Shifted y coordinate of rotated target pixel
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT x, INT y, INT xx,
                                     INT yy, INT newxpos, INT newypos, GX_PIXELMAP *pixelmap, INT xdiff, INT ydiff)
{
    INT             alpha;
    GX_COLOR        red;
    GX_COLOR        green;
    GX_COLOR        blue;

    VOID          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = _gx_display_driver_565rgb_pixel_blend;

    _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate_handle_edge(xx, yy, pixelmap, xdiff, ydiff, &red, &green,
                                                                                                      &blue, &alpha);

    if (alpha)
    {
        red /= (ULONG)alpha;
        green /= (ULONG)alpha;
        blue /= (ULONG)alpha;
    }

    /*LDRA_INSPECTED 96 S Use of mixed mode arithmetic */
    red = BYTE_RANGE(red);
    /*LDRA_INSPECTED 96 S Use of mixed mode arithmetic */
    green = BYTE_RANGE(green);
    /*LDRA_INSPECTED 96 S Use of mixed mode arithmetic */
    blue = BYTE_RANGE(blue);
    /*LDRA_INSPECTED 96 S Use of mixed mode arithmetic *//*LDRA_INSPECTED 93 S  */
    alpha = (INT) BYTE_RANGE((GX_COLOR) alpha);

    blend_func(context, x + newxpos, y + newypos,
               (GX_COLOR)(((red & 0xf8) << 8)|((green & 0xfc) << 3)|((blue & 0xf8) >> 3)),
               (GX_UBYTE)alpha);
}

/*******************************************************************************************************************//**
 * @brief  Subroutine of _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate().
 * @param   xx[in]                x coordinate of rotated target pixel
 * @param   yy[in]                y coordinate of rotated target pixel
 * @param   pixelmap[in]          Pointer to a GX_PIXELMAP structure
 * @param   xdiff[in]             Shifted x coordinate of rotated target pixel
 * @param   ydiff[in]             Shifted y coordinate of rotated target pixel
 * @param   red[in][out]          Red component of a pixel
 * @param   green[in][out]        Green component of a pixel
 * @param   blue[in][out]         Blue component of a pixel
 * @param   alpha[in][out]        GUIX alpha value
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
static VOID _gx_synergy_display_driver_16bpp_32argb_pixelmap_rotate_handle_edge(INT xx, INT yy, GX_PIXELMAP *pixelmap,
                                 INT xdiff, INT ydiff, GX_COLOR * red, GX_COLOR * green, GX_COLOR * blue, INT * alpha)
{
    GX_COLOR       *get;
    GX_COLOR        a;
    GX_COLOR        b;
    GX_COLOR        c;
    GX_COLOR        d;
    if ((xx >= 0) && (xx < (pixelmap -> gx_pixelmap_height - 1)) && \
       (yy >= 0) && (yy < (pixelmap -> gx_pixelmap_width - 1)))
    {
        get = (GX_COLOR *)pixelmap -> gx_pixelmap_data;
        get += yy * pixelmap -> gx_pixelmap_height;
        get += xx;

        a = *get;
        b = *(get + 1);
        c = *(get + pixelmap -> gx_pixelmap_height);
        d = *(get + pixelmap -> gx_pixelmap_height + 1);
    }
    else
    {
        get = (GX_COLOR *)pixelmap -> gx_pixelmap_data;

        a = (ULONG) 0;
        b = a;
        c = a;
        d = a;
        if (-1 == xx)
        {
            /* handle left edge.  */
            if (yy >= 0)
            {
                b = *(get + (yy * pixelmap -> gx_pixelmap_height));
            }

            if (yy < (pixelmap -> gx_pixelmap_width - 1))
            {
                d = *(get + ((yy + 1) * pixelmap -> gx_pixelmap_height));
            }
        }
        else if (-1 == yy)
        {
            /* handle top edge.  */
            c = *(get + xx);

            if (xx < (pixelmap -> gx_pixelmap_height - 1))
            {
                d = *(get + xx + 1);
            }
        }
        else if (xx == (pixelmap -> gx_pixelmap_height - 1))
        {
            /* handle right edge. */
            a = *(get + (yy * pixelmap -> gx_pixelmap_height) + xx);

            if (yy < (pixelmap -> gx_pixelmap_width - 1))
            {
                c = *(get + ((yy + 1) * pixelmap -> gx_pixelmap_height) + xx);
            }
        }
        else
        {
            /* handle bottom edge. */
            a = *(get + (yy * pixelmap -> gx_pixelmap_height) + xx);
            b = *(get + (yy * pixelmap -> gx_pixelmap_height) + xx + 1);
        }
    }

    /*LDRA_INSPECTED 330 S */
    * red = ((REDVAL_32BPP(a) * (a >> 24) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff)) +
            /*LDRA_INSPECTED 330 S */
            (REDVAL_32BPP(b) * (b >> 24) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (REDVAL_32BPP(c) * (c >> 24) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (REDVAL_32BPP(d) * (d >> 24) * (GX_COLOR)xdiff * (GX_COLOR)ydiff)) >> 16;

    /*LDRA_INSPECTED 330 S */
    * green = ((GREENVAL_32BPP(a) * (a >> 24) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff)) + \
            /*LDRA_INSPECTED 330 S */
            (GREENVAL_32BPP(b) * (b >> 24) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (GREENVAL_32BPP(c) * (c >> 24) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (GREENVAL_32BPP(d) * (d >> 24) * (GX_COLOR)xdiff * (GX_COLOR)ydiff)) >> 16;

    /*LDRA_INSPECTED 330 S */
    * blue = ((BLUEVAL_32BPP(a) * (a >> 24) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff)) + \
            /*LDRA_INSPECTED 330 S */
            (BLUEVAL_32BPP(b) * (b >> 24) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (BLUEVAL_32BPP(c) * (c >> 24) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff)) +         \
            /*LDRA_INSPECTED 330 S */
            (BLUEVAL_32BPP(d) * (d >> 24) * (GX_COLOR)xdiff * (GX_COLOR)ydiff)) >> 16;

    /*LDRA_INSPECTED 93 S */
    * alpha = (INT) ((((a >> 24UL) * (256UL - (GX_COLOR)xdiff) * (256UL - (GX_COLOR)ydiff)) + \
            /*LDRA_INSPECTED 330 S */
            ((b >> 24UL) * (GX_COLOR)xdiff * (256UL - (GX_COLOR)ydiff)) +         \
            /*LDRA_INSPECTED 330 S */
            ((c >> 24UL) * (GX_COLOR)ydiff * (256UL - (GX_COLOR)xdiff)) +         \
            /*LDRA_INSPECTED 330 S */
            ((d >> 24UL) * (GX_COLOR)xdiff * (GX_COLOR)ydiff)) >> 16);

}
#endif  /* GX_USE_SYNERGY_DRW */

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Frame buffer toggle operation with copying data with rotation by software
 *  without D/AVE 2D acceleration and screen rotation.
 * This function is called by _gx_synergy_buffer_toggle_16bpp().
 * @param     canvas[in]           Pointer to a canvas
 * @param     copy[in]             Pointer to a rectangle region to copy
 * @param     src[in]              Pointer to canvas memory
 **********************************************************************************************************************/
static VOID gx_synergy_rotated_copy_to_working_16bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src)
{
     GX_RECTANGLE display_size;
     GX_RECTANGLE copy_clip;
     USHORT       *pGetRow;
     USHORT       *pPutRow;

     INT          copy_width;
     INT          copy_height;
     INT          canvas_stride;
     INT          display_stride;

     USHORT       *pGet;
     USHORT       *pPut;
     INT          row;
     INT          col;

     GX_DISPLAY * display = canvas->gx_canvas_display;

     gx_utility_rectangle_define(&display_size, 0, 0,
             (GX_VALUE)(display->gx_display_height - 1),
             (GX_VALUE)(display->gx_display_width - 1));

     if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
     {
         copy_clip.gx_rectangle_left = copy->gx_rectangle_top;
         copy_clip.gx_rectangle_top = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_right - 1);
         copy_clip.gx_rectangle_right = copy->gx_rectangle_bottom;
         copy_clip.gx_rectangle_bottom = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_left - 1);
     }
     else
     {
         copy_clip.gx_rectangle_top = copy->gx_rectangle_left;
         copy_clip.gx_rectangle_bottom = copy->gx_rectangle_right;
         copy_clip.gx_rectangle_left = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_bottom - 1);
         copy_clip.gx_rectangle_right = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_top - 1);
     }

     /** Offset canvas within frame buffer. */
     if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
     {
         gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_y,
        (GX_VALUE)(display->gx_display_width - canvas->gx_canvas_display_offset_x - canvas->gx_canvas_x_resolution));
     }
     else
     {
         gx_utility_rectangle_shift(&copy_clip, (GX_VALUE)(display->gx_display_height -
          canvas->gx_canvas_display_offset_y - canvas->gx_canvas_y_resolution), canvas->gx_canvas_display_offset_x);
     }

     _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
     copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
     copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

     if ((copy_width <= 0) || (copy_height <= 0))
     {
         return;
     }

     pGetRow = (USHORT *) src;
     pPutRow = (USHORT *) working_frame;

     canvas_stride = (INT)(canvas->gx_canvas_y_resolution);
     pPutRow = pPutRow + ((INT)(((INT)(copy_clip.gx_rectangle_top)) * canvas_stride));
     pPutRow = pPutRow + ((INT)(copy_clip.gx_rectangle_left));

     display_stride = (INT)(display->gx_display_height);
     pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
     pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left));

     for (row = 0; row < copy_height; row++)
     {
         pGet = pGetRow;
         pPut = pPutRow;

         for (col = 0; col < copy_width; col++)
         {
             /*LDRA_INSPECTED 9 S *//*LDRA_INSPECTED 9 S */
             *pPut++ = *pGet++;
         }
         pPutRow += canvas_stride;
         pGetRow += display_stride;
     }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Frame buffer toggle operation with copying data with rotation by software
 *  without D/AVE 2D acceleration and screen rotation.
 * This function is called by _gx_synergy_buffer_toggle_32bpp().
 * @param     canvas[in]            Pointer to a canvas
 * @param     copy[in]              Pointer to a rectangle region to copy
 * @param     src[in]               Pointer to canvas memory
 **********************************************************************************************************************/
static VOID gx_synergy_rotated_copy_to_working_32bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy, ULONG *src)
{
     GX_RECTANGLE display_size;
     GX_RECTANGLE copy_clip;
     ULONG       *pGetRow;
     ULONG       *pPutRow;

     INT          copy_width;
     INT          copy_height;
     INT          canvas_stride;
     INT          display_stride;

     ULONG       *pGet;
     ULONG       *pPut;
     INT          row;
     INT          col;

     GX_DISPLAY * display = canvas->gx_canvas_display;

     gx_utility_rectangle_define(&display_size, 0, 0,
             (GX_VALUE)(display->gx_display_height - 1),
             (GX_VALUE)(display->gx_display_width - 1));

     if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
     {
         copy_clip.gx_rectangle_left = copy->gx_rectangle_top;
         copy_clip.gx_rectangle_top = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_right - 1);
         copy_clip.gx_rectangle_right = copy->gx_rectangle_bottom;
         copy_clip.gx_rectangle_bottom = (GX_VALUE)(canvas->gx_canvas_x_resolution - copy->gx_rectangle_left - 1);
     }
     else
     {
         copy_clip.gx_rectangle_top = copy->gx_rectangle_left;
         copy_clip.gx_rectangle_bottom = copy->gx_rectangle_right;
         copy_clip.gx_rectangle_left = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_bottom - 1);
         copy_clip.gx_rectangle_right = (GX_VALUE)(canvas->gx_canvas_y_resolution - copy->gx_rectangle_top - 1);
     }

     /** Offset canvas within frame buffer. */
     if(GX_SCREEN_ROTATION_CW == (INT) (display->gx_display_rotation_angle))
     {
         gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_y,
        (GX_VALUE)(display->gx_display_width - canvas->gx_canvas_display_offset_x - canvas->gx_canvas_x_resolution));
     }
     else
     {
         gx_utility_rectangle_shift(&copy_clip, (GX_VALUE)(display->gx_display_height -
         canvas->gx_canvas_display_offset_y - canvas->gx_canvas_y_resolution), canvas->gx_canvas_display_offset_x);
     }

     _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
     copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
     copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

     if ((copy_width <= 0) || (copy_height <= 0))
     {
         return;
     }

     pGetRow = (ULONG *) src;
     pPutRow = (ULONG *) working_frame;

     canvas_stride = (INT)(canvas->gx_canvas_y_resolution);
     pPutRow = pPutRow + (((INT)((INT)(copy_clip.gx_rectangle_top)) * canvas_stride));
     pPutRow = pPutRow + ((INT)(copy_clip.gx_rectangle_left));

     display_stride = (INT)(display->gx_display_height);
     pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
     pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left));

     for (row = 0; row < copy_height; row++)
     {
         pGet = pGetRow;
         pPut = pPutRow;

         for (col = 0; col < copy_width; col++)
         {
             *pPut = *pGet;
             ++pPut;
             ++pGet;
         }
         pPutRow += canvas_stride;
         pGetRow += display_stride;
     }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Software rotated draw function to toggle frame buffer.
 *  This function performs copies canvas memory to working frame buffer if a canvas is used, then toggles frame buffer,
 *  finally copies visible frame buffer to working frame buffer. This function is called by GUIX if hardware rendering
 *  acceleration is not enabled.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   dirty[in]          Pointer to a dirty rectangle area
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_synergy_rotated_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE * dirty)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE copy;
    GX_DISPLAY  *display;

    copy.gx_rectangle_bottom = (SHORT) 0;
    copy.gx_rectangle_left = (SHORT) 0;
    copy.gx_rectangle_right = (SHORT) 0;
    copy.gx_rectangle_top = (SHORT) 0;

    display = canvas->gx_canvas_display;

    gx_utility_rectangle_define(&Limit, 0, 0,
                                (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
                                (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    sf_el_gx_frame_toggle(canvas->gx_canvas_display->gx_display_handle, &visible_frame);
    sf_el_gx_frame_pointers_get(canvas->gx_canvas_display->gx_display_handle, &visible_frame, &working_frame);

    /** If canvas memory is pointing directly to frame buffer, toggle canvas memory. */
    if (canvas->gx_canvas_memory == (GX_COLOR *) visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
    {
        if (GX_COLOR_FORMAT_565RGB == (INT) (display->gx_display_color_format))
        {
            gx_synergy_rotated_copy_to_working_16bpp(canvas, &copy, (ULONG *)visible_frame);
        }
        else
        {
            gx_synergy_rotated_copy_to_working_32bpp(canvas, &copy, (ULONG *)visible_frame);
        }
    }
}
