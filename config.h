
#include <X11/XF86keysym.h>
/* Helper macros for spawning commands */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }
#define CMD(...)                                                               \
  {                                                                            \
    .v = (const char *[]) { __VA_ARGS__, NULL }                                \
  }

static const unsigned int borderpx = 1; /* border pixel of windows */

#if BAR_BORDER_PATCH
/* This allows the bar border size to be explicitly set separately from
 * borderpx. If left as 0 then it will default to the borderpx value of the
 * monitor and will automatically update with setborderpx. */
static const unsigned int barborderpx = 0; /* border pixel of bar */
#endif                                     // BAR_BORDER_PATCH
static const unsigned int snap = 32;       /* snap pixel */

#if VANITYGAPS_PATCH
static const unsigned int gappih = 10; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    10; /* vert outer gap between windows and screen edge */
static const int smartgaps_fact =
    1; /* gap factor when there is only one client; 0 = no gaps, 3 = 3x outer
          gaps */
#endif // VANITYGAPS_PATCH

static const int showbar = 1; /* 0 means no bar */
static const int topbar = 1;  /* 0 means bottom bar */

#if BAR_HEIGHT_PATCH
static const int bar_height = 38;
#endif // BAR_HEIGHT_PATCH
#if BAR_PADDING_PATCH
static const int vertpad = 10; /* vertical padding of bar */
static const int sidepad = 10; /* horizontal padding of bar */
#endif                         // BAR_PADDING_PATCH
/* Status is to be shown on: -1 (all monitors), 0 (a specific monitor by index),
 * 'A' (active monitor) */
static const int statusmon = 'A';
#if BAR_STATUSPADDING_PATCH
static const int horizpadbar = 2; /* horizontal padding for statusbar */
static const int vertpadbar = 0;  /* vertical padding for statusbar */
#endif                            // BAR_STATUSPADDING_PATCH
#if BAR_STATUSBUTTON_PATCH
static const char buttonbar[] = "<O>";
#endif // BAR_STATUSBUTTON_PATCH
#if BAR_SYSTRAY_PATCH
static const unsigned int systrayspacing = 2; /* systray spacing */
static const int showsystray = 1;             /* 0 means no systray */
#endif                                        // BAR_SYSTRAY_PATCH
#if BAR_TAGLABELS_PATCH
static const char ptagf[] = "[%s %s]"; /* format of a tag label */
static const char etagf[] = "[%s]";    /* format of an empty tag */
static const int lcaselbl = 0;         /* 1 means make tag label lowercase */
#endif                                 // BAR_TAGLABELS_PATCH
#if BAR_UNDERLINETAGS_PATCH
static const unsigned int ulinepad =
    5; /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke =
    0; /* thickness / height of the underline */
static const unsigned int ulinevoffset =
    0; /* how far above the bottom of the bar the line should appear */
static const int ulineall =
    0; /* 1 to show underline on all tags, 0 for just the active ones */
#endif // BAR_UNDERLINETAGS_PATCH

#if NAMETAG_PATCH
#if NAMETAG_PREPEND_PATCH
/* The format in which the tag is written when named. E.g. %d: %.12s will write
 * the tag number followed the first 12 characters of the given string. You can
 * also just use "%d: %s" here. */
#define NAMETAG_FORMAT "%d: %.12s"
#else
#define NAMETAG_FORMAT "%s"
#endif // NAMETAG_PREPEND_PATCH
/* The maximum amount of bytes reserved for each tag text. */
#define MAX_TAGLEN 16
/* The command to run (via popen). This can be tailored by adding a prompt,
 * passing other command line arguments or providing name options. Optionally
 * you can use other dmenu like alternatives like rofi -dmenu. */
#define NAMETAG_COMMAND "dmenu < /dev/null"
#endif // NAMETAG_PATCH
//
/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype = INDICATOR_NONE;
static int tiledindicatortype = INDICATOR_NONE;
static int floatindicatortype = INDICATOR_NONE;

static const char *fonts[] = {"JetBrainsMono Nerd Font:size=14"};
static const char dmenufont[] = "JetBrainsMono Nerd Font:size=12";

static char c000000[] = "#000000"; // placeholder value

