/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <X11/X.h>

#define BROWSER "firefox"
#define THUNAR "thunar"
#define TERMINAL "kitty"
#define GHOSTTY "ghostty"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
#define ICONSIZE bh   /* icon size */
#define ICONSPACING (bh -4) /* space between icon and title */
static const int vertpad            = 10;        /* vertical padding of bar */
static const int sidepad            = 10;       /* horizontal padding of bar */
static const char *fonts[]          = { "monospace:size=11" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_black[]       = "#000000";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_border[]      = "#00ffff";
static const char col_inv[]         = "#ffe4e1";
static const char col_salmon[]      = "#f08080";
static const char col_hotpink[]     = "#ff69b4";
static const char col_borderpink[]  = "#ffa0ad";

/* solarized colors http://ethanschoonover.com/solarized */
static const char s_base03[]        = "#002b36";
static const char s_base02[]        = "#073642";
static const char s_base01[]        = "#586e75";
static const char s_base00[]        = "#657b83";
static const char s_base0[]         = "#839496";
static const char s_base1[]         = "#93a1a1";
static const char s_base2[]         = "#eee8d5";
static const char s_base3[]         = "#fdf6e3";

static const char *colors[][SchemeN][3]      = {
	/*               fg         bg          border   */
    { /* Normal colorscheme light (use with wallhaven wall) */
	[SchemeNorm] = { col_gray3, col_gray1,  col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3,  col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan, col_border },
    },
    { /* Normal colorscheme dark (use with wallhaven-kumar or wallhaven-DragonTemple wall) */
	[SchemeNorm] = { col_gray3, col_gray1,  col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3,  col_gray2 },
	[SchemeSel]  = { col_black, col_cyan, col_border },
    },
    { /* Light pink theme (use with Shadows wall) */
	[SchemeNorm] = { col_gray3, col_gray1,  col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3,  col_gray2 },
	[SchemeSel]  = { col_gray1, col_salmon, col_borderpink },
    },
    { /* Hotpink theme (the following two colorschemes can be used with wallhaven-kanji wall) */
	[SchemeNorm] = { col_gray3, col_gray1,  col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3,  col_gray2 },
	[SchemeSel]  = { col_gray1, col_hotpink, col_borderpink },
    },
    { /* Black theme (use with Red_Eclipse wall) */
	[SchemeNorm] = { col_gray3, col_gray1,  col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3,  col_gray2 },
	[SchemeSel]  = { col_gray4, col_black, col_borderpink },
    },
};
static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};	/* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
static const char *spcmd1[] = {TERMINAL, "--name", "spterm", "--title", "spterm", NULL };
static const char *spcmd2[] = {GHOSTTY, "--x11-instance-name=spfm", "--title=spfm", "--window-height=35", "--window-width=150", "-e", "yazi", NULL };
static const char *spcmd3[] = {GHOSTTY, "--x11-instance-name=tty-clock", "--title=tty-clock", "--window-height=10", "--window-width=38", "-e", "tty-clock", NULL };
static Sp scratchpads[] = {
	/*    name           cmd  */
	{"spterm",spcmd1},
    {"spfm",spcmd2},
    {"tty-clock",spcmd3},
};

