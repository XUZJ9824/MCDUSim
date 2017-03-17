/////////////////////////////////////////////////////////////////////////////
// Name:        wx/setup.h
/////////////////////////////////////////////////////////////////////////////
// created by Ian, supposed to be created by Unix setup

// last change to match ...VS Projects...\mcdusim\lib\vc_lib\mswud\wx\setup.h
// 08-Jun-2011 3:15pm
//      wxUSE_STD_DEFAULT               1 // was 0
//      wxUSE_STD_IOSTREAM              1 // was 0
// 07-Jun-2011 4:50pm
//      wxUSE_DIALUP_MANAGER            1 // was 0
//      wxUSE_STD_IOSTREAM              1 // was 0
//      wxUSE_STD_STRING                1 // was 0
//      wxUSE_TOOLTIPS                  1 // was 0

#define wxUSE_ABOUTDLG                  1 /* used if wxUSE_GUI */ /* Compile in wxAboutBox() function showing the standard "About" dialog. */
#define wxUSE_ACCEL                     1 /* used if wxUSE_GUI */ /* wxAcceleratorTable/Entry classes and support for them in wxMenu(Bar) */
#define wxUSE_ACCESSIBILITY             0 /* wxAccessible for enhanced and customisable accessibility. Depends on wxUSE_OLE. */
#define wxUSE_ACTIVEX                   1 /* enable wxActiveXContainer class allowing to embed OLE controls in wx. */
#define wxUSE_AFM_FOR_POSTSCRIPT        1 /* use font metric files in GetTextExtent */
#define wxUSE_ALL_THEMES                1 /* enable compilation of all themes */
#define wxUSE_ANIMATIONCTRL             1 /* used if wxUSE_GUI */ /* need for wxAnimationCtrl class */
#define wxUSE_ANY                       1 /* need for wxAny class: the successor for wxVariant. */
#define wxUSE_APPLE_IEEE                1 /* Apple code will writes portable floats on the disk. See extended.c for the license */
#define wxUSE_ARCHIVE_STREAMS           1 /* wxArchive classes for accessing archives such as zip and tar */
#define wxUSE_AUI                       1 /* use wxWidget's AUI docking system */
#define wxUSE_AUTOID_MANAGEMENT         1 /* use reference counted ID management */
#define wxUSE_BASE64                    1 /* Base64 support. needed for storing binary data in wxConfig on most platforms. */
#define wxUSE_BITMAPCOMBOBOX            1 /* used if wxUSE_GUI */ /* wxBitmapComboBox is a combobox that can have images in front of text items. */
#define wxUSE_BMPBUTTON                 1 /* used if wxUSE_GUI */ /* need for wxBitmapButton class */
#define wxUSE_BUSYINFO                  1 /* wxBusyInfo class: displays window with message when app is busy. Works like wxBusyCursor */
#define wxUSE_BUTTON                    1 /* used if wxUSE_GUI */ /* need for wxButton class */
#define wxUSE_CALENDARCTRL              1 /* used if wxUSE_GUI */ /* need for wxCalendarCtrl class */
#define wxUSE_CARET                     1 /* used if wxUSE_GUI */ /* need for wxCaret class: a "cursor" in a text control (called caret under Windows) */
#define wxUSE_CHECKBOX                  1 /* used if wxUSE_GUI */ /* need for wxCheckBox class */
#define wxUSE_CHECKLISTBOX              1 /* used if wxUSE_GUI */ /* need for wxCheckListBox class (requires wxUSE_OWNER_DRAWN) */
#define wxUSE_CHOICE                    1 /* used if wxUSE_GUI */ /* need for wxChoice class */
#define wxUSE_CHOICEBOOK                1 /* used if wxUSE_GUI */ /* need for wxChoicebook control: similar to wxNotebook but uses wxChoice instead of the tabs */
#define wxUSE_CHOICEDLG                 1 /* used if wxUSE_GUI */ /* Use single/multiple choice dialogs. */
#define wxUSE_CLIPBOARD                 1 /* used if wxUSE_GUI */ /* need for wxClipboard class: clipboard copy/paste */
#define wxUSE_CMDLINE_PARSER            1 /* need for wxCmdLineParser class: upport command line parsing */
#define wxUSE_COLLPANE                  1 /* used if wxUSE_GUI */ /* need for wxCollapsiblePane class */
#define wxUSE_COLOURDLG                 1 /* used if wxUSE_GUI */ /* for colour picker dialog */
#define wxUSE_COLOURPICKERCTRL          1 /* used if wxUSE_GUI */ /* need for wxColourPickerCtrl class */
#define wxUSE_COMBOBOX                  1 /* used if wxUSE_GUI */ /* need for wxComboBox class */
#define wxUSE_COMBOCTRL                 1 /* used if wxUSE_GUI */ /* need for wxComboCtrl and related classes */
#define wxUSE_COMMON_DIALOGS            1 /* recommended. set 0 to disable printing architecture and interactive wxPrinterDC */
#define wxUSE_CONFIG                    1 /* need for wxConfig class: allow app to store its settings in persistent storage (.INI files or registry) */
#define wxUSE_CONFIG_NATIVE             1 /* use native config classes for Windows (.INI files or registry) or portable text files in Unix */
#define wxUSE_CONSOLE_EVENTLOOP         1 /* to use wxEventLoop even in console applications */
#define wxUSE_CONSTRAINTS               1 /* 0 for no window layout constraint system */
#define wxUSE_CONTROLS                  1 /* must be set if you use pretty much any control: button, edit box, etc */
#define wxUSE_CRASHREPORT               1 /* set to 0 if your programs never crash */
#define wxUSE_DATAOBJ                   1 /* used if wxUSE_GUI */ /* need for wxDataObject and related classes: allow clipboard and OLE drag-n-drop */
#define wxUSE_DATEPICKCTRL              1 /* need for wxDatePickerCtrl class */
#define wxUSE_DATEPICKCTRL_GENERIC      0 /* 1 to use wxDatePickerCtrlGeneric and the native wxDatePickerCtrl */
#define wxUSE_DATETIME                  1 /* for wxDateTime and related classes */
#define wxUSE_DATAVIEWCTRL              1 /* used if wxUSE_GUI */ /* need for wxDataViewCtrl class */
#define wxUSE_DATEPICKCTRL              1 /* used if wxUSE_GUI */ /* need for wxDatePickerCtrl class */
#define wxUSE_DC_CACHEING               1 /* for wxDC cacheing implementation */
#define wxUSE_DEBUG_CONTEXT             0 /* enables wxDebugContext */
#define wxUSE_DEBUG_NEW_ALWAYS          0 /* In debug mode, causes new to be defined to be WXDEBUG_NEW (see object.h) */
#define wxUSE_DEBUGREPORT               1 /* need for wxDebugReport class */
#define wxUSE_DIALUP_MANAGER            1 /* need for wxDialUpManager class: handles network connect/disconnect, dial-up notification when network connection established/terminated. Requires wxUSE_DYNAMIC_LOADER */
#define wxUSE_DIRDLG                    1 /* need for wxDirDlg class: get directory name from user */
#define wxUSE_DIRPICKERCTRL             1 /* used if wxUSE_GUI */ /* need for wxDirPickerCtrl class */
#define wxUSE_DISPLAY                   1 /* used if wxUSE_GUI */ /* need for wxDisplay class: enumerate all system displaysand their geometry */
#define wxUSE_DOC_VIEW_ARCHITECTURE     1 /* used if wxUSE_GUI */ /* 0 to disable MDI support. */
#define wxUSE_DRAG_AND_DROP             1 /* need for wxDropTarget and wxDropSource classes. not needed for drag and drop in wxTreeCtrl */
#define wxUSE_DRAGIMAGE                 1
#define wxUSE_DYNAMIC_LOADER            1 /* experimental, don't use for now, but needed by wxDialUpManager */
#define wxUSE_DYNLIB_CLASS              1 /* (Win32 only) Compile in run-time DLL loading and function calling, needed by wxUSE_DIALUP_MANAGER */
#define wxUSE_EDITABLELISTBOX           1 /* need for wxEditableListBox class */
#define wxUSE_ENH_METAFILE              1 /* metafile usage: use Enhanced MetaFiles (Win9X or later), needs wxUSE_METAFILE set */
#define wxUSE_EXCEPTIONS                1 /* Compile library in exception-safe mode: suggest 1 if using C++ exceptions in your code, 0 if not */
#define wxUSE_EXTENDED_RTTI             0 /* to use extended RTTI (work in progress) */
#define wxUSE_FILE                      1 /* need for wxFile classes */
#define wxUSE_FFILE                     1 /* need for wxFFile classes */
#define wxUSE_FILE_HISTORY              1 /* need for wxFileHistory class */
#define wxUSE_FILECTRL                  1 /* need for wxFileCtrl class */
#define wxUSE_FILEDLG                   1 /* used if wxUSE_GUI */ /* for file open/save dialogs, used many places */
#define wxUSE_FILEPICKERCTRL            1 /* used if wxUSE_GUI */ /* need for wxFilePickerCtrl class */
#define wxUSE_FILESYSTEM                1 /* enable virtual file systems (required by wxHTML) or using wxUSE_FS_xxx */
#define wxUSE_FINDREPLDLG               1 /* for find/replace dialogs */
#define wxUSE_FONTDLG                   1 /* used if wxUSE_GUI */ /* for font picker dialog */
#define wxUSE_FONTENUM                  1 /* need for wxFontEnumerator class */
#define wxUSE_FONTMAP                   1 /* used if wxUSE_GUI */ /* need for wxFontMapper class */
#define wxUSE_FONTPICKERCTRL            1 /* used if wxUSE_GUI */ /* need for wxFontPickerCtrl class */
#define wxUSE_FS_ARCHIVE                1 /* enable virtual archive filesystem (requires wxUSE_FILESYSTEM) */
#define wxUSE_FS_INET                   1 /* enable virtual Internet filesystem (requires wxUSE_FILESYSTEM) */
#define wxUSE_FS_ZIP                    1 /* enable virtual ZIP filesystem (requires wxUSE_FILESYSTEM) */
#define wxUSE_FSVOLUME                  1 /* wxFSVolume class providing access to the configured/active mount points */
#define wxUSE_FSWATCHER                 1 /* enable wxFileSystemWatcher */
#define wxUSE_GAUGE                     1 /* used if wxUSE_GUI */ /* need for wxGauge class */
#define wxUSE_GEOMETRY                  1 /* Miscellaneous geometry code: needed for Canvas library */
#define wxUSE_GIF                       1 /* for GIF format support in wxImage class */
#define wxUSE_GLCANVAS                  1 /* enable OpenGL support, needsa OpenGL headers & libraries (opengl32.lib,glu32.lib) */
#define wxUSE_GLOBAL_MEMORY_OPERATORS   0 /* 0 for built-in Visual C debugging */
#define wxUSE_GRID                      1 /* used if wxUSE_GUI */ /* need for wxGrid class */
#define wxUSE_GUI                       1
#define wxUSE_HEADERCTRL                1 /* need for wxHeaderCtrl class */
#define wxUSE_HELP                      1 /* used if wxUSE_GUI */ /* 0 for no help facility */
#define wxUSE_HOTKEY                    1 /* for Hotkey support (currently Windows only) */
#define wxUSE_HTML                      1 /* used if wxUSE_GUI */ /* wxHTML sublibrary allows to display HTML in wxWindow programs */
#define wxUSE_MS_HTML_HELP              1 /* need to use MS HTML help for wxHelpController and wxCHMHelpController */
#define wxUSE_HYPERLINKCTRL             1 /* used if wxUSE_GUI */ /* need for wxHyperlinkCtrl class */
#define wxUSE_ICO_CUR                   1 /* used if wxUSE_GUI */ /* for MS Icons and Cursors format support in wxImage class */
#define wxUSE_IFF                       0 /* used if wxUSE_GUI */ /* for IFF format support (Amiga format) in wxImage class */
#define wxUSE_IMAGE                     1 /* need for wxImage class. set to use anything besides .bmp files */
#define wxUSE_IMAGLIST                  1 /* used if wxUSE_GUI */ /* need for wxImageList, required for wxNotebook, wxTreeCtrl, wxListCtrl */
#define wxUSE_INFOBAR                   1 /* need for wxInfoBar class */
#define wxUSE_INICONF                   0 /* 0 for registry, 1 for .INI files using wxConfig and wxIniConfig */
#define wxUSE_INKEDIT                   0 /* to use InkEdit control (Tablet PC), if available */
#define wxUSE_INTL                      1 /* i18n support: _() macro, wxLocale class. Requires wxUSE_TEXTFILE */
#define wxUSE_IOSTREAMH                 0 /* only set with a really old compiler (below VC++ 4.2) */
#define wxUSE_IPC                       1 /* 0 for no interprocess comms */
#define wxUSE_IPV6                      0 /* if you need IPv6 socket classes, requires wxUSE_SOCKETS */
#define wxUSE_JOYSTICK                  1 /* used if wxUSE_GUI */ /* Joystick support class */
#define wxUSE_LIBJPEG                   1 /* for JPEG format support (requires libjpeg) in wxImage class */
#define wxUSE_LIBMSPACK                 0 /* used if wxUSE_GUI */
#define wxUSE_LIBPNG                    1 /* for PNG format support (requires libpng) in wxImage class. also requires wxUSE_ZLIB */
#define wxUSE_LIBTIFF                   1 /* for TIFF format support (requires libtiff) in wxImage class */
#define wxUSE_LISTBOOK                  1 /* used if wxUSE_GUI */ /* need for wxListbook control: similar to wxNotebook using wxListCtrl instead of tabs */
#define wxUSE_LISTBOX                   1 /* used if wxUSE_GUI */ /* need for wxListBox class */
#define wxUSE_LISTCTRL                  1 /* used if wxUSE_GUI */ /* need for wxListCtrl class */
#define wxUSE_LOG                       1 /* Support for message/error logging: wxLogXXX() and wxLog and derived classes */
#define wxUSE_LOG_DIALOG                1 /* used if wxUSE_GUI */ /* see wxUSE_LOG */
#define wxUSE_LOGGUI                    1 /* used if wxUSE_GUI */ /* see wxUSE_LOG */
#define wxUSE_LOGWINDOW                 1 /* used if wxUSE_GUI */ /* see wxUSE_LOG */
#define wxUSE_LONGLONG                  1 /* need for wxLongLong class, needed by wxDateTime */
#define wxUSE_MDI                       1 /* used if wxUSE_GUI */ /* 0 0 to disable MDI support */
#define wxUSE_MDI_ARCHITECTURE          1 /* used if wxUSE_GUI */ /* 0 to disable MDI document/view architecture, requires wxUSE_MDI and wxUSE_DOC_VIEW_ARCHITECTURE */
#define wxUSE_MEDIACTRL                 1 /* need for wxMediaCtrl class */
#define wxUSE_MEMORY_TRACING            0 /* enables debugging versions of wxObject::new and wxObject::delete *IF* __WXDEBUG__ also defined */
#define wxUSE_MENUS                     1 /* used if wxUSE_GUI */ /* need for wxMenu class */
#define wxUSE_METAFILE                  1 /* neeed for any metafile usage: see wxUSE_ENH_METAFILE and wxUSE_WIN_METAFILES_ALWAYS */
#define wxUSE_MFC                       0 /* set only to use wxWidgets and MFC in the same program */
#define wxUSE_MIMETYPE                  1 /* need for wxMimeTypesManager class */
#define wxUSE_MINIFRAME                 1 /* need for wxMiniFrame class: a frame with narrow title bar */
#define wxUSE_MOUSEWHEEL                1 /* include mouse wheel support */
#define wxUSE_MSGDLG                    1 /* used if wxUSE_GUI */ /* need for wxMessageBox() and wxMessageDialog() */
#define wxUSE_NATIVE_STATUSBAR          1 /* for wxStatusBar95 or a generic wxStatusBar */
#define wxUSE_NOTEBOOK                  1 /* used if wxUSE_GUI */ /* need for wxNotebook class: several "tabs" on the side */
#define wxUSE_NOTIFICATION_MESSAGE      1 /* need for wxNotificationMessage class: non-intrusive messages using balloons, banners, or popups */
#define wxUSE_NUMBERDLG                 1 /* for number entry dialog */
#define wxUSE_ODCOMBOBOX                1 /* used if wxUSE_GUI */ /* need for wxOwnerDrawnComboBox class */
#define wxUSE_OLE                       1 /* for generic OLE support, needed for drag-and-drop, clipboard, OLE Automation */
#define wxUSE_OLE_AUTOMATION            1 /* need for wxAutomationObject class */
#define wxUSE_ON_FATAL_EXCEPTION        1 /* calls wxApp::OnFatalException() instead of GPF. Windows only, and only if compiler supports Win32 SEH (eg, VC++) */
#define wxUSE_OWNER_DRAWN               1 /* needed for owner-drawn menu and listboxes. also needed by wxUSE_CHECKLISTBOX */
#define wxUSE_PALETTE                   1 /* used if wxUSE_GUI */ /* to compile in wxPalette class in wxImage class */
#define wxUSE_PCX                       1 /* for PCX format support in wxImage class */
#define wxUSE_PNM                       1 /* need for PNM format support in wxImage class */
#define wxUSE_POPUPWIN                  1 /* used if wxUSE_GUI */ /* need for wxPopupWindow class, used by wxTipWindow */
#define wxUSE_POSTSCRIPT                0 /* need for supporting PostScript printing: device context */
#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1 /* 1=PostScript print/preview architecture code under Windows, 0=use Windows printing */
#define wxUSE_PRINTF_POS_PARAMS         1 /* need for positional parameter support (e.g. %1$d, %2$s ...) in wxVsnprintf */
#define wxUSE_PRINTING_ARCHITECTURE     1 /* used if wxUSE_GUI */ /* need for print/preview architecture code */
#define wxUSE_PROGRESSDLG               1 /* need for progress dialog class for lengthy operations */
#define wxUSE_PROPGRID                  1 /* need for wxPropertyGrid class */
#define wxUSE_PROTOCOL                  1 /* need if any of the 3 wxUSE_PROTOCOL_xxx is set */
#define wxUSE_PROTOCOL_FILE             1 /* need for wxFileProto class */
#define wxUSE_PROTOCOL_FTP              1 /* need for wxFTP class */
#define wxUSE_PROTOCOL_HTTP             1 /* need for wxHTTP or wxURL class */
#define wxUSE_RADIOBOX                  1 /* used if wxUSE_GUI */ /* need for wxRadioBox class */
#define wxUSE_RADIOBTN                  1 /* used if wxUSE_GUI */ /* need for wxRadioButton class */
#define wxUSE_REARRANGECTRL             1 /* need for wxRearrangeDialog, or wxCheckListBox with up/down buttons */
#define wxUSE_REGEX                     1 /* for regular expression matching via wxRegEx class */
#define wxUSE_REGKEY                    1 /* need for wxRegKey class */
#define wxUSE_RIBBON                    1 /* to use Ribbon classes for interfaces */
#define wxUSE_RICHEDIT                  1 /* for RICHEDIT controls for wxTextCtrl */
#define wxUSE_RICHEDIT2                 1 /* for extra features of richedit v2 and later */
#define wxUSE_RICHTEXT                  1 /* need for wxRichTextCtrl class */
#define wxUSE_SASH                      1 /* used if wxUSE_GUI */
#define wxUSE_SCROLLBAR                 1 /* used if wxUSE_GUI */ /* need for wxScrollBar class */
#define wxUSE_SEARCHCTRL                1 /* need for wxSearchCtrl */
#define wxUSE_SNGLINST_CHECKER          1 /* need for wxSingleInstanceChecker class to verify at startup if another instance is running */
#define wxUSE_SLIDER                    1 /* used if wxUSE_GUI */ /* need for wxSlider */
#define wxUSE_SOCKETS                   1 /* need for socket classes */
#define wxUSE_SOUND                     1 /* used if wxUSE_GUI */ /* need for wxSound class */
#define wxUSE_SPINBTN                   1 /* used if wxUSE_GUI */ /* need for wxSpinButton class */
#define wxUSE_SPINCTRL                  1 /* used if wxUSE_GUI */ /* need for wxSpinCtrl class */
#define wxUSE_SPLASH                    1 /* used if wxUSE_GUI */ /* for splash screen class */
#define wxUSE_SPLINES                   1
#define wxUSE_SPLITTER                  1 /* used if wxUSE_GUI */ /* need for wxSplitterWindow class */
#define wxUSE_STACKWALKER               1 /* recommended, set 0 if your programs never crash */
#define wxUSE_STARTUP_TIPS              1
#define wxUSE_STATBMP                   1 /* used if wxUSE_GUI */ /* need for wxStaticBitmap class */
#define wxUSE_STATBOX                   1 /* used if wxUSE_GUI */ /* need for wxStaticBox class */
#define wxUSE_STATLINE                  1 /* used if wxUSE_GUI */ /* need for wxStaticLine class */
#define wxUSE_STATTEXT                  1 /* used if wxUSE_GUI */ /* need for wxStaticText class */
#define wxUSE_STATUSBAR                 1 /* used if wxUSE_GUI */ /* need for CreateStatusBar() in 3 classes */
#define wxUSE_STD_DEFAULT               1 /* default value for wxUSE_STD_IOSTREAM and wxUSE_STD_STRING */
#define wxUSE_STD_IOSTREAM              1 /* use 1 if using standard streams anyhow and dependency on standard streams library not problem */
#define wxUSE_STD_STRING                1 /* use 1 to enable conversion to standard C++ string */
#define wxUSE_STC                       1 /* 1 to use wxStyledTextCtrl class */
#define wxUSE_STDPATHS                  1
#define wxUSE_STL                       0 /* needed to derive wxList(Foo) and wxArray(Foo) */
#define wxUSE_STOPWATCH                 1
#define wxUSE_STREAMS                   1
#define wxUSE_SVG                       1
#define wxUSE_SYSTEM_OPTIONS            1
#define wxUSE_TAB_DIALOG                0 /* used if wxUSE_GUI */
#define wxUSE_TARSTREAM                 1
#define wxUSE_TASKBARICON               1
#define wxUSE_TASKBARICON_BALLOONS      1
#define wxUSE_TEXTBUFFER                1
#define wxUSE_TEXTCTRL                  1 /* used if wxUSE_GUI */ /* need for wxTextCtrl class */
#define wxUSE_TEXTDLG                   1
#define wxUSE_TEXTFILE                  1 /* need for wxTextFile class */
#define wxUSE_TGA                       1 /* for TGA format support (loading only) in wxImage class */
#define wxUSE_THEME_GTK                 0
#define wxUSE_THEME_METAL               0
#define wxUSE_THEME_MONO                0
#define wxUSE_THEME_WIN32               0
#define wxUSE_THREADS                   1 /* need for wxThread class */
#define wxUSE_TIMER                     1 /* for wxTimer class */
#define wxUSE_TIPWINDOW                 1 /* used if wxUSE_GUI */
#define wxUSE_TOGGLEBTN                 1 /* requires wxButton */
#define wxUSE_TOOLBAR                   1 /* used if wxUSE_GUI */
#define wxUSE_TOOLBAR_NATIVE            1
#define wxUSE_TOOLBOOK                  1
#define wxUSE_TOOLTIPS                  1 /* used if wxUSE_GUI */
#define wxUSE_TREEBOOK                  1
#define wxUSE_TREECTRL                  1 /* used if wxUSE_GUI */ /* need for wxTreeCtrl class */
#define wxUSE_UIACTIONSIMULATOR         0
#define wxUSE_UNICODE                   1 /* library is always built in Unicode mode now */
#define wxUSE_UIACTIONSIMULATOR         0
#define wxUSE_URL                       1 /* need for wxURL class */
#define wxUSE_URL_NATIVE                0 /* need for wxURLNativeImp class, experimental, Windows only */
#define wxUSE_UXTHEME                   1 /* compile MS Windows XP theme engine support */
#define wxUSE_VALIDATORS                1 /* used if wxUSE_GUI */
#define wxUSE_VARIANT                   1
#define wxUSE_WCHAR_T                   1 /* wxUSE_WCHAR_T now required by wxWidgets */
#define wxUSE_WIN_METAFILES_ALWAYS      0 /* metafile usage: set if you need to force use of old WMFs, needs wxUSE_METAFILE set */
#define wxUSE_WIZARDDLG                 1
#define wxUSE_WXDIB                     1
#define wxUSE_WXHTML_HELP               1 /* used if wxUSE_GUI */
#define wxUSE_XLOCALE                   1
#define wxUSE_XML                       1 /* Default is the same as wxUSE_XRC */
#define wxUSE_XPM                       1 /* for XPM format support in wxImage class */
#define wxUSE_XRC                       1 /* used if wxUSE_GUI */
#define wxUSE_ZIPSTREAM                 1
#define wxUSE_ZLIB                      1

