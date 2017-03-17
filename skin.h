#ifndef __SKIN_H__
#define __SKIN_H__
/*****************************************************************
 *
 * Name:        skin.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: skin.h,v 1.6 2012/02/17 19:00:29 macrae.i Exp $
 */

struct mcduGeometry_st
{
    int             bmp_width;
    int             bmp_height;
    UINT            res_normal;
    const wxString& bmp_normal;
    UINT            res_backlit;
    const wxString& bmp_backlit;
    UINT            res_mouseOver;
    const wxString& bmp_mouseOver;
    UINT            res_pressed;
    const wxString& bmp_pressed;
    wxBitmapType    bmp_type; // BMP,PNG,JPG,etc

    int             screen_x;
    int             screen_y;
    int             screen_n_cols;
    int             screen_char_area_w;
    int             screen_n_rows;
    int             screen_char_area_h;

    int             screen_family;
    int             screen_normal_size;
    int             screen_small_size;
    int             screen_small_offset;
    int             screen_style;
    int             screen_weight;

    int             equipment_id;
};

struct skinControls_st
{
    int     controlID;
    int     controlType;
    int     left;
    int     top;
    int     right;
    int     bottom;
};

#endif //#ifndef __SKIN_H__