// Normal
static char normfgcolor[] = "#abb2bf";
static char normbgcolor[] = "#282c34";
static char normbordercolor[] = "#3e4451";
static char normfloatcolor[] = "#c678dd";

// Selected
static char selfgcolor[] = "#abb2bf";
static char selbgcolor[] = "#3e4451";
static char selbordercolor[] = "#61afef";
static char selfloatcolor[] = "#61afef";

// Title (normal)
static char titlenormfgcolor[] = "#abb2bf";
static char titlenormbgcolor[] = "#282c34";
static char titlenormbordercolor[] = "#3e4451";
static char titlenormfloatcolor[] = "#c678dd";

// Title (selected)
static char titleselfgcolor[] = "#abb2bf";
static char titleselbgcolor[] = "#3e4451";
static char titleselbordercolor[] = "#61afef";
static char titleselfloatcolor[] = "#61afef";

// Tags (normal)
static char tagsnormfgcolor[] = "#abb2bf";
static char tagsnormbgcolor[] = "#282c34";
static char tagsnormbordercolor[] = "#3e4451";
static char tagsnormfloatcolor[] = "#c678dd";

// Tags (selected/highlighted)
static char tagsselfgcolor[] = "#282c34";
static char tagsselbgcolor[] = "#61afef";
static char tagsselbordercolor[] = "#282c34";
static char tagsselfloatcolor[] = "#282c34";

// Hidden
static char hidnormfgcolor[] = "#61afef";
static char hidselfgcolor[] = "#61afef";
static char hidnormbgcolor[] = "#282c34";
static char hidselbgcolor[] = "#282c34";

// Urgent
static char urgfgcolor[] = "#e06c75";
static char urgbgcolor[] = "#282c34";
static char urgbordercolor[] = "#e06c75";
static char urgfloatcolor[] = "#c678dd";

#if BAR_VTCOLORS_PATCH
static const char title_bg_dark[] = "#303030";
static const char title_bg_light[] = "#fdfdfd";
static const int color_ptrs[][ColCount] = {
    /*                       fg      bg      border  float */
    [SchemeNorm] = {-1, -1, 5, 12},      [SchemeSel] = {-1, -1, 11, 13},
    [SchemeTitleNorm] = {6, -1, -1, -1}, [SchemeTitleSel] = {6, -1, -1, -1},
    [SchemeTagsNorm] = {2, 0, 0, -1},    [SchemeTagsSel] = {6, 5, 5, -1},
    [SchemeHidNorm] = {5, 0, 0, -1},     [SchemeHidSel] = {6, -1, -1, -1},
    [SchemeUrg] = {7, 9, 9, 15},
#if BAR_LTSYMBOL_SCHEME_PATCH
    [SchemeLtSymbol] = {-1, 3, 0, 0},
#endif // BAR_LTSYMBOL_SCHEME_PATCH
};
#endif // BAR_VTCOLORS_PATCH

static char *colors[][ColCount] = {
    /*                       fg                bg                border float */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor, normfloatcolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor, selfloatcolor},
    [SchemeTitleNorm] = {titlenormfgcolor, titlenormbgcolor,
                         titlenormbordercolor, titlenormfloatcolor},
    [SchemeTitleSel] = {titleselfgcolor, titleselbgcolor, titleselbordercolor,
                        titleselfloatcolor},
    [SchemeTagsNorm] = {tagsnormfgcolor, tagsnormbgcolor, tagsnormbordercolor,
                        tagsnormfloatcolor},
    [SchemeTagsSel] = {tagsselfgcolor, tagsselbgcolor, tagsselbordercolor,
                       tagsselfloatcolor},
    [SchemeHidNorm] = {hidnormfgcolor, hidnormbgcolor, c000000, c000000},
    [SchemeHidSel] = {hidselfgcolor, hidselbgcolor, c000000, c000000},
    [SchemeUrg] = {urgfgcolor, urgbgcolor, urgbordercolor, urgfloatcolor},

};