#define WXWIN_COMPATIBILITY_2_6         0
#define WXWIN_COMPATIBILITY_2_8         1
#define wxDIALOG_UNIT_COMPATIBILITY     0 /* MSW-only: 0 for accurate dialog units, 1 for old behaviour */
#define REMOVE_UNUSED_ARG               1 /* Set this to 0 if your compiler can't cope with omission of prototype parameters. */

// Enable the new wxGraphicsPath and wxGraphicsContext classes for an advanced
// 2D drawing API.  (Still somewhat experimental)
//
// Please note that on Windows gdiplus.dll is loaded dynamically which means
// that nothing special needs to be done as long as you don't use
// wxGraphicsContext at all or only use it on XP and later systems but you
// still do need to distribute it yourself for an application using
// wxGraphicsContext to be runnable on pre-XP systems.
//
// Default is 1 except if you're using a non-Microsoft compiler under Windows
// as only MSVC7+ is known to ship with gdiplus.h. For other compilers (e.g.
// mingw32) you may need to install the headers (and just the headers)
// yourself. If you do, change the setting below manually.
//
// Recommended setting: 1 if supported by the compilation environment

// notice that we can't use wxCHECK_VISUALC_VERSION() here as this file is
// included from wx/platform.h before wxCHECK_VISUALC_VERSION() is defined
#ifdef _MSC_VER
#   if _MSC_VER >= 1310
        // MSVC7.1+ comes with new enough Platform SDK, enable
        // wxGraphicsContext support for it
