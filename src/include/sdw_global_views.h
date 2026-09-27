/* Blocks of globals that are fields of one object, shared by T270 and T293. The type names are descriptive, not recovered. */
/* BYTES: bss-name. */
/* BYTES(bss-name): CamScriptState, TextPort, TextScroll (aggregate views): aggregate view: these globals are one object so the .bss keeps their order and alignment (see text.cpp / camera.cpp) */
#ifndef SDW_GLOBAL_VIEWS_H
#define SDW_GLOBAL_VIEWS_H

struct CamScriptState { /* 0x6e4398 */
    Vec3s eye;          /* +0x00 g_camScriptEye */
    Vec3s rot;          /* +0x06 g_camScriptRot */
    s16 focal;          /* +0x0c g_camScriptFocal */
    Vec3s prevEye;      /* +0x0e g_camScriptPrevEye */
    u8 returnMode;      /* +0x14 g_camScriptReturnMode */
    Vec3s prevRot;      /* +0x16 g_camScriptPrevRot */
    s16 prevFocal;      /* +0x1c g_camScriptPrevFocal */
    Vec3s preEye;       /* +0x1e g_camPreScriptEye */
    s16 preFocal;       /* +0x24 g_camPreScriptFocal */
    Vec3s preRot;       /* +0x26 g_camPreScriptRot */
};

struct TextPort {
    u32 *layer;                 /* +0x00 g_textLayer: the 2D layer handle Draw2D_LayerToZ turns into a depth */
    s16 winX, winY, winW, winH; /* +0x04 g_textWinX/Y/W/H */
    s16 cursorX, cursorY;       /* +0x0c g_textCursorX/Y (signed: every reader movsx) */
    s16 clipOffX, clipOffY;     /* +0x10 g_textClipOffX/Y */
    u32 noClip;                 /* +0x14 g_textNoClip */
};

struct TextScroll {
    char *begin;    /* +0x00 g_scrollTextBegin: first page marker of the text being scrolled */
    char *source;   /* +0x04 g_scrollTextSource: the text ScrollText_Run was last given */
    char *page;     /* +0x08 g_scrollTextPage: start of the page on screen */
    char *nextPage; /* +0x0c g_scrollTextNextPage: where the page on screen ends */
    union {
        u32 flags;                   /* +0x10 g_scrollTextFlags */
        ScrollTextFlagBits flagBits; /* +0x10 the same word as ScrollTextFlagBits */
    };
    s32 startMs;   /* +0x14 g_scrollTextStartMs: g_rawTimeMs when the text started; written only */
    s32 pageMs;    /* +0x18 g_scrollTextPageMs: display time of the page, ms; 0 = not parsed yet */
    s32 remainMs;  /* +0x1c g_scrollTextRemainMs: display time left, ms */
    s32 inputTime; /* +0x20 g_scrollTextInputTime: g_rawTime of the last scroll input (auto-repeat) */
};

#endif