/* Tags
 * In a traditional dwm the number of tags in use can be changed simply by
 * changing the number of strings in the tags array. This build does things a
 * bit different which has some added benefits. If you need to change the number
 * of tags here then change the NUMTAGS macro in dwm.c.
 *
 * Examples:
 *
 *  1) static char *tagicons[][NUMTAGS*2] = {
 *         [DEFAULT_TAGS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
 * "B", "C", "D", "E", "F", "G", "H", "I" },
 *     }
 *
 *  2) static char *tagicons[][1] = {
 *         [DEFAULT_TAGS] = { "•" },
 *     }
 *
 * The first example would result in the tags on the first monitor to be 1
 * through 9, while the tags for the second monitor would be named A through I.
 * A third monitor would start again at 1 through 9 while the tags on a fourth
 * monitor would also be named A through I. Note the tags count of NUMTAGS*2 in
 * the array initialiser which defines how many tag text / icon exists in the
 * array. This can be changed to *3 to add separate icons for a third monitor.
 *
 * For the second example each tag would be represented as a bullet point. Both
 * cases work the same from a technical standpoint - the icon index is derived
 * from the tag index and the monitor index. If the icon index is is greater
 * than the number of tag icons then it will wrap around until it an icon
 * matches. Similarly if there are two tag icons then it would alternate between
 * them. This works seamlessly with alternative tags and alttagsdecoration
 * patches.
 */
#if NAMETAG_PATCH
static char tagicons[][NUMTAGS][MAX_TAGLEN] =
#else
static char *tagicons[][NUMTAGS] =
#endif // NAMETAG_PATCH
    {
        [DEFAULT_TAGS] = {"", "", "", "󰇮", "", ""},
        [ALTERNATIVE_TAGS] = {"A", "B", "C", "D", "E", "F", "G"},
        [ALT_TAGS_DECORATION] = {"<1>", "<2>", "<3>", "<4>", "<5>", "<6>",
                                 "<7>"},
    };

#if BAR_TAGGRID_PATCH
/* grid of tags */
#define SWITCHTAG_UP 1 << 0
#define SWITCHTAG_DOWN 1 << 1
#define SWITCHTAG_LEFT 1 << 2
#define SWITCHTAG_RIGHT 1 << 3
#define SWITCHTAG_TOGGLETAG 1 << 4
#define SWITCHTAG_TAG 1 << 5
#define SWITCHTAG_VIEW 1 << 6
#define SWITCHTAG_TOGGLEVIEW 1 << 7

static const int tagrows = 2;
#endif // BAR_TAGGRID_PATCH

/* There are two options when it comes to per-client rules:
 *  - a typical struct table or
 *  - using the RULE macro
 *
 * A traditional struct table looks like this:
 *    // class      instance  title  wintype  tags mask  isfloating  monitor
 *    { "Gimp",     NULL,     NULL,  NULL,    1 << 4,    0,          -1 },
 *    { "Firefox",  NULL,     NULL,  NULL,    1 << 7,    0,          -1 },
 *
 * The RULE macro has the default values set for each field allowing you to only
 * specify the values that are relevant for your rule, e.g.
 *
 *    RULE(.class = "Gimp", .tags = 1 << 4)
 *    RULE(.class = "Firefox", .tags = 1 << 7)
 *
 * Refer to the Rule struct definition for the list of available fields
 * depending on the patches you enable.
 */
static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     *	WM_WINDOW_ROLE(STRING) = role
     *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
     */
    RULE(.wintype = WTYPE "DIALOG", .isfloating = 1)
        RULE(.wintype = WTYPE "UTILITY", .isfloating = 1)
            RULE(.wintype = WTYPE "TOOLBAR", .isfloating = 1)
                RULE(.wintype = WTYPE "SPLASH", .isfloating = 1)
                    RULE(.class = "Gimp", .tags = 1 << 4)
                        RULE(.class = "Firefox", .tags = 1 << 7)
#if RENAMED_SCRATCHPADS_PATCH
                            RULE(.instance = "spterm", .scratchkey = 's',
                                 .isfloating = 1)
#elif SCRATCHPADS_PATCH
                            RULE(.instance = "spterm", .tags = SPTAG(0),
                                 .isfloating = 1)
#endif // SCRATCHPADS_PATCH
};