/* tagging */
// static const char *tags[] = { "", "󰖟", "", "", ""};
// static const char *tags[] = { "1", "2", "3", "4", "5"};
static const char *tags[] = { "I", "II", "III", "IV", "V"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance      title       tags mask       isfloating   monitor */
	{ "Gimp",     NULL,         NULL,       0,              1,           -1 },
	{ NULL,       "spterm",     NULL,       SPTAG(0),       1,           -1 },
	{ NULL,       "spfm",       NULL,       SPTAG(1),       1,           -1 },
	{ NULL,       "tty-clock",  NULL,       SPTAG(2),       1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "  ",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[@]",      spiral },
	{ " 𝜙 ",      dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ "<M>",      centeredfloatingmaster },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { TERMINAL, NULL };
static const char *ghosttycmd[]  = { GHOSTTY, NULL };
static const char *webcmd[]  = { BROWSER, NULL };
static const char *fmcmd[]  = { THUNAR, NULL };
static const char *layoutmenu_cmd = "layoutmenu.sh";
static const char *volupcmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05+", NULL };   
static const char *voldowncmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05-", NULL };
static const char *volmutecmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };
static const char *wall1cmd[] = { "feh", "--bg-scale", "/home/thomas/Wallpapers/wallhaven.jpg", NULL };
static const char *wall2cmd[] = { "feh", "--bg-scale", "/home/thomas/Wallpapers/wallhaven-DragonTemple.jpg", NULL };
static const char *wall3cmd[] = { "feh", "--bg-scale", "/home/thomas/Wallpapers/Shadows.png", NULL };
static const char *wall4cmd[] = { "feh", "--bg-scale", "/home/thomas/Wallpapers/wallhaven-kanji.jpg", NULL };
static const char *wall5cmd[] = { "feh", "--bg-scale", "/home/thomas/Wallpapers/Red_Eclipse.png", NULL };

#include "movestack.c"
static const Key keys[] = {
	/* modifier                     key                      function        argument */
    { 0,                            XF86XK_AudioRaiseVolume, spawn,          {.v = volupcmd } },
    { 0,                            XF86XK_AudioLowerVolume, spawn,          {.v = voldowncmd } },
    { 0,                            XF86XK_AudioMute,        spawn,          {.v = volmutecmd } },
    { MODKEY,                       XK_F1,                   spawn,          {.v = wall1cmd } },
    { MODKEY,                       XK_F2,                   spawn,          {.v = wall2cmd } },
    { MODKEY,                       XK_F3,                   spawn,          {.v = wall3cmd } },
    { MODKEY,                       XK_F4,                   spawn,          {.v = wall4cmd } },
    { MODKEY,                       XK_F5,                   spawn,          {.v = wall5cmd } },
	/*{ MODKEY,                     XK_p,                    spawn,          {.v = dmenucmd } },*/
	{ MODKEY|ShiftMask,             XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_g,                    spawn,          {.v = ghosttycmd } },
    { MODKEY,                       XK_w,                    spawn,          {.v = webcmd } },
    { MODKEY|ShiftMask,             XK_y,                    spawn,          {.v = fmcmd } },
	{ MODKEY,                       XK_b,                    togglebar,      {0} },
	{ MODKEY,                       XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,                    incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_comma,                setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_period,               setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,                    movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,                    movestack,      {.i = -1 } },
	{ MODKEY,                       XK_space,                zoom,           {0} },
	{ MODKEY|Mod1Mask,              XK_u,                    incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_u,                    incrgaps,       {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_i,                    incrigaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,                    incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,                    incrogaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,                    incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,                    incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,                    incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,                    incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,                    incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,                    incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,                    incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,                    incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,                    incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_0,                    togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,                    defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,                  view,           {0} },
	{ MODKEY,                       XK_q,                    killclient,     {0} },
	{ MODKEY,                       XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,           XK_m,                    layoutmenu,     {0} },
	{ MODKEY|ControlMask,           XK_comma,                cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period,               cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_space,                togglefloating, {0} },
	{ MODKEY,                       XK_Down,                 moveresize,     {.v = "0x 50y 0w 0h" } },
	{ MODKEY,                       XK_Up,                   moveresize,     {.v = "0x -50y 0w 0h" } },
	{ MODKEY,                       XK_Right,                moveresize,     {.v = "50x 0y 0w 0h" } },
	{ MODKEY,                       XK_Left,                 moveresize,     {.v = "-50x 0y 0w 0h" } },
	{ MODKEY|ShiftMask,             XK_Down,                 moveresize,     {.v = "0x 0y 0w 25h" } },
	{ MODKEY|ShiftMask,             XK_Up,                   moveresize,     {.v = "0x 0y 0w -25h" } },
	{ MODKEY|ShiftMask,             XK_Right,                moveresize,     {.v = "0x 0y 25w 0h" } },
	{ MODKEY|ShiftMask,             XK_Left,                 moveresize,     {.v = "0x 0y -25w 0h" } },
	{ MODKEY|ControlMask,           XK_Up,                   moveresizeedge, {.v = "t"} },
	{ MODKEY|ControlMask,           XK_Down,                 moveresizeedge, {.v = "b"} },
	{ MODKEY|ControlMask,           XK_Left,                 moveresizeedge, {.v = "l"} },
	{ MODKEY|ControlMask,           XK_Right,                moveresizeedge, {.v = "r"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Up,                   moveresizeedge, {.v = "T"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Down,                 moveresizeedge, {.v = "B"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Left,                 moveresizeedge, {.v = "L"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Right,                moveresizeedge, {.v = "R"} },
	{ MODKEY,                       XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_h,                    focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_l,                    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_h,                    tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_l,                    tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_s,                    setscheme,      {.i = +1 } },
	{ MODKEY,                       XK_Return,               togglescratch,  {.ui = 0 } },
    { MODKEY,                       XK_y,                    togglescratch,  {.ui = 1 } },
    { MODKEY|ControlMask,           XK_t,                    togglescratch,  {.ui = 2 } },
	{ MODKEY,                       XK_x,                    movecenter,     {0} },
	TAGKEYS(                        XK_1,                                  0)
	TAGKEYS(                        XK_2,                                  1)
	TAGKEYS(                        XK_3,                                  2)
	TAGKEYS(                        XK_4,                                  3)
	TAGKEYS(                        XK_5,                                  4)
	{ MODKEY|ShiftMask,             XK_q,                    quit,           {0} },
	{ MODKEY,                       XK_s,                    togglesticky,   {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button3,        layoutmenu,     {0} }, /* spawn layoutmenu by clicking on the layout icon */
    { ClkWinTitle,          0,              Button3,        spawn,          {.v = (const char *[]){BROWSER, NULL}}}, /* spawn browser by clicking on the window title area */
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } }, /* spawn terminal by clicking on dwmblocks */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} }, /* resize window with mouse by clicking on window */
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} }, /* resize window with mouse by clicking on window */
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} }, /* toggle floating by clicking on window */
	{ ClkTagBar,            0,              Button1,        view,           {0} }, /* go to selected tag */
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} }, /* show clients of selected tag in your current tag */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} }, /* move client to selected tag */
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} }, /* send client to selected tag and also keep it in your current tag */
};

