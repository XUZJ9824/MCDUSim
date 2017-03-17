/*****************************************************************
 *
 * Name:        termScreen.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: termScreen.cpp,v 1.17 2012/01/13 18:28:30 macrae.i Exp $
 */
#define WANT_CLEAR_COUNT            0   // 0=normal, 1=show count of # of clear() calls
#define WANT_DEBUG_SCRATCHPAD       0   // 0=normal, 1=debug
#define WANT_DEBUG_PRINT_TEXT       0   // 0=normal, 1=debug

#include "termScreen.h"
#include "log.h"
#include "MCDU_key.h"   // for KEY__L1...KEY__R6

#include "mcduSim.h"    // for CMcduSimApp
extern CMcduSimApp * theApp;

#define DELETE_AND_NULL(x) \
    if (x) { \
        delete x; \
        x = NULL; \
    }

#define DEGREE_CHAR     0x1c

#include "skin.h"
extern mcduGeometry_st * which_mcdu_geom;   /* defined in mcdusim.cpp */

#define SIM_FONT__FAMILY        which_mcdu_geom->screen_family
#define SIM_FONT__NORMAL_SIZE   which_mcdu_geom->screen_normal_size
#define SIM_FONT__SMALL_SIZE    which_mcdu_geom->screen_small_size
#define SIM_FONT__SMALL_OFFSET  which_mcdu_geom->screen_small_offset
#define SIM_FONT__STYLE         which_mcdu_geom->screen_style
#define SIM_FONT__WEIGHT        which_mcdu_geom->screen_weight

extern void Feed_the_Log(char * buffer); // in log_printf.cpp
void termscreen_log_printf(const char *format, ...)
{
    char buffer[500];

    va_list marker;
    va_start(marker, format);
    int narg = vsprintf_s(buffer, sizeof(buffer), format, marker);
    va_end(marker);

    Feed_the_Log(buffer);
}

//////////////////////////////////////////////////////////////////////

termChar_st * CTermScreen::FindCell(int row, int col)
{
    static termChar_st dummy_cell = {
                            /*charcode*/0x00,
                            /*color   */   0,
                            /*attrib  */   0,
                            /*size    */   0};
    if (row >=0 && row < screen_n_rows && col >=0 && col < screen_n_cols) {
        /*  [0,0]       [0,1]    [0,2] . . .    [0,#c-1]
         *  [1,0]       [1,1]    [1,2] . . .    [1,#c-1]
         *    .           .        .   . . .      .
         *    .           .        .   . . .      .
         *    .           .        .   . . .      .
         *  [#r-1,0] [#r-1,1] [#r-1,2] . . . [#r-1,#c-1]
         */
        return m_screen + (row * screen_n_cols + col);
    }
    return & dummy_cell;
}

void CTermScreen::TermScreen_Init(char * screen_ident)
{
    m_screen_identification = screen_ident;
    update_flag = 0;
    m_activeFlag = 0;

    m_led_exec = 0;
    m_led_dspl = 0;
    m_led_msg = 0;
    m_led_fail = 0;

    int i;
    for (i=0; i<6; i++) {
        m_left_button_width [i] = (SCREEN_N_COLS/2)+1;
        m_right_button_width[i] = (SCREEN_N_COLS/2)-1;
    }

    fontNormal           = new wxFont(SIM_FONT__NORMAL_SIZE, SIM_FONT__FAMILY, 
                                      SIM_FONT__STYLE, SIM_FONT__WEIGHT);
    fontNormalUnderscore = new wxFont(SIM_FONT__NORMAL_SIZE, SIM_FONT__FAMILY, 
                                      SIM_FONT__STYLE, SIM_FONT__WEIGHT, true);
    fontSmall            = new wxFont(SIM_FONT__SMALL_SIZE,  SIM_FONT__FAMILY, 
                                      SIM_FONT__STYLE, SIM_FONT__WEIGHT);
    fontSmallUnderscore  = new wxFont(SIM_FONT__SMALL_SIZE,  SIM_FONT__FAMILY, 
                                      SIM_FONT__STYLE, SIM_FONT__WEIGHT, true);

    screen_n_rows = which_mcdu_geom->screen_n_rows;
    screen_n_cols = which_mcdu_geom->screen_n_cols;
    m_screen = new termChar_st[screen_n_rows * screen_n_cols];
    clear();
}

CTermScreen::CTermScreen(char * screen_ident)
{
    TermScreen_Init(screen_ident);
}