/* Bar rules allow you to configure what is shown where on the bar, as well as
 * introducing your own bar modules.
 *
 *    monitor:
 *      -1  show on all monitors
 *       0  show on monitor 0
 *      'A' show on active monitor (i.e. focused / selected) (or just -1 for
 * active?) bar - bar index, 0 is default, 1 is extrabar alignment - how the
 * module is aligned compared to other modules widthfunc, drawfunc, clickfunc -
 * providing bar module width, draw and click functions name - does nothing,
 * intended for visual clue and for logging / debugging
 */
static const BarRule barrules[] = {
/* monitor   bar    alignment         widthfunc                 drawfunc
 * clickfunc                hoverfunc                name */
#if BAR_STATUSBUTTON_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_stbutton, draw_stbutton, click_stbutton, NULL,
     "statusbutton"},
#endif // BAR_STATUSBUTTON_PATCH
#if BAR_LAUNCHER_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_launcher, draw_launcher, click_launcher, NULL,
     "launcher"},
#endif // BAR_LAUNCHER_PATCH
#if BAR_POWERLINE_TAGS_PATCH
    {0, 0, BAR_ALIGN_LEFT, width_pwrl_tags, draw_pwrl_tags, click_pwrl_tags,
     hover_pwrl_tags, "powerline_tags"},
#endif // BAR_POWERLINE_TAGS_PATCH
#if BAR_TAGS_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_tags, draw_tags, click_tags, hover_tags,
     "tags"},
#endif // BAR_TAGS_PATCH
#if BAR_TAGLABELS_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_taglabels, draw_taglabels, click_taglabels,
     hover_taglabels, "taglabels"},
#endif // BAR_TAGLABELS_PATCH
#if BAR_TAGGRID_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_taggrid, draw_taggrid, click_taggrid, NULL,
     "taggrid"},
#endif // BAR_TAGGRID_PATCH
#if BAR_SYSTRAY_PATCH
    {0, 0, BAR_ALIGN_RIGHT, width_systray, draw_systray, click_systray, NULL,
     "systray"},
#endif // BAR_SYSTRAY_PATCH
#if BAR_LTSYMBOL_PATCH
    {-1, 0, BAR_ALIGN_LEFT, width_ltsymbol, draw_ltsymbol, click_ltsymbol, NULL,
     "layout"},
#endif // BAR_LTSYMBOL_PATCH
#if BAR_STATUSCOLORS_PATCH && BAR_STATUSCMD_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_statuscolors, draw_statuscolors,
     click_statuscmd, NULL, "statuscolors"},
#elif BAR_STATUSCOLORS_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_statuscolors, draw_statuscolors,
     click_statuscolors, NULL, "statuscolors"},
#elif BAR_STATUS2D_PATCH && BAR_STATUSCMD_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_status2d, draw_status2d,
     click_statuscmd, NULL, "status2d"},
#elif BAR_STATUS2D_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_status2d, draw_status2d,
     click_status2d, NULL, "status2d"},
#elif BAR_POWERLINE_STATUS_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_pwrl_status, draw_pwrl_status,
     click_pwrl_status, NULL, "powerline_status"},
#elif BAR_STATUS_PATCH && BAR_STATUSCMD_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_status, draw_status, click_statuscmd,
     NULL, "status"},
#elif BAR_STATUS_PATCH
    {statusmon, 0, BAR_ALIGN_RIGHT, width_status, draw_status, click_status,
     NULL, "status"},
#endif // BAR_STATUS2D_PATCH | BAR_STATUSCMD_PATCH
#if XKB_PATCH
    {0, 0, BAR_ALIGN_RIGHT, width_xkb, draw_xkb, click_xkb, NULL, "xkb"},
#endif // XKB_PATCH
#if BAR_FLEXWINTITLE_PATCH
    {-1, 0, BAR_ALIGN_NONE, width_flexwintitle, draw_flexwintitle,
     click_flexwintitle, NULL, "flexwintitle"},
#elif BAR_TABGROUPS_PATCH
    {-1, 0, BAR_ALIGN_NONE, width_bartabgroups, draw_bartabgroups,
     click_bartabgroups, NULL, "bartabgroups"},
