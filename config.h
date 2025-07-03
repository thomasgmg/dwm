/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include <X11/X.h>

#define BROWSER "firefox"
#define FALKON "falkon"

 #define TERMINAL "ghostty"

/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int barpadv	= 7;		/* Vertical padding: how far the bar is from the top */
static const unsigned int barpadh	= 15;		/* Horizontal padding: how far the bar is from each side */
static const unsigned int barheight	= 2;		/* Bar height: How long the bar is vertically */
static const unsigned int barborder	= 3;		/* bar vertical padding (from top)*/
static const unsigned int floatbar	= 1;		/* 0 means bar won't float; float or dock the bar */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 15;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
#define ICONSIZE 16   /* icon size */
#define ICONSPACING 5 /* space between icon and title */
static const char *fonts[] = {"Noto Sans:size=12",
                              "JetBrainsMono Nerd Font:size=12",
                              "Noto Color Emoji:size=12"};
static const char dmenufont[] = "JetBrainsMono Nerd Font:size=12";
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_cyan[] = "#6a5acd";
static const char col_border[] = "#005577";
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_gray3, col_gray1, col_gray2},
    [SchemeSel] = {col_gray4, col_border, col_cyan},
	[SchemeBar]  = { NULL, 		NULL,	   col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
// const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd1[] = {TERMINAL, "--x11-instance-name=spterm", "--title=spterm", "--window-height=15", "--window-width=70", NULL };
const char *spcmd2[] = {"cool-retro-term", NULL };
const char *spcmd3[] = {TERMINAL, "--x11-instance-name=spyazi", "--title=spyazi", "--window-height=25", "--window-width=100","-e", "yazi", NULL };
// const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "ranger", NULL };
// const char *spcmd3[] = {"keepassxc", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
    {"cool-retro-term", spcmd2},
    {"spyazi", spcmd3},
	// {"spranger",    spcmd2},
	// {"keepassxc",   spcmd3},
};