CTermScreen::CTermScreen( wxFrame * parent, char * screen_ident) :
                            m_parent(parent)
{
    wxColourDatabase colorDB;
    for (int i=0; i<8; i++) m_colors[i] = new wxColour();
    *m_colors[0] = colorDB.Find(wxT("BLACK"));
    *m_colors[1] = colorDB.Find(wxT("CYAN"));
    *m_colors[2] = colorDB.Find(wxT("RED"));
    *m_colors[3] = colorDB.Find(wxT("YELLOW"));
    *m_colors[4] = colorDB.Find(wxT("GREEN"));
    *m_colors[5] = colorDB.Find(wxT("MAGENTA"));
    *m_colors[6] = colorDB.Find(wxT("ORANGE")); //AMBER
    *m_colors[7] = colorDB.Find(wxT("WHITE"));

    TermScreen_Init(screen_ident);
}

CTermScreen::~CTermScreen()
{
    int i;
    for (i=0; i<8; i++) {
        DELETE_AND_NULL(m_colors[i]);
    }
    DELETE_AND_NULL(m_screen);

    DELETE_AND_NULL(fontNormal);
    DELETE_AND_NULL(fontNormalUnderscore);
    DELETE_AND_NULL(fontSmall);
    DELETE_AND_NULL(fontSmallUnderscore);
}

//////////////////////////////////////////////////////////////////////

void    CTermScreen::clear()
{
    fill((unsigned char)' ');

    #if WANT_CLEAR_COUNT
    static int nClearCounter = 0;
    char clear_counter_msg[20];
    sprintf_s(clear_counter_msg, sizeof(clear_counter_msg), "Clear:%2d", ++nClearCounter);
    printText(screen_n_rows-1,
              screen_n_cols-strlen(clear_counter_msg)-1,
              clear_counter_msg, CHAR_SIZE_SMALL, C_BLACK, ATT_REVERSE);
    #endif //WANT_CLEAR_COUNT
}

void    CTermScreen::InvalidateScreen()
{
    setUpdateRowsCols(0, screen_n_cols-1, 0, screen_n_rows-1);
    commitUpdates();
}

void    CTermScreen::fill(unsigned char c)
{
    int i;
    //memset(m_screen,0,sizeof(m_screen));
    termChar_st * screen = m_screen;
    for (i=0; i<(screen_n_rows * screen_n_cols); i++)
    {
        screen->charcode = c;
        screen->color = DEFAULT_COLOR;
        screen->attrib = 0;
        screen->size = 0;
        screen++;
    }

    InvalidateScreen();
}

void    CTermScreen::printChar( int row,
                                int col,
                                char c,
                                int size,
                                int color = DEFAULT_COLOR,
                                int attrib = 0)
{
    if ( (row >= screen_n_rows) || (col >= screen_n_cols) )
    {
        if (row >= MAX_SCREEN_ROWS || (col >= MAX_SCREEN_COLS) ) {
            LOG2N("printChar out of limits (r,c):", row, col);
            return;   // clip if invalid position
        }

        /* want to see these characters -- might indicate wrong MCDU has been chosen */
        if (row >= screen_n_rows) row = screen_n_rows-1;
        if (col >= screen_n_cols) col = screen_n_cols-1;
    }
    
    termChar_st * cell = FindCell(row,col);
    cell->charcode = c;
    cell->color = color;
    cell->attrib = attrib;
    cell->size = size;
}

void    CTermScreen::noteCommandPromptWidth( int row, int col, int len)
{
    // is this a command prompt for one of the L1..L6 or R1..R6 buttons
    if (row>=2 && (row%2)==0) {
        int which_key = (row-2)/2; // 0,1,2,3,4,5
        if (col==0) {   // left side
            m_left_button_width[which_key] = len;
        }
        else
        if (col+len==screen_n_cols) { // right side
            m_right_button_width[which_key] = len;
        }
        // else neither
    } // not a button row
}

void    CTermScreen::printText( int row,
                                int col,
                                char * text,
                                int size,
                                int color,
                                int attrib)
{
    char * ptr = text;
    int lastCol = col;
    if (*ptr == 0) return;

    bool bWantDebug = false;
    #if WANT_DEBUG_SCRATCHPAD
    if (row>=(screen_n_rows-1)) bWantDebug = true;
    #endif
    #if WANT_DEBUG_PRINT_TEXT
    bWantDebug = true;
    #endif
    if (bWantDebug) {
    termscreen_log_printf("printText(%2d %2d \"%s\" %d %d %d}\n", row, col, text, size, color, attrib);
    }

    noteCommandPromptWidth(row, col, strlen(text));
    setUpdateRowsCols(col, lastCol, row, row);

    while (*ptr != 0)
    {
        printChar( row, lastCol, *ptr++, size, color, attrib);
        appendUpdateRowsCols(col, lastCol++, row, row);
    }
    commitUpdates();
}