#elif BAR_AWESOMEBAR_PATCH
    {-1, 0, BAR_ALIGN_NONE, width_awesomebar, draw_awesomebar, click_awesomebar,
     NULL, "awesomebar"},
#elif BAR_FANCYBAR_PATCH
    {-1, 0, BAR_ALIGN_NONE, width_fancybar, draw_fancybar, click_fancybar, NULL,
     "fancybar"},
#elif BAR_WINTITLE_PATCH
    {-1, 0, BAR_ALIGN_NONE, width_wintitle, draw_wintitle, click_wintitle, NULL,
     "wintitle"},
#endif // BAR_TABGROUPS_PATCH | BAR_AWESOMEBAR_PATCH | BAR_FANCYBAR_PATCH |
       // BAR_WINTITLE_PATCH
#if BAR_EXTRASTATUS_PATCH
#if BAR_STATUSCOLORS_PATCH && BAR_STATUSCMD_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_statuscolors_es,
     draw_statuscolors_es, click_statuscmd_es, NULL, "statuscolors_es"},
#elif BAR_STATUSCOLORS_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_statuscolors_es,
     draw_statuscolors_es, click_statuscolors, NULL, "statuscolors_es"},
#elif BAR_STATUS2D_PATCH && BAR_STATUSCMD_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_status2d_es, draw_status2d_es,
     click_statuscmd_es, NULL, "status2d_es"},
#elif BAR_STATUS2D_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_status2d_es, draw_status2d_es,
     click_status2d, NULL, "status2d_es"},
#elif BAR_POWERLINE_STATUS_PATCH
    {statusmon, 1, BAR_ALIGN_RIGHT, width_pwrl_status_es, draw_pwrl_status_es,
     click_pwrl_status, NULL, "powerline_status"},
#elif BAR_STATUSCMD_PATCH && BAR_STATUS_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_status_es, draw_status_es,
     click_statuscmd_es, NULL, "status_es"},
#elif BAR_STATUS_PATCH
    {statusmon, 1, BAR_ALIGN_CENTER, width_status_es, draw_status_es,
     click_status, NULL, "status_es"},
#endif // BAR_STATUS2D_PATCH | BAR_STATUSCMD_PATCH
#endif // BAR_EXTRASTATUS_PATCH
#if BAR_FLEXWINTITLE_PATCH
#if BAR_WINTITLE_HIDDEN_PATCH
    {-1, 1, BAR_ALIGN_RIGHT_RIGHT, width_wintitle_hidden, draw_wintitle_hidden,
     click_wintitle_hidden, NULL, "wintitle_hidden"},
#endif
#if BAR_WINTITLE_FLOATING_PATCH
    {-1, 1, BAR_ALIGN_LEFT, width_wintitle_floating, draw_wintitle_floating,
     click_wintitle_floating, NULL, "wintitle_floating"},
#endif // BAR_WINTITLE_FLOATING_PATCH
#endif // BAR_FLEXWINTITLE_PATCH
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
#if FLEXTILE_DELUXE_LAYOUT
static const int nstack = 0; /* number of clients in primary stack area */
#endif                       // FLEXTILE_DELUXE_LAYOUT
static const int resizehints =
    0; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */
#if DECORATION_HINTS_PATCH
static const int decorhints = 1; /* 1 means respect decoration hints */
#endif                           // DECORATION_HINTS_PATCH

#if NROWGRID_LAYOUT
#define FORCE_VSPLIT 1
#endif

#if TAPRESIZE_PATCH
/* mouse scroll resize */
static const int scrollsensetivity =
    30; /* 1 means resize window by 1 pixel for each scroll event */
/* resizemousescroll direction argument list */
static const int scrollargs[][2] = {
    /* width change         height change */
    {+scrollsensetivity, 0},
    {-scrollsensetivity, 0},
    {0, +scrollsensetivity},
    {0, -scrollsensetivity},
};
#endif // TAPRESIZE_PATCH

