/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 6;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char buttonbar[]       = "";
static const unsigned int stairpx   = 20;       /* depth of the stairs layout */
static const int stairdirection     = 1;        /* 0: left-aligned, 1: right-aligned */
static const int stairsamesize      = 1;        /* 1 means shrink all the staired windows to the same size */
#define ICONSIZE 16   /* icon size */
#define ICONSPACING 5 /* space between icon and title */
/* gruvbox color theme */
static const char gruvbox_bg[] = "#282828";
static const char gruvbox_fg[] = "#ebdbb2";
static const char gruvbox_red[] = "#cc241d";
static const char gruvbox_green[] = "#98971a";
static const char gruvbox_yellow[] = "#d79921";
static const char gruvbox_blue[] = "#458588";
static const char gruvbox_purple[] = "#b16286";
static const char gruvbox_aqua[] = "#689d6a";
static const char gruvbox_gray[] = "#a89984";
static const char gruvbox_orange[] = "#d65d0e";

static const char *fonts[] = {
    "JetBrainsMono Nerd Font:size=10",
	"PingFangJetBrains:size=10",      // fonts for title contains chinese font.
};
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][4]      = {
	/*               fg         bg         border     float */
	[SchemeNorm] = { gruvbox_fg, gruvbox_bg, col_gray2, col_gray2 },
	[SchemeSel] =  { gruvbox_fg, gruvbox_orange,  gruvbox_orange, col_gray1 },
	[SchemeHid]  = { gruvbox_orange, col_gray1, col_gray2, col_cyan },
};
static const unsigned int alphas[][3]      = {
    /*               fg      bg        border*/
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};
/*
 * Allow dwm to execute commands from autostart array in your config.h file.
 * And when you exit dwm all processes from autostart array will be killed.
*/
static const char *const autostart[] = {
    "compfy", NULL,
    "feh", "--bg-fill", "/home/butteria/Pictures/wallpapers/dwm_bg.png", NULL,
    "fcitx5", NULL,
    "flameshot", NULL,
	NULL /* terminate */
};

static const char *tagsel[][2] = {
   /*   fg         bg    */
  { gruvbox_fg, gruvbox_bg }, /* norm */
  { gruvbox_bg, gruvbox_orange  }, /* sel */
  { gruvbox_bg, gruvbox_fg }, /* occ but not sel */
  { gruvbox_bg, gruvbox_purple }, /* has pinned tag */
};
/* tagging */
// "⁰","¹","²","³","⁴","⁵","⁶","⁷","⁸","⁹","¹⁰"]'
static const char *tags[] = { "", "", "", "", "", "", "", "8", "9" };
static const char *tagsalt[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const int momentaryalttags = 0; /* 1 means alttags will show only when key is held down*/

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "kitty",   NULL,     NULL,           1,         0,          1,           0,        -1 },
	{ "firefox", NULL,     NULL,           1 << 1,    0,          0,          -1,        -1 },
	{ "Steam",   NULL,     NULL,           1 << 3,    0,          0,           0,        -1 },
    { "weixin",  NULL,     NULL,           1 << 4,    1,          0,           1,        -1 },
	{ "qq",      NULL,     NULL,           1 << 4,    1,          0,           1,        -1 },
	{ "MATLAB",  NULL,     NULL,           1 << 5,    0,          0,           1,        -1 },
	{ "Gimp",    NULL,     NULL,           1 << 5,    0,          0,           1,        -1 },
	{ "mpv",     NULL,     NULL,           0,         1,          0,           1,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[S]",      stairs },
};

/* key definitions */
// Mod4Mask is WIN key
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *launchercmd[] = { "rofi", "-no-lazy-grab", "-show", "drun", "-modi", "drun", "-theme", "~/.config/rofi/launcher.rasi", NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *layoutmenu_cmd = "layoutmenu.sh";
static const char *up_vol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",   NULL  };
static const char *down_vol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",   NULL  };
static const char *mute_vol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle", NULL  };
static const char *brighter[] = { "brightnessctl", "set", "5%+", NULL  };
static const char *dimmer[]   = { "brightnessctl", "set", "5%-", NULL  };

#include "exitdwm.c"
static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = launchercmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_q,      killclient,     {0} },
    { MODKEY|ShiftMask,             XK_q,      spawn,          SHCMD("kill -9 $(xprop | grep _NET_WM_PID | awk '{print $3}')") },
	{ MODKEY,                       XK_Escape, exitdwm,        {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_s,      togglesticky,   {0} },
	{ MODKEY,                       XK_n,      togglealttag,   {0} },
    /* Adjusting volume */
    { 0, XF86XK_AudioMute,        spawn, {.v = mute_vol }  },
    { 0, XF86XK_AudioLowerVolume, spawn, {.v = down_vol }  },
    { 0, XF86XK_AudioRaiseVolume, spawn, {.v = up_vol }  },
    /* Adjusting brightness */
    { 0, XF86XK_MonBrightnessDown, spawn, {.v = dimmer }  },
    { 0, XF86XK_MonBrightnessUp,   spawn, {.v = brighter }  },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkButton,		    0,		        Button1,	    spawn,		    SHCMD("jgmenu_run")},
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkRootWin,           0,              Button3,        spawn,          SHCMD("jgmenu_run")},
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button3,        killclient,     {0} },
	{ ClkLtSymbol,          0,              Button3,        layoutmenu,     {0} },
};

