#ifndef __TERMSCREEN_H__
#define __TERMSCREEN_H__
/*****************************************************************
 *
 * Name:        termScreen.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: termScreen.h,v 1.13 2012/01/13 18:28:30 macrae.i Exp $
 */

#include "wx/wx.h"
#include "wx/gdicmn.h"
//#include "wx/colour.h"

#define     MAX_SCREEN_ROWS     14 /* pick the largest for all appearances */
#define     MAX_SCREEN_COLS     24 /* pick the largest for all appearances */

#define     SCREEN_X            (which_mcdu_geom->screen_x)
#define     SCREEN_Y            (which_mcdu_geom->screen_y)
#define     SCREEN_N_COLS       (which_mcdu_geom->screen_n_cols)
#define     SCREEN_N_ROWS       (which_mcdu_geom->screen_n_rows)
#define     SCREEN_CHAR_AREA_W  (which_mcdu_geom->screen_char_area_w)
#define     SCREEN_CHAR_AREA_H  (which_mcdu_geom->screen_char_area_h)

#define     SCREEN_W            (SCREEN_N_COLS * SCREEN_CHAR_AREA_W)
#define     SCREEN_H            (SCREEN_N_ROWS * SCREEN_CHAR_AREA_H)

#define     C_BLACK         0   // 000000
#define     C_CYAN          1   // 00ffff
#define     C_RED           2   // ff0000
#define     C_YELLOW        3   // ffff00
#define     C_GREEN         4   // 008000
#define     C_MAGENTA       5   // ff00ff
#define     C_AMBER         6   // ffa500
#define     C_WHITE         7   // ffffff

#define     DEFAULT_COLOR   C_GREEN

#define     ATT_NORMAL      0
#define     ATT_REVERSE     1
#define     ATT_UNDERSCORE  2
#define     ATT_FLASHING    4

#define     CHAR_SIZE_SMALL 0
#define     CHAR_SIZE_NORMAL 1


//-------- TEXT -----------

struct termChar_st
{
    unsigned char   charcode;
    unsigned int    color   : 3;
    unsigned int    attrib  : 4;
    unsigned int    size    : 1;
};

typedef struct {
    int             nRows;
    int             nCols;
    termChar_st *   cells;
} saved_termscreen_st;

class CTermScreen
{
public:
    CTermScreen(char * screen_ident);
    CTermScreen( wxFrame * parent, char * screen_ident);
    ~CTermScreen();

public:
    //friend class CMcduSimFrame;
    void    paintScreen(wxRect rect);
    void    setFlashCycle ( int flashCycle);
    void    noteCommandPromptWidth(int row, int col, int size); /* for debugging */
    void    getScreenStringForKey (char * buffer, int key); /* for debugging */
    void    setUpdateRowsCols (int rowLeft, int rowRight, int colTop, int colBottom);
    void    commitUpdates (void);
    void    PreserveContents(saved_termscreen_st * info);
    void    RestoreContents(saved_termscreen_st * info);

    //friend class CProt;
    void    clear();
    void    printText(  int row,
                        int col,
                        char * text,
                        int size,
                        int color,
                        int attrib);
    void    printMenuText( int pRow,
                        int pCol,
                        char * text,
                        int size,
                        int color,
                        int attrib);

    //friend class CPythonInt;
    void getCharDesc (termChar_st * charDesc, int row, int col);

public:
    void    InvalidateScreen();
private:
    void    TermScreen_Init(char * screen_ident);
    void    fill(unsigned char c);

    int     m_led_exec;
    int     m_led_dspl;
    int     m_led_msg;
    int     m_led_fail;

    int     m_left_button_width[6];     //L1 to L6
    int     m_right_button_width[6];    //R1 to R6

private:
    int             screen_n_rows;
    int             screen_n_cols;
    termChar_st *   m_screen;
    termChar_st *   FindCell(int row, int col);

    wxFont *        fontNormal;
    wxFont *        fontNormalUnderscore;
    wxFont *        fontSmall;
    wxFont *        fontSmallUnderscore;

    void appendUpdateRowsCols (int rowLeft, int rowRight, int colTop, int colBottom);
    void    printChar(int row, int col, char c, int size, int color, int attrib);

    int update_flag;
    int m_update_col_left;
    int m_update_col_right;
    int m_update_row_top;
    int m_update_row_bottom;
    wxFrame * m_parent;
    int m_flashCycle;

    int m_activeFlag;

    wxColour * m_colors[8];

    char * m_screen_identification;
public:
    char * ScreenIdent() { return m_screen_identification; }

};
//----------------------------------------------------------------
#endif //__TERMSCREEN_H__