#if FLEXTILE_DELUXE_LAYOUT
static const Layout layouts[] = {
    /* symbol     arrange function, { nmaster, nstack, layout, master axis,
       stack axis, secondary stack axis, symbol func } */
    {"[]=",
     flextile,
     {-1, -1, SPLIT_VERTICAL, TOP_TO_BOTTOM, TOP_TO_BOTTOM, 0,
      NULL}},           // default tile layout
    {"><>", NULL, {0}}, /* no layout function means floating behavior */
    {"[M]", flextile, {-1, -1, NO_SPLIT, MONOCLE, MONOCLE, 0, NULL}}, // monocle
    {"|||",
     flextile,
     {-1, -1, SPLIT_VERTICAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM, 0,
      NULL}}, // columns (col) layout
    {">M>",
     flextile,
     {-1, -1, FLOATING_MASTER, LEFT_TO_RIGHT, LEFT_TO_RIGHT, 0,
      NULL}}, // floating master
    {"[D]",
     flextile,
     {-1, -1, SPLIT_VERTICAL, TOP_TO_BOTTOM, MONOCLE, 0, NULL}}, // deck
    {"TTT",
     flextile,
     {-1, -1, SPLIT_HORIZONTAL, LEFT_TO_RIGHT, LEFT_TO_RIGHT, 0,
      NULL}}, // bstack
    {"===",
     flextile,
     {-1, -1, SPLIT_HORIZONTAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM, 0,
      NULL}}, // bstackhoriz
    {"|M|",
     flextile,
     {-1, -1, SPLIT_CENTERED_VERTICAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM,
      TOP_TO_BOTTOM, NULL}}, // centeredmaster
    {"-M-",
     flextile,
     {-1, -1, SPLIT_CENTERED_HORIZONTAL, TOP_TO_BOTTOM, LEFT_TO_RIGHT,
      LEFT_TO_RIGHT, NULL}}, // centeredmaster horiz
    {":::",
     flextile,
     {-1, -1, NO_SPLIT, GAPPLESSGRID, GAPPLESSGRID, 0, NULL}}, // gappless grid
    {"[\\]",
     flextile,
     {-1, -1, NO_SPLIT, DWINDLE, DWINDLE, 0, NULL}}, // fibonacci dwindle
    {"(@)",
     flextile,
     {-1, -1, NO_SPLIT, SPIRAL, SPIRAL, 0, NULL}}, // fibonacci spiral
    {"[T]",
     flextile,
     {-1, -1, SPLIT_VERTICAL, LEFT_TO_RIGHT, TATAMI, 0, NULL}}, // tatami mats
#if TILE_LAYOUT
    {"", tile, {0}},
#endif
#if MONOCLE_LAYOUT
    {"", monocle, {0}},
#endif
#if BSTACK_LAYOUT
    {"TTT", bstack, {0}},
#endif
#if BSTACKHORIZ_LAYOUT
    {"===", bstackhoriz, {0}},
#endif
#if CENTEREDMASTER_LAYOUT
    {"|M|", centeredmaster, {0}},
#endif
#if CENTEREDFLOATINGMASTER_LAYOUT
    {">M>", centeredfloatingmaster, {0}},
#endif
#if COLUMNS_LAYOUT
    {"|||", col, {0}},
#endif
#if DECK_LAYOUT
    {"[D]", deck, {0}},
#endif
#if FIBONACCI_SPIRAL_LAYOUT
    {"(@)", spiral, {0}},
#endif
#if FIBONACCI_DWINDLE_LAYOUT
    {"[\\]", dwindle, {0}},
#endif
#if GRIDMODE_LAYOUT
    {"HHH", grid, {0}},
#endif
#if HORIZGRID_LAYOUT
    {"---", horizgrid, {0}},
#endif
#if GAPPLESSGRID_LAYOUT
    {":::", gaplessgrid, {0}},
#endif
#if NROWGRID_LAYOUT
    {"###", nrowgrid, {0}},
#endif
};
#else
static const Layout layouts[] = {
/* symbol     arrange function */
#if TILE_LAYOUT
    {"", tile}, /* first entry is default */
#endif
    {"󱂬", NULL}, /* no layout function means floating behavior */
#if MONOCLE_LAYOUT
    {"", monocle},
#endif
};
#endif // FLEXTILE_DELUXE_LAYOUT

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* commands */
#if !NODMENU_PATCH
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
#endif   // NODMENU_PATCH
static const char *dmenucmd[] = {
    "rofi",
    "-show",
    "drun",
    "-theme",
    "/home/andrew/.config/rofi/launchers/type-3/style-2.rasi",
    NULL};