void    CTermScreen::printMenuText( int pRow,
                                int pCol,
                                char * text,
                                int size,
                                int color,
                                int attrib)
{
    int row = 2;
    int col = 0;
    char * ptr = text;
    int lastCol = col;
    if (*ptr == 0) return;

    bool bWantDebug = false;
    #if WANT_DEBUG_SCRATCHPAD
    if (row>=(screen_n_rows-1)) bWantDebug = true;
    #endif
    #if WANT_DEBUG_PRINT_TEXT
    bWantDebug = true;
    #endif
    if (bWantDebug) {
    termscreen_log_printf("printMenuText(%2d %2d \"%s\" %d %d %d}\n", row, col, text, size, color, attrib);
    }

    noteCommandPromptWidth(row, col, strlen(text));
    setUpdateRowsCols(col, lastCol, row, row);

    while (*ptr != 0)
    {
        printChar( row, lastCol, *ptr++, size, color, attrib);
        appendUpdateRowsCols(col, lastCol++, row, row);
    }
    commitUpdates();
}

void    CTermScreen::setUpdateRowsCols(int colLeft, int colRight, int rowTop, int rowBottom)
{
//    if (update_flag != 0) { wxLogWarning(wxT("setUpdateRowsCols called with update_flag = 1, overrun?"));}
    update_flag = 1;
    m_update_col_left = colLeft;
    m_update_col_right= colRight;
    m_update_row_top = rowTop;
    m_update_row_bottom = rowBottom;
    //termscreen_log_printf("   setUpdateRowsCols LRTB %2d %2d %2d %2d\n", colLeft, colRight, rowTop, rowBottom);
}

void    CTermScreen::appendUpdateRowsCols (int colLeft, int colRight, int rowTop, int rowBottom)
{
    if (update_flag == 0) { wxLogWarning(wxT("setUpdateRowsCols called before any setUpdateRowsCols"));}
    if (colLeft < m_update_col_left) m_update_col_left = colLeft;
    if (colRight > m_update_col_right) m_update_col_right = colRight;
    if (rowTop < m_update_row_top ) m_update_row_top = rowTop;
    if (rowBottom > m_update_row_bottom ) m_update_row_bottom = rowBottom;
    //termscreen_log_printf("appendUpdateRowsCols LRTB %2d %2d %2d %2d -> %2d %2d %2d %2d\n", 
    //        colLeft, colRight, rowTop, rowBottom, 
    //        m_update_col_left, m_update_col_right, m_update_row_top, m_update_row_bottom);
}

void CTermScreen::commitUpdates (void)
{
    if (update_flag == 0) { wxLogWarning(wxT("commitUpdates called with update_flag = 0"));}
    //LOG4N("commitUpdates coord: L,R,T,B", m_update_col_left, m_update_col_right, m_update_row_top, m_update_row_bottom);
    int x = SCREEN_X + (m_update_col_left * SCREEN_CHAR_AREA_W);
    int w = (m_update_col_right - m_update_col_left + 1) * SCREEN_CHAR_AREA_W;
    int y = SCREEN_Y + (m_update_row_top * SCREEN_CHAR_AREA_H);
    int h = (m_update_row_bottom - m_update_row_top + 1) * SCREEN_CHAR_AREA_H;

    //termscreen_log_printf("       commitUpdates LRTB %2d %2d %2d %2d XY %3d %3d WH %3d %3d\n",
    //        m_update_col_left, m_update_col_right, m_update_row_top, m_update_row_bottom,
    //        x, y, w, h);

    m_parent->RefreshRect (wxRect( x,
                         y,
                         w,
                         h+1 ), /*(im) +1 to erase descender on Q when in scratchpad and switching to MENU */
                         false);

    //LOG4N("commitUpdates area: x,y,w,h", x, y, w, h);
}

void CTermScreen::setFlashCycle ( int flashCycle)
{
    int doRefreshFlag = 0;
    int i;

    m_flashCycle = flashCycle;

    // if there is no flashing character then do not request refresh screen to avoid flickering

    termChar_st * screen = m_screen;
    for (i=0; i< (screen_n_rows * screen_n_cols); i++)
    {
        if (screen->attrib & ATT_FLASHING)
        {
            doRefreshFlag = 1;
            break;
        }
        screen++;
    }

    if (doRefreshFlag == 0) return;

    InvalidateScreen();
}