/* tagging */
static const char *tags[] = {" ", " ", " ", " ", "🥶",
                             "👻",   "👾",   "😴",   "🌀"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",	  NULL,			NULL,		0,				1,			 -1 },
	{ "Firefox",  NULL,			NULL,		1 << 8,			0,			 -1 },
	{ NULL,		  "spterm",		NULL,		SPTAG(0),		1,			 -1 },
	{ NULL,		  "spyazi",		NULL,		SPTAG(2),		1,			 -1 },
	// { NULL,		  "keepassxc",	NULL,		SPTAG(2),		0,			 -1 },
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

// #include "grid.c"
// #include "fibonacci.c"
static const Layout layouts[] = {
    /* symbol     arrange function */
    {" ",     tile},  /* first entry is default */
    // {"><>", NULL},  /* no layout function means floating behavior */
    {"🔭 ",    monocle},
 	{ "[🐚]",  spiral },
 	{ "[ ]",  dwindle },
	{ "[ ]",  deck },
	{ "[ ]",  bstack },
	{ "===",   bstackhoriz },
    {" 󱇚  ",   grid},
	{ "###",   nrowgrid },
	{ "---",   horizgrid },
	{ ":::",   gaplessgrid },
	{ "|M|",   centeredmaster },
	{ "<M>",   centeredfloatingmaster },
	{ "><>",   NULL }, /* no layout function means floating behavior */
	{ NULL,    NULL },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,                 TAG)                                                      \
  {MODKEY,                           KEY, view,       {.ui = 1 << TAG}}, \
  {MODKEY | ControlMask,             KEY, toggleview, {.ui = 1 << TAG}}, \
  {MODKEY | ShiftMask,               KEY, tag,        {.ui = 1 << TAG}}, \
  {MODKEY | ControlMask | ShiftMask, KEY, toggletag,  {.ui = 1 << TAG}},
#define STACKKEYS(MOD, ACTION)                                                 \
  {MOD,                XK_j, ACTION##stack, {.i = INC(+1)}}, \
      {MOD,            XK_k, ACTION##stack, {.i = INC(-1)}}, \
      {MOD,            XK_v, ACTION##stack, {.i = 0}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run",           "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL};
static const char *termcmd[] = {"ghostty",               NULL};
static const char *volupcmd[] = {"wpctl",   "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05+",  NULL };
static const char *voldowncmd[] = {"wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05-",  NULL };
static const char *volmutecmd[] = {"wpctl", "set-mute",   "@DEFAULT_AUDIO_SINK@", "toggle", NULL };

static const Key keys[] = {
    /* modifier                     key        function        argument */
    { 0,                            XF86XK_AudioRaiseVolume, spawn,           {.v = volupcmd } },
    { 0,                            XF86XK_AudioLowerVolume, spawn,           {.v = voldowncmd } },
    { 0,                            XF86XK_AudioMute,        spawn,           {.v = volmutecmd } },
    {MODKEY,                        XK_w,                    spawn,           {.v = (const char *[]){BROWSER, NULL}}},
    // {MODKEY | ShiftMask,         XK_f,                    spawn,           {.v = (const char *[]){FALKON,  NULL}}},
    /* { MODKEY,                    XK_s,                    togglesticky,    {0} },                          */
    // { MODKEY,                    XK_d,                    spawn,           {.v = dmenucmd}},
    { MODKEY|ShiftMask,             XK_Return,               spawn,           {.v = termcmd}},
    { MODKEY,                       XK_b,                    togglebar,       {0}},
    { MODKEY,                       XK_j,                    focusstack,      {.i = +1}},
    { MODKEY,                       XK_k,                    focusstack,      {.i = -1}},
    { MODKEY,                       XK_o,                    incnmaster,      {.i = +1}},
    { MODKEY|ShiftMask,             XK_o,                    incnmaster,      {.i = -1}},
    // { MODKEY,                    XK_Left,                 setmfact,        {.f = -0.05}},
    // { MODKEY,                    XK_Right,                setmfact,        {.f = +0.05}},
    { MODKEY,                       XK_space,                zoom,            {0}},
	{ MODKEY|ControlMask,           XK_u,                    incrgaps,        {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_u,                    incrgaps,        {.i = -1 } },
	{ MODKEY|ControlMask,           XK_i,                    incrigaps,       {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_i,                    incrigaps,       {.i = -1 } },
	{ MODKEY|ControlMask,           XK_o,                    incrogaps,       {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_o,                    incrogaps,       {.i = -1 } },
	{ MODKEY|ControlMask,           XK_6,                    incrihgaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_6,                    incrihgaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_7,                    incrivgaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_7,                    incrivgaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_8,                    incrohgaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_8,                    incrohgaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_9,                    incrovgaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_9,                    incrovgaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_0,                    togglegaps,      {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_0,                    defaultgaps,     {0} },
    { MODKEY,                       XK_Tab,                  view,            {0}},
    { MODKEY,                       XK_q,                    killclient,      {0}},
    { MODKEY,                       XK_t,                    setlayout,       {.v = &layouts[0]}},
    { MODKEY,                       XK_m,                    setlayout,       {.v = &layouts[1]}},
    { MODKEY,                       XK_r,                    setlayout,       {.v = &layouts[2]}},
    { MODKEY|ShiftMask,             XK_r,                    setlayout,       {.v = &layouts[3]}},
    { MODKEY,                       XK_d,                    setlayout,       {.v = &layouts[4]}},
    { MODKEY,                       XK_g,                    setlayout,       {.v = &layouts[7]}},
    { MODKEY|ShiftMask,             XK_g,                    setlayout,       {.v = &layouts[10]}},
    { MODKEY,                       XK_c,                    setlayout,       {.v = &layouts[11]}},
    { MODKEY|ShiftMask,             XK_c,                    setlayout,       {.v = &layouts[12]}},
	{ MODKEY,                       XK_r,                    setlayout,       {.v = &layouts[6]} },
	{ MODKEY|ShiftMask,             XK_r,                    setlayout,       {.v = &layouts[7]} },
	{ MODKEY|ControlMask,           XK_comma,                cyclelayout,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period,               cyclelayout,     {.i = +1 } },
    /*{ MODKEY,                     XK_space,                setlayout,       {0}},                           */
    {MODKEY|ShiftMask,              XK_space,                togglefloating,  {0}},
	{ MODKEY,                       XK_Down,                 moveresize,      {.v = "0x 50y 0w 0h" } },
	{ MODKEY,                       XK_Up,                   moveresize,      {.v = "0x -50y 0w 0h" } },
	{ MODKEY,                       XK_Right,                moveresize,      {.v = "50x 0y 0w 0h" } },
	{ MODKEY,                       XK_Left,                 moveresize,      {.v = "-50x 0y 0w 0h" } },
	{ MODKEY|ShiftMask,             XK_Down,                 moveresize,      {.v = "0x 0y 0w 50h" } },
	{ MODKEY|ShiftMask,             XK_Up,                   moveresize,      {.v = "0x 0y 0w -50h" } },
	{ MODKEY|ShiftMask,             XK_Right,                moveresize,      {.v = "0x 0y 50w 0h" } },
	{ MODKEY|ShiftMask,             XK_Left,                 moveresize,      {.v = "0x 0y -50w 0h" } },
	{ MODKEY|ControlMask,           XK_Up,                   moveresizeedge,  {.v = "t"} },
	{ MODKEY|ControlMask,           XK_Down,                 moveresizeedge,  {.v = "b"} },
	{ MODKEY|ControlMask,           XK_Left,                 moveresizeedge,  {.v = "l"} },
	{ MODKEY|ControlMask,           XK_Right,                moveresizeedge,  {.v = "r"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Up,                   moveresizeedge,  {.v = "T"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Down,                 moveresizeedge,  {.v = "B"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Left,                 moveresizeedge,  {.v = "L"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Right,                moveresizeedge,  {.v = "R"} },
    {MODKEY,                        XK_0,                    view,            {.ui = ~0}},
    {MODKEY|ShiftMask,              XK_0,                    tag,             {.ui = ~0}},
    {MODKEY,                        XK_h,                    focusmon,        {.i = -1}},
    {MODKEY,                        XK_l,                    focusmon,        {.i = +1}},
    {MODKEY|ShiftMask,              XK_l,                    tagmon,          {.i = -1}},
    {MODKEY|ShiftMask,              XK_h,                    tagmon,          {.i = +1}},
	{MODKEY,                        XK_Return,               togglescratch,   {.ui = 0 } },
	{MODKEY|ControlMask,            XK_c,                    togglescratch,   {.ui = 1 } },
	{MODKEY|ControlMask,            XK_d,                    togglescratch,   {.ui = 2 } },
    TAGKEYS(XK_1,                   0) TAGKEYS(XK_2,         1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4,                3)
    TAGKEYS(XK_5,                   4) TAGKEYS(XK_6,         5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8,                7)
    TAGKEYS(XK_9,                   8){MODKEY | ShiftMask,   XK_q,            quit,                           {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol,   0,      Button1, setlayout,      {0}},
    {ClkLtSymbol,   0,      Button3, setlayout,      {.v = &layouts[2]}},
    {ClkWinTitle,   0,      Button2, zoom,           {0}},
    {ClkStatusText, 0,      Button2, spawn,          {.v = termcmd}},
    {ClkClientWin,  MODKEY, Button1, movemouse,      {0}},
    {ClkClientWin,  MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin,  MODKEY, Button1, resizemouse,    {0}},
    {ClkTagBar,     0,      Button1, view,           {0}},
    {ClkTagBar,     0,      Button3, toggleview,     {0}},
    {ClkTagBar,     MODKEY, Button1, tag,            {0}},
    {ClkTagBar,     MODKEY, Button3, toggletag,      {0}},
};