static const char *termcmd[] = {"st", NULL};
static const char *browsercmd[] = {"firefox", NULL};

/* Commands for volume and brightness */
/* volume up / down / mute + ping dwmblocks */
static const char *volupcmd[] = {
    "sh", "-c",
    "pactl set-sink-volume @DEFAULT_SINK@ +5% && kill -44 $(pidof dwmblocks)",
    NULL};

static const char *voldowncmd[] = {
    "sh", "-c",
    "pactl set-sink-volume @DEFAULT_SINK@ -5% && kill -44 $(pidof dwmblocks)",
    NULL};

static const char *volmutecmd[] = {
    "sh", "-c",
    "pactl set-sink-mute @DEFAULT_SINK@ toggle && kill -44 $(pidof dwmblocks)",
    NULL};
static const char *brightupcmd[] = {"brightnessctl", "set", "+10%", NULL};
static const char *brightdowncmd[] = {"brightnessctl", "set", "10%-", NULL};

#if BAR_STATUSCMD_PATCH
#if BAR_DWMBLOCKS_PATCH
/* This defines the name of the executable that handles the bar (used for
 * signalling purposes) */
#define STATUSBAR "dwmblocks"
#else
/* commands spawned when clicking statusbar, the mouse button pressed is
 * exported as BUTTON */
static const StatusCmd statuscmds[] = {
    {"notify-send Volume$BUTTON", 1},
    {"notify-send CPU$BUTTON", 2},
    {"notify-send Battery$BUTTON", 3},
};
/* test the above with: xsetroot -name "$(printf '\x01Volume |\x02 CPU |\x03
 * Battery')" */
static const char *statuscmd[] = {"/bin/sh", "-c", NULL, NULL};
#endif // BAR_DWMBLOCKS_PATCH
#endif // BAR_STATUSCMD_PATCH

#if ON_EMPTY_KEYS_PATCH
static const char *firefoxcmd[] = {"firefox", NULL};
static const Key on_empty_keys[] = {
    /* modifier key            function                argument */
    {0, XK_f, spawn, {.v = firefoxcmd}},
};
#endif // ON_EMPTY_KEYS_PATCH