void CTermScreen::paintScreen(wxRect rect) // must be only called from onPaint's parent
{
//    int first_row = (rect.x - SCREEN_X) / SCREEN_CHAR_AREA_W;
    // for now let repaint the entire screen...
    // TODO: only repaint characters in changing area

    int colIdx;
    int rowIdx;
    int lastAttrib = -1;
    int lastColor = -1;
    int lastSize = -1;
    int x = SCREEN_X;
    int y = SCREEN_Y;
    wxChar buf[4]={0,0,0,0};

    static wxRect screenRect(SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H);

    //LOG4N("screen paint:", rect.x, rect.y, rect.width, rect.height);

    if( rect.Intersects(screenRect) == false) return;

    //LOG("repaint screen");


    wxClientDC dc (m_parent);
    //wxFont fontNormal(10, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
    //wxFont fontNormalUnderscore(10, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD, true);
    //wxFont fontSmall(8, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
    //wxFont fontSmallUnderscore(8, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD, true);
    
    //wxFont fontNormal(SIM_FONT__NORMAL_SIZE, SIM_FONT__FAMILY, SIM_FONT__STYLE, SIM_FONT__WEIGHT);
    //wxFont fontNormalUnderscore(SIM_FONT__NORMAL_SIZE, SIM_FONT__FAMILY, SIM_FONT__STYLE, SIM_FONT__WEIGHT, true);
    //wxFont fontSmall(SIM_FONT__SMALL_SIZE, SIM_FONT__FAMILY, SIM_FONT__STYLE, SIM_FONT__WEIGHT);
    //wxFont fontSmallUnderscore(SIM_FONT__SMALL_SIZE, SIM_FONT__FAMILY, SIM_FONT__STYLE, SIM_FONT__WEIGHT, true);
    
    dc.SetBackgroundMode(wxSOLID ); //wxTRANSPARENT);
    //dc.SetFont(font);
    //dc.SetTextForeground(*wxGREEN); //wxWHITE);
    //dc.SetTextBackground(*wxBLACK);
    //wxPen pen(*wxRED, 1);
    //dc.SetPen(pen);
    //dc.DrawText(wxT("Hello"), 152,120);

    for (rowIdx = 0; rowIdx<screen_n_rows; rowIdx++)
    {
        for (colIdx = 0; colIdx<screen_n_cols; colIdx++)
        {
            // set Font size if needed
            termChar_st * cell = FindCell(rowIdx, colIdx);
            if ( (cell->size != lastSize) || (cell->attrib != lastAttrib) )
            {
                lastSize = cell->size;
                if (lastSize == CHAR_SIZE_NORMAL)
                {
                    //LOG("changing Font size to normal");
                    lastSize = CHAR_SIZE_NORMAL;
                    lastAttrib = cell->attrib;
                    if (lastAttrib & ATT_UNDERSCORE) dc.SetFont(*fontNormalUnderscore);
                    else                             dc.SetFont(*fontNormal);
                }
                else
                {
                    //LOG("changing Font size to small");
                    lastSize = CHAR_SIZE_SMALL;
                    lastAttrib = cell->attrib;
                    if (lastAttrib & ATT_UNDERSCORE) dc.SetFont(*fontSmallUnderscore);
                    else                             dc.SetFont(*fontSmall);
                }
                // set reverse video
                if (lastAttrib & ATT_REVERSE)  dc.SetTextBackground(*wxWHITE);
                else                           dc.SetTextBackground(*wxBLACK);

            }

            // TODO: set colour and attribute     m_colors[0] = colorDB.Find("MAGENTA");
            if (cell->color != lastColor)
            {
                lastColor = cell->color;
                dc.SetTextForeground( *m_colors[lastColor] );
            }

            // print char
            buf[0] = cell->charcode;
            //{ char msg[100];
            //sprintf_s(msg, sizeof(msg), "[%2d,%2d]=%02X",rowIdx,colIdx, buf[0]);
            //LOG(msg);
            //}
           if (buf[0] == DEGREE_CHAR)
           {
                buf[0] = 0xb0;       // replace degree character
           } else
           if (theApp->IsSmallFontAsLowerEnabled()) {
               if (lastSize==CHAR_SIZE_SMALL && isupper(buf[0])) {
                   buf[0] = tolower(buf[0]); // replace A..Z by a..z when small character
               }
           }
           else
           if (islower(buf[0])) {
                buf[0] = toupper(buf[0]); // replace a..z by A..Z
           } else
           if ((buf[0] < ' ') || (buf[0] > '_')  ||
               ((lastAttrib & ATT_FLASHING) && m_flashCycle == 0 ) ) {
                        buf[0] = ' ';
           }

           if (lastSize == CHAR_SIZE_SMALL) {
            dc.DrawText( buf[0], x,y+SIM_FONT__SMALL_OFFSET);
           } else {
            dc.DrawText( buf[0], x,y);
           }

            //dc.DrawText( s, x,y);
            x += SCREEN_CHAR_AREA_W;
            if (x >= (SCREEN_W + SCREEN_X))
            {
                x = SCREEN_X;
                y += SCREEN_CHAR_AREA_H;
                if (y > SCREEN_H + SCREEN_Y) y = SCREEN_Y;
            }

        } /*cols*/
    } /*rows*/

}