#       define wxUSE_GRAPHICS_CONTEXT 1
#   else
        // MSVC 6 didn't include GDI+ headers so disable by default, enable it
        // here if you use MSVC 6 with a newer SDK
#       define wxUSE_GRAPHICS_CONTEXT 0
#   endif
#else
    // Disable support for other Windows compilers, enable it if your compiler
    // comes with new enough SDK or you installed the headers manually.
    //
    // Notice that this will be set by configure under non-Windows platforms
    // anyhow so the value there is not important.
#   define wxUSE_GRAPHICS_CONTEXT 0
#endif

// Set wxUSE_UNICODE_MSLU to 1 if you're compiling wxWidgets in Unicode mode
// and want to run your programs under Windows 9x and not only NT/2000/XP.
// This setting enables use of unicows.dll from MSLU (MS Layer for Unicode, see
// http://www.microsoft.com/globaldev/handson/dev/mslu_announce.mspx). Note
// that you will have to modify the makefiles to include unicows.lib import
// library as the first library (see installation instructions in install.txt
// to learn how to do it when building the library or samples).
//
// If your compiler doesn't have unicows.lib, you can get a version of it at
// http://libunicows.sourceforge.net
#define wxUSE_UNICODE_MSLU              0

#define wxDEBUG_LEVEL 0 /* defined as 1 in wx/debug.h  */