static const Key keys[] = {
    /* modifier                     key            function argument
     */
    {MODKEY, XK_space, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, spawn, {.v = browsercmd}},
    {MODKEY | ShiftMask, XK_b, togglebar, {0}},
#if TOGGLETOPBAR_PATCH
    {MODKEY | ShiftMask, XK_b, toggletopbar, {0}},
#endif // TOGGLETOPBAR_PATCH
#if TAB_PATCH
    {MODKEY | ControlMask, XK_b, tabmode, {-1}},
#endif // TAB_PATCH
#if FOCUSMASTER_PATCH || FOCUSMASTER_RETURN_PATCH
    {MODKEY | ControlMask, XK_space, focusmaster, {0}},
#endif // FOCUSMASTER_PATCH / FOCUSMASTER_RETURN_PATCH
#if STACKER_PATCH
    STACKKEYS(MODKEY, focus) STACKKEYS(MODKEY | ShiftMask, push)
#else
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
#endif // STACKER_PATCH
#if FOCUSDIR_PATCH
        {MODKEY, XK_Left, focusdir, {.i = 0}}, // left
    {MODKEY, XK_Right, focusdir, {.i = 1}},    // right
    {MODKEY, XK_Up, focusdir, {.i = 2}},       // up
    {MODKEY, XK_Down, focusdir, {.i = 3}},     // down
#endif                                         // FOCUSDIR_PATCH
#if PLACEDIR_PATCH
    {MODKEY | ControlMask, XK_Left, placedir, {.i = 0}},  // left
    {MODKEY | ControlMask, XK_Right, placedir, {.i = 1}}, // right
    {MODKEY | ControlMask, XK_Up, placedir, {.i = 2}},    // up
    {MODKEY | ControlMask, XK_Down, placedir, {.i = 3}},  // down
#endif                                                    // PLACEDIR_PATCH
#if SWAPFOCUS_PATCH && PERTAG_PATCH
    {MODKEY, XK_s, swapfocus, {.i = -1}},
#endif // SWAPFOCUS_PATCH
#if SWITCHCOL_PATCH
    {MODKEY, XK_v, switchcol, {0}},
#endif // SWITCHCOL_PATCH
#if ROTATESTACK_PATCH
    {MODKEY | ShiftMask, XK_j, rotatestack, {.i = +1}},
    {MODKEY | ShiftMask, XK_k, rotatestack, {.i = -1}},
#endif // ROTATESTACK_PATCH
#if INPLACEROTATE_PATCH
    {MODKEY | Mod4Mask, XK_j, inplacerotate, {.i = +2}}, // same as rotatestack
    {MODKEY | Mod4Mask, XK_k, inplacerotate, {.i = -2}}, // same as reotatestack
    {MODKEY | Mod4Mask | ShiftMask, XK_j, inplacerotate, {.i = +1}},
    {MODKEY | Mod4Mask | ShiftMask, XK_k, inplacerotate, {.i = -1}},
#endif // INPLACEROTATE_PATCH
#if PUSH_PATCH || PUSH_NO_MASTER_PATCH
    {MODKEY | ControlMask, XK_j, pushdown, {0}},
    {MODKEY | ControlMask, XK_k, pushup, {0}},
#endif // PUSH_PATCH / PUSH_NO_MASTER_PATCH
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
#if FLEXTILE_DELUXE_LAYOUT
    {MODKEY | ControlMask, XK_i, incnstack, {.i = +1}},
    {MODKEY | ControlMask, XK_u, incnstack, {.i = -1}},
#endif // FLEXTILE_DELUXE_LAYOUT
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_z, zoom, {0}},
#if VANITYGAPS_PATCH
    {MODKEY | Mod1Mask, XK_u, incrgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_u, incrgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_i, incrigaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_i, incrigaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_o, incrogaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_o, incrogaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_6, incrihgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_6, incrihgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_7, incrivgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_7, incrivgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_8, incrohgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_8, incrohgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_9, incrovgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_9, incrovgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_0, togglegaps, {0}},
    {MODKEY | Mod1Mask | ShiftMask, XK_0, defaultgaps, {0}},
#endif // VANITYGAPS_PATCH
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_q, killclient, {0}},
    {MODKEY | ShiftMask, XK_q, quit, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY | ShiftMask, XK_m, setlayout, {0}},
    {MODKEY | ShiftMask, XK_t, togglefloating, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = volupcmd}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = voldowncmd}},
    {0, XF86XK_AudioMute, spawn, {.v = volmutecmd}},
    {0, XF86XK_MonBrightnessUp, spawn, {.v = brightupcmd}},
    {0, XF86XK_MonBrightnessDown, spawn, {.v = brightdowncmd}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5)};

static const Button buttons[] = {
/* click                event mask           button          function argument
 */
#if BAR_STATUSBUTTON_PATCH
    {ClkButton, 0, Button1, spawn, {.v = dmenucmd}},
#endif // BAR_STATUSBUTTON_PATCH
    {ClkLtSymbol, 0, Button1, setlayout, {0}},

    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},

    {ClkWinTitle, 0, Button2, zoom, {0}},
#if BAR_STATUSCMD_PATCH && BAR_DWMBLOCKS_PATCH
    {ClkStatusText, 0, Button1, sigstatusbar, {.i = 1}},
    {ClkStatusText, 0, Button2, sigstatusbar, {.i = 2}},
    {ClkStatusText, 0, Button3, sigstatusbar, {.i = 3}},
#elif BAR_STATUSCMD_PATCH
    {ClkStatusText, 0, Button1, spawn, {.v = statuscmd}},
    {ClkStatusText, 0, Button2, spawn, {.v = statuscmd}},
    {ClkStatusText, 0, Button3, spawn, {.v = statuscmd}},
#else
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
#endif // BAR_STATUSCMD_PATCH

    {ClkClientWin, MODKEY, Button1, movemouse, {0}},

};