void CTermScreen::getCharDesc (termChar_st * charDesc, int row/*0,1,2...N_ROWS-1*/, int col/*0,1,2,...N_COLS-1*/)
{
    if (row<0 || row >= screen_n_rows || col<0 || col >= screen_n_cols)
    {
        //log_printf("getCharDesc(%d,%d)?\n", row, col);
        charDesc->charcode = '\0';
        return;
    }
    /* find cell, copy contents back to caller */
    *charDesc = (* FindCell(row,col));
}

void CTermScreen::getScreenStringForKey (char * buffer, int key)
{
    int row, col, len;
    char * buffer_start = buffer;
    bool bKeyIsLeft  = false;
    bool bKeyIsRight = false;
    *buffer = '\0';

    if (key >= KEY__L1 && key <= KEY__L6) {
        int which_key = (key-KEY__L1);  // 0, 1, 2, 3, 4, 5
        row = 2*(which_key+1);          // 2, 4, 6, 8,10,12
        col = 0;
        len = m_left_button_width[which_key];   // (SCREEN_N_COLS/2)+1
        bKeyIsLeft = true;
    }
    else
    if (key >= KEY__R1 && key <= KEY__R6) {
        int which_key = (key-KEY__R1);  // 0, 1, 2, 3, 4, 5
        row = 2*(which_key+1);          // 2, 4, 6, 8,10,12
        col = screen_n_cols - m_right_button_width[which_key]; //(SCREEN_N_COLS/2)-1;
        len = m_right_button_width[which_key];  // (SCREEN_N_COLS/2)+1
        bKeyIsRight = true;
    }
    else
        return;

    if (row>=0 && row < screen_n_rows && col>=0 || col < screen_n_cols) {
        bool bOnlyBlanksSoFar = true;
        while (len && col < screen_n_cols) {
            unsigned char ch = FindCell(row,col)->charcode;

            if (!bKeyIsRight || !bOnlyBlanksSoFar || ch!=' ') {
                bOnlyBlanksSoFar = false;
                *buffer = ch;
                if (!*buffer++)
                    break; // whoops, out of bounds
            } // else R1..R6, don't stoire leading blanks

            *buffer = '\0';
            -- len;
            ++ col;
        }
    }

    if (bKeyIsLeft) {
        // trim_trailing_blanks from output buffer, buffer points at null at end of string
        while (--buffer >= buffer_start) {
            if (*buffer!=' ')
                break;
            *buffer = '\0';
        }
    }
}

void CTermScreen::PreserveContents(saved_termscreen_st * info)
{
    info->nRows = screen_n_rows;
    info->nCols = screen_n_cols;
    info->cells = new termChar_st[screen_n_rows * screen_n_cols];

    for (int i=0; i<screen_n_rows; i++) {
        for (int j=0; j<screen_n_cols; j++) {
            info->cells[i * info->nCols + j] = (* FindCell(i,j));
        }
    }
}

void CTermScreen::RestoreContents(saved_termscreen_st * info)
{
    int nOldRows = info->nRows; if (nOldRows > screen_n_rows) nOldRows = screen_n_rows;
    int nOldCols = info->nCols; if (nOldCols > screen_n_cols) nOldCols = screen_n_cols;
    
    clear();

    for (int i=0; i<nOldRows; i++) {
        for (int j=0; j<nOldCols; j++) {
            (* FindCell(i,j)) = info->cells[i * info->nCols + j];
        }
    }

    delete [] info->cells;
    info->cells = NULL;
}
