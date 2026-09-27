/*
 * T287, guessed original file MCard.cpp: the memory-card screens.
 * match-init: MCard_InitRectBody_thunk MCard_InitRectChoiceLeft_thunk MCard_InitRectChoiceRight_thunk MCard_InitRectChoiceWide_thunk MCard_InitRectFooter_thunk
 * match-addr: $S1=0x6e3674 $S2=0x6e3698
 *   .text  0x54a5f0-0x54d4fa (the five rectangle initialisers first, .CRT$XCU 0x5790a4-0x5790b4)
 *   .rdata 0x577170-0x5771bc (g_mcardIconGroup, g_mcardIconFrame, saveTitle + 2 pad)
 *   .data  0x57c838-0x57e6cc (saveIconData, the rectangles, g_mcardSlotNav, the literals of g_mcardStrings, the table,
 *          then the functions' literals)
 *   .bss   0x6e3670-0x6e38b0
 * Contents, in address order: the rectangles, the helpers, the card image storage, init, the UI footers, the cursor,
 * the slot thumbnails, the state machine, the screens, the slot chooser and
 * MCard_Stub_54d4f5 0x54d4f5. The tables are defined from the exe bytes (tools/data_init.py). MCard_SetMode precedes
 * MCard_GetString in the object, so the latter is declared first.
 *
 * Data representation (devices, not claims about the original spelling):
 * - saveIconData is not const: the exe has it in .data (0x57c838, 8-aligned), unlike saveTitle, which is in .rdata.
 *   g_mcardIconGroup / g_mcardIconFrame are const (.rdata).
 * - .bss order. VC6 puts a file's uninitialised globals (and those with constructors) first, ordered by a hash of their
 *   names ((h ^ h >> 16) & 0x3ff, h = h*4 + (h >> 4) + c), then, in definition order, the ones explicitly initialised
 *   to zero and the dynamically initialised aggregates (g_mcardRectChoiceRight). Arrays of 64 bytes or more are
 *   8-aligned, smaller arrays 4-aligned, scalars at their natural alignment. The exe's g_cardBlocks (176 bytes) sits at
 *   0x6e37a4, 4 mod 8, inside the 0x200-byte PS1 card image that starts with g_saveCardHeader: so the image was one
 *   object in the original, and the fields are laid out here as members of an anonymous union (VC6 names it $S2;
 *   g_saveCardHeader spans the whole image, [0x200], as Crc32 and Card_ReadBlocks use it). The mode / cursor / icon
 *   globals 0x6e3674-0x6e3698 are a second anonymous union ($S1): their names' hash keys are not in address order,
 *   and they must follow g_mcardFileName (key 472, hashed, first) and precede the image; $S1 / $S2 hash to 979 / 980.
 *   The members carry the globals' names and have internal linkage (no other object refers to them). The globals
 *   after the image are initialised to zero so that they follow in definition order, before g_mcardRectChoiceRight.
 * - 0x6e38ac-0x6e38b0: four bytes no instruction refers to, after g_mcardRectChoiceRight and before the next object's
 *   8-aligned .bss (T290 would start at 0x6e38ac otherwise). Named by address.
 */
/* BYTES: dead-code, layout, slot-group, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): not const: the original has it in .data */
#include "sdw_types.h"
#define SDW_MEMBERS_Progress         \
    inline void SetCardOpen(s32 on); \
    void SetSaveTimestamp(u32 stamp) \
    {                                \
        saveTimestamp = stamp;       \
    }                                \
    void SetSaveSlot(s32 slot)       \
    {                                \
        saveSlot = (u8)slot;         \
    }                                \
    void SetSaveSlotValid(s32);      \
    void SetAutoSaveOn(s32);         \
    u32 GetSaveTimestamp()           \
    {                                \
        return saveTimestamp;        \
    }
#include "sdw_classes.h"
#define SDW_INLINE_PROGRESS_GETLANGUAGE 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_GETLANGUAGE
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_global_views.h"

/* ==== .rdata 0x577170-0x5771bc ==== */
/* 0x577170 / 0x577184 - per card-icon index: the icon group (0 or 1) and the frame in it (Card_DrawScreen's ICON). */
extern const u8 g_mcardIconGroup[20] = {0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1};
extern const u8 g_mcardIconFrame[20] = {0, 1, 0, 2, 3, 1, 2, 3, 4, 5, 6, 4, 5, 6, 7, 7, 8, 9, 8, 9};
/* 0x577198 - the PS1 save title, Shift-JIS */
static const u8 saveTitle[0x22] = {0x82, 0x72, 0x82, 0x67, 0x82, 0x64, 0x82, 0x64, 0x82, 0x6f, 0x81, 0x40,
                                   0x82, 0x63, 0x82, 0x6e, 0x82, 0x66, 0x81, 0x4c, 0x82, 0x8e, 0x81, 0x40,
                                   0x82, 0x76, 0x82, 0x6e, 0x82, 0x6b, 0x82, 0x65, 0,    0};

/* ==== .data 0x57c838- ==== */
/* 0x57c838 - the PS1 save icon: palette (0x20 bytes) then bitmap (0x80), adjacent in the original; one array so the
 * compiler inserts no alignment between them. */
static u8 saveIconData[0xa0] = {
    0x00, 0x00, 0xf3, 0x21, 0xcb, 0x1c, 0xda, 0x12, 0xb1, 0x04, 0xd9, 0x4e, 0x85, 0x10, 0x97, 0x46, 0x90, 0x1d,
    0x3b, 0x53, 0x1f, 0x17, 0x43, 0x08, 0xc8, 0x14, 0x16, 0x26, 0x5e, 0x57, 0x0a, 0x21, 0x00, 0xb0, 0x00, 0xb0,
    0x0b, 0x00, 0x00, 0x00, 0x00, 0x56, 0x06, 0x8b, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x56, 0xb1, 0x60, 0xd5, 0x0b,
    0x00, 0x00, 0x00, 0x60, 0x19, 0x0b, 0x56, 0xb1, 0x00, 0x00, 0x00, 0xb0, 0xe5, 0xb1, 0xc0, 0x19, 0x0b, 0x00,
    0x00, 0x00, 0x56, 0x19, 0xbb, 0xd5, 0xb8, 0x00, 0x00, 0x00, 0xb0, 0x7c, 0x0c, 0x86, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x60, 0xb8, 0x86, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3b, 0x4a, 0x0b, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xb4, 0xd4, 0x67, 0x0b, 0xb6, 0x00, 0xb0, 0x77, 0x62, 0x5f,
    0xee, 0xb7, 0x65, 0xbb, 0x2b, 0xff, 0xe7, 0x75, 0x5f, 0x69, 0x6b, 0x5d, 0xe9, 0x55, 0x9e, 0x8c, 0x87, 0xc5,
    0x00, 0x5c, 0x55, 0x99, 0xf7, 0x7b, 0x59, 0xb5, 0x00, 0xb0, 0xbb, 0xb6, 0x7f, 0xc1, 0xcb, 0x06};

/* ==== .bss 0x6e3670-0x6e38b0 (see the header for the order) ==== */
const char *g_mcardFileName; /* 0x6e3670  "SdwDatas.BKS", set by MCard_Init */
static union {               /* $S1 0x6e3674 */
    struct {
        void **g_mcardIconGroups[2]; /* 0x6e3674 */
        s32 g_mcardDelayMs;          /* 0x6e367c */
        u16 g_mcardIconW1;           /* 0x6e3680 */
        u16 g_mcardIconH1;           /* 0x6e3682 */
        u16 g_mcardIconReserved[2];  /* 0x6e3684 */
        u16 g_mcardFreeBlocks;       /* 0x6e3688 */
        u8 g_mcardActiveMode;        /* 0x6e368a */
        u8 g_mcardNavSkipState;      /* 0x6e368b */
        u8 g_mcardChooserVariant;    /* 0x6e368c */
        u8 g_mcardState;             /* 0x6e368d */
        s32 g_mcardResult;           /* 0x6e3690 */
        u8 g_mcardFade;              /* 0x6e3694 */
    };
};
static union {                  /* $S2 0x6e3698: the 0x200-byte PS1 card image */
    u8 g_saveCardHeader[0x200]; /* 0x6e3698  'SC' header, title, CLUT, icon, then the save data */
    struct {
        u8 saveImageFrames[0x100]; /* +0x000  header + icon frames (g_saveCardHeader) */
        u32 g_saveImageTimestamp;  /* +0x100  0x6e3798 */
        u16 g_saveImageStatus;     /* +0x104  0x6e379c */
        s8 g_saveLastSlot;         /* +0x106  0x6e379e */
        u8 saveImagePad107;        /* +0x107 */
        char g_cardBlockState[4];  /* +0x108  0x6e37a0  'E' empty / 'F' full */
        u8 g_cardBlocks[4][0x2c];  /* +0x10c  0x6e37a4  the four saved progress records */
        u32 g_saveImageCrc;        /* +0x1bc  0x6e3854  CRC32 of the image with this field zeroed */
    }; /* +0x1c0-0x200 unreferenced */
};
const char *(*g_pGetUiString)(u32 id) = 0; /* 0x6e3898 */
s32 g_mcardScreen = 0;                     /* 0x6e389c */
u8 g_mcardPhase = 0;                       /* 0x6e38a0 */
u8 g_mcardImageCached = 0;                 /* 0x6e38a1 */
u8 g_mcardMode = 0;                        /* 0x6e38a2 */

/* ==== the card-screen rectangles and their five static initialisers (ten CRT functions: a thunk and an initialiser
 * each); ChoiceRight is dynamically initialised whole and lands in .bss 0x6e38a4, the others in .data 0x57c8d8.. ==== */
inline s32 MCardScreenWidth()
{
    return 512;
}

s16 g_mcardRectBody[4] = {48, 8, MCardScreenWidth() - 96, 156};
s16 g_mcardRectChoiceLeft[4] = {32, 168, (MCardScreenWidth() - 64) / 2, 40};
s16 g_mcardRectChoiceRight[4] = {(s16)((MCardScreenWidth() - 64) / 2) + 32, 168, (MCardScreenWidth() - 64) / 2, 40};
s16 g_mcardRectChoiceWide[4] = {32, 168, MCardScreenWidth() - 96, 40};
s16 g_mcardRectFooter[4] = {16, 202, MCardScreenWidth() - 32, 40};

u32 g_mcardBss_6e38ac = 0; /* 0x6e38ac  unreferenced (see the header); descriptive name */

/* 0x57c8f8 - the cursor's next slot, [slot-1][direction]: slots are one-based (MCard_FindSlotWithState / StepCursor) */
u8 g_mcardSlotNav[4][2] = {{4, 2}, {1, 3}, {2, 4}, {3, 1}};

/* 0x57e298 - the card screens' texts, [language][id] (MCard_GetString); their literals precede the table in .data
 * (0x57c900-0x57e293). The language is Progress.language: English, Spanish, Italian, Portuguese. */
const char *g_mcardStrings[4][51] = {
    "looking for saved game...\nplease wait.",
    "version PC cha\xeene -Format Card ?- devrait \xeatre inutile",
    "would you like to create a save file?",
    "create new game:\nautomatic save on.",
    "load game:\nautomatic save on.",
    "Save game",
    "Are you sure you want to save this game?",
    "are you sure you want to overwrite this game?\nautomatic save on.",
    "version PC cha\xeene -Formatting - devrait \xeatre inutile",
    "creating save file...\nplease wait.",
    "loading data...\nplease wait.",
    "Saving data...\nplease wait.",
    "Overwriting data...\nplease wait.",
    "Autosaving data...\nplease wait.",
    "version PC cha\xeene -Format ok - devrait \xeatre inutile",
    "load completed.\nautomatic save on.",
    "Save completed.",
    "Overwrite completed.",
    "version PC cha\xeene -Format Failed - devrait \xeatre inutile",
    "creation failed!\nplease try again.",
    "load failed!\nplease try again.",
    "save failed!\nplease try again.",
    "overwrite failed!\nplease try again.",
    "version PC cha\xeene -No memory card- devrait \xeatre inutile",
    "version PC cha\xeene -Full card- devrait \xeatre inutile",
    "save file not found.",
    "start a new game?\nautomatic save off.",
    "version PC cha\xeene -Damaged- devrait \xeatre inutile",
    "data may be corrupt!\nplease try again.",
    "Data save cancelled!",
    "Autosave completed!",
    "Autosave failed!\nAutosave disabled.",
    "version PC cha\xeene -AutoSav unformat- devrait \xeatre inutile",
    "version PC cha\xeene -AutoSav nocard- devrait \xeatre inutile",
    "version PC cha\xeene -AutoSav damaged- devrait \xeatre inutile",
    "Data may be corrupt!\nAutosave disabled.",
    "cancel",
    "retry",
    "yes",
    "no",
    "ok",
    "empty",
    "start game",
    "new game",
    "back",
    "load",
    "save",
    "game",
    "%d sheep to catch",
    "bonus",
    "save file",
    "BUSCANDO UNA PARTIDA GUARDADA...\nESPERA UNOS INSTANTES.",
    "unused - Format Card ? -",
    "\xbfQUIERES CREAR UN ARCHIVO DE GUARDADO?",
    "crear nueva partida:\nguardado autom\xe1tico activado.",
    "cargar partida:\nguardado autom\xe1tico activado.",
    "Guardar partida:",
    "\xbfseguro que quieres guardar esta partida?",
    "\xbfSEGURO QUE QUIERES SOBREESCRIBIR LA PARTIDA?\nGUARDADO AUTOM\xc1TICO ACTIVADO.",
    "unused - Formatting -",
    "CREANDO UN ARCHIVO DE GUARDADO...\nESPERA UNOS INSTANTES.",
    "CARGANDO DATOS...\nESPERA UNOS INSTANTES.",
    "guardando datos...\nESPERA UNOS INSTANTES.",
    "sobreescribiendo datos...\nESPERA UNOS INSTANTES.",
    "guardando datos autom\xe1ticamente...\nESPERA UNOS INSTANTES.",
    "unused - Format ok -",
    "carga finalizada.\nguardado autom\xe1tico activado.",
    "guardado realizado.",
    "sobreescritura realizada.",
    "unused - Format Failed -",
    "\xa1"
    "ERROR AL CREAR EL ARCHIVO!\nINT\xc9NTALO DE NUEVO.",
    "\xa1"
    "ERROR AL CARGAR!\nINT\xc9NTALO DE NUEVO.",
    "\xa1"
    "ERROR AL GUARDAR!\nINT\xc9NTALO DE NUEVO.",
    "\xa1"
    "ERROR AL SOBRESCRIBIR!\nINT\xc9NTALO DE NUEVO.",
    "unused - No memory card -",
    "unused - Full card -",
    "NO SE HA ENCONTRADO NINGUNA PARTIDA GUARDADA.",
    "\xbfiniciar nueva partida?\nguardado autom\xe1tico desactivado.",
    "unused - Damaged -",
    "\xa1LOS DATOS PUEDEN ESTAR DA\xd1"
    "ADOS!\nINTENTA GUARDAR DE NUEVO.",
    "\xa1guardado de datos cancelado!",
    "\xa1guardado autom\xe1tico realizado!",
    "\xa1"
    "error al guardar autom\xe1ticamente!\nguardado autom\xe1tico desactivado.",
    "unused - Autosav unformat -",
    "unused - Autosav nocard -",
    "unused - Autosav damaged -",
    "\xa1los datos podr\xed"
    "an estar da\xf1"
    "ados!\nGuardado autom\xe1tico desactivado.",
    "cancelar",
    "reintentar",
    "s\xed",
    "no",
    "aceptar",
    "Vac\xed"
    "a",
    "comenzar partida",
    "nueva partida",
    "volver",
    "cargar",
    "guardar",
    "partida",
    "%d oveja(s) por atrapar.",
    "punto(s) bonificaci\xf3n",
    "ARCHIVO DE GUARDADO",
    "Ricerca del file di salvataggio... Attendi.",
    "$M_CARD$ non formattata.\nVuoi formattarla?",
    "RALPH IL LUPO ALL' ATTACCO\nVuoi creare un file di salvataggio?",
    "Nuova partita?\nSalvataggio automatico attivato.",
    "Vuoi caricare?\nSalvataggio automatico attivato.",
    "Salva partita?",
    "Sei sicuro di voler salvare questa partita?",
    "Sei sicuro di voler sovrascrivere questa partita?",
    "Formattazione... Attendi.",
    "Creazione del file di salvataggio... Attendi.",
    "Caricamento dati... Attendi.",
    "Salvataggio... Attendi.",
    "Sovrascrittura dati... Attendi.",
    "Salvataggio automatico... Attendi.",
    "Formattazione completa.",
    "terminata.",
    "Salvataggio completato.",
    "Sovrascrittura terminata.",
    "Formattazione fallita!",
    "Creazione del file di salvataggio fallita! Riprova.",
    "Caricamento fallito! Riprova.",
    "Salvataggio fallito! Riprova.",
    "Sovrascrittura fallita! Riprova.",
    "Nessuna $M_CARD$ inserita!\nInserisci una $M_CARD$ con almeno un blocco libero.",
    "La $M_CARD$ \xe9 piena!\nInserisci una nuova $M_CARD$ oppure cancella un blocco usando il Memory Manager interno della console.",
    "SALVATAGGIO NON TROVATO.",
    "Nuova partita",
    "La $M_CARD$ potrebbe essere danneggiata!\nRiprova o inserisci una nuova $M_CARD$.",
    "I dati potrebbero essere danneggiati!\nRiprova",
    "Salvataggio annullato!",
    "Salvataggio automatico terminato!",
    "Salvataggio automatico fallito!\nSalvataggio automatico disattivato!",
    "$M_CARD$ non formattata!\nSalvataggio automatico disattivato.",
    "Nessuna $M_CARD$ inserita!\nSalvataggio automatico disattivato.",
    "Errore del file di salvataggio!\nSalvataggio automatico disattivato.",
    "I dati potrebbero essere danneggiati!\nSalvataggio automatico disattivato!",
    "annulla",
    "riprova",
    "s\xec",
    "no",
    "ok",
    "vuoto",
    "inizia partita",
    "nuova partita",
    "indietro",
    "carica",
    "Salva",
    "partita",
    "%d pecora/e ancora da catturare",
    "bonus",
    "salvataggio",
    "PROCURANDO PARTIDA SALVA...\nESPERE...",
    "unused - Format Card ? -",
    "VOC\xca QUER SALVAR UMA PARTIDA NOVA?",
    "Nova partida:\nGRAVA\xc7\xc3O AUTOM\xc1TICA LIGADA.",
    "Carregar partita:\nGRAVA\xc7\xc3O AUTOM\xc1TICA LIGADA..",
    "Salvar partida:",
    "Voc\xea tem certeza que quer salvar esta partida?",
    "VOC\xca TEM CERTEZA QUE QUER SUBSTITUIR ESTA PARTIDA?\nGRAVA\xc7\xc3O AUTOM\xc1TICA LIGADA.",
    "unused - Formatting -",
    "CRIANDO NOVA GRAVA\xc7\xc3O...\nESPERE, POR FAVOR.",
    "CARREGANDO DADOS...\nESPERE, POR FAVOR.",
    "Salvando dados...\nESPERE, POR FAVOR.",
    "Substituindo dados...\nESPERE, POR FAVOR.",
    "Salvando dados automaticamente...\nESPERE, POR FAVOR.",
    "unused - Format ok -",
    "Carregamento completa\nGRAVA\xc7\xc3O AUTOM\xc1TICA LIGADA.",
    "Grava\xe7\xe3o completa.",
    "Substitui\xe7\xe3o completa.",
    "unused - Format Failed -",
    "NOVA GRAVA\xc7\xc3O FALHOU!\nTENTE DE NOVO. ",
    "CARREGAMENTO FALHOU!\nTENTE DE NOVO.",
    "GRAVA\xc7\xc3O FALHOU!\nTENTE DE NOVO.",
    "SUBSTITUI\xc7\xc3O FALHOU!\nTENTE DE NOVO.",
    "unused - No memory card -",
    "unused - Full card -",
    "N\xc3O SE ENCONTROU ARQUIVO SALVO.",
    "iniciar uma nova partida?\nGrava\xe7\xe3o autom\xe1tica desativada.",
    "unused - Damaged -",
    "DADOS DANIFICADOS.\nTENTE DE NOVO.",
    "Grava\xe7\xe3o de dados cancelada.",
    "Grava\xe7\xe3o autom\xe1tica completada.",
    "Grava\xe7\xe3o autom\xe1tica falhou!\nGrava\xe7\xe3o autom\xe1tica desativada.",
    "unused - AutoSav unformat -",
    "unused - AutoSav nocard -",
    "unused - AutoSav damaged -",
    "Talvez os dados contenham erros!\nGrava\xe7\xe3o autom\xe1tica desativada.",
    "Cancelar",
    "Tente novamente.",
    "sim",
    "N\xe3o ",
    "ok",
    "Vazio",
    "Iniciar jogo",
    "Novo jogo",
    "Voltar",
    "Carregar",
    "Salvar",
    "Partida",
    "Sobram %d ovelhas para pegar",
    "B\xf4nus",
    "SALVAR"};

/* ==== declarations ==== */
#include "../sdk/crt.h"
extern s32 g_dtRawMs;
#include "../engine/time.h"
#include "../engine/progress.h"
#include "../engine/game_state.h"
#include "../engine/draw2d.h"
#include "../engine/screen.h"
#include "../engine/card.h"
#include "../engine/prompt.h"
#include "../engine/maths.h"
#include "../engine/crc32.h"
#include "../engine/interface.h"
#include "../engine/text.h"
#include "../engine/input.h"
#include "../engine/load_warmeshes.h"
extern u32 *g_screenLayerBase;

extern TextPort g_textPort;
#define g_textWinX (g_textPort.winX)

extern TextPort g_textPort;
#define g_textWinY (g_textPort.winY)

extern s16 g_rectChoiceLeft[4], g_rectChoiceRight[4];
/* The original caller passes this ignored PS1 filename. */
u32 *Res_GetValidatedIdList(u16, u16 *);
u16 Text_CountWrappedLines(const char *);
void Draw2D_TexRect(float, float, float, float, float, s32, float, float, u32, float, float, u32, float, float, u32,
                    float, float, u32);
s32 MCard_DelayElapsed();
void Save_InitCardHeader();
void MCard_SetMode(u8);
const char *MCard_GetString(u32 id);
s32 Card_Access(s32);
s32 Card_CommitBlock(s32, s32);
void MCard_DrawValidFooter();
void MCard_DrawValidCancelFooter();
void MCard_DrawOkRetryFooter();
u8 MCard_ShowMessage(char *);
u8 MCard_FindSlotWithState(s32, char);
u8 MCard_StepCursor(s32, char);
u8 MCard_StepBlockScan();
u8 MCard_SlotChooserConfirmSave();
s32 Card_AnyBlockUsed();
void Card_DrawScreen();
u8 MCard_UpdateScreen();
s8 MCard_PromptYesNo(char *, char *, char *);
u8 MCard_PromptConfirm(char *, char *);
void MCard_ResetCursor();
u8 MCard_ChooseOption(char *, char *, char *);
u8 MCard_SlotChooser(char *, char *, char *);
void MCard_DrawTextColored(s16 *, u8, u32, char *, u32);
void MCard_Stub_54d4f5(s32);

/* Reconstructed inline accessors: the original retains their argument/this and byte-result temporaries under the
 * game's /Od /Ob1 recipe. */
inline void Progress::SetCardOpen(s32 on)
{
    runtimeBits.cardOpen = on;
}
inline void Progress::SetSaveSlotValid(s32 on)
{
    runtimeBits.saveSlotValid = on;
}
inline void Progress::SetAutoSaveOn(s32 on)
{
    runtimeBits.autoSaveOn = on;
}
inline u32 MCardImageTimestamp()
{
    u32 stamp = g_saveImageTimestamp;
    return stamp;
}

/* Card_DrawScreen's icon lookup */
/* cast kept: Res_FindBitmapGroup returns a group's bitmap records untyped */
#define ICON(i) ((DavBitmapRec *)g_mcardIconGroups[g_mcardIconGroup[i]][g_mcardIconFrame[i]])
/* MCard_UpdateScreen's shorthands */
/* cast kept: the UI strings are const, the card-screen functions take char * */
#define UI_STRING(id) ((char *)g_pGetUiString(id))
#define PLAIN(id) Ui_DrawTextInRect(g_mcardRectBody, TEXTALIGN_CENTER, 0, UI_STRING(id))
#define MESSAGE(id, footer)                    \
    result = MCard_ShowMessage(UI_STRING(id)); \
    footer()

/* 0x54a6da. Full EAX result, not merely a bool in AL. */
s32 MCard_DelayElapsed()
{
    g_mcardDelayMs -= g_dtRawMs;
    return g_mcardDelayMs < 0;
}

/* 0x54a6fb */
void Save_InitCardHeader()
{
    Str_CopyAsciiN((char *)g_saveCardHeader, "SC", 2); /* cast kept: the card image is bytes, its magic ASCII */
    g_saveCardHeader[2] = 0x11;
    g_saveCardHeader[3] = 1;
    g_saveImageTimestamp = 0;
    g_saveImageCrc = 0;
    memcpy(g_saveCardHeader + 4, saveTitle, 0x22);
    memset(g_saveCardHeader + 0x44, 0, 0x1c);
    memcpy(g_saveCardHeader + 0x60, saveIconData, 0x20);
    memcpy(g_saveCardHeader + 0x80, saveIconData + 0x20, 0x80);
    for (u16 index = 0; index < 4; ++index)
        g_cardBlockState[index] = 'E';
    g_saveLastSlot = 0;
    g_saveImageStatus = CARD_IMAGE_STATUS;
}

/* 0x54a7c3 - MCard_Init. The stack work record reproduces the observed /Od slots; it is not an overlay of any game
 * object. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void MCard_Init()
{
    struct Work {
        u32 *list;
        u16 *id;
        u16 groupCount, count;
        DavBitmapRec *bitmap;
        u16 ids[4];
        u16 unused;
        u16 index;
    } w;
    w.ids[0] = DAV_IDI_IMEMCICA;
    w.ids[1] = DAV_IDI_IMEMCICB;
    g_mcardImageCached = 0;
    if (!Reg_OpenProgressKey())
        printf("!!! Error initialising MCard !!!\n");
    else {
        g_mcardPhase = 5;
        g_mcardScreen = MCS_NONE;
        for (w.index = 0; w.index < 2; ++w.index) {
            w.list = Res_GetValidatedIdList(w.ids[w.index], &w.count);
            if (w.list && w.count) {
                w.id = (u16 *)*w.list; /* cast kept: an id list's entries are record addresses */
                /* cast kept: the bitmap table pointer of the packed DAV directory, at +0xa, as the loader reads it
                 * (DavDirectory is only forward-declared by the class header) */
                w.bitmap = *(DavBitmapRec **)((u8 *)g_pDav->header->dir + 0xa) + *w.id;
                g_mcardIconW1 = w.bitmap->width - 1;
                g_mcardIconH1 = w.bitmap->height - 1;
                g_mcardIconReserved[w.index] = 0;
                g_mcardIconGroups[w.index] = Res_FindBitmapGroup(w.bitmap, &w.groupCount);
            }
        }
        g_mcardFileName = "SdwDatas.BKS";
        MCard_SetMode(MCARD_MODE_NONE);
        g_mcardFreeBlocks = 0;
        g_mcardActiveMode = g_mcardMode;
        g_mcardState = MCARD_CUR_NONE;
        g_mcardFade = 15;
        g_pProgress->SetCardOpen(1);
    }
}

/* 0x54a92b. */
void MCard_Shutdown()
{
    Reg_CloseProgressKey();
    g_pProgress->SetCardOpen(0);
}

/* 0x54a968. Retain the separate first and shared second/third case bodies. */
void MCard_SetMode(u8 mode)
{
    g_mcardMode = mode;
    switch (mode) {
        case MCARD_MODE_NONE:
            g_pGetUiString = 0;
            break;
        case MCARD_MODE_LOAD:
            g_pGetUiString = MCard_GetString;
            break;
        case MCARD_MODE_SAVE:
        case MCARD_MODE_AUTOSAVE:
            g_pGetUiString = MCard_GetString;
    }
}

/* 0x54a9c1. Slot4 requests image validation without loading a record. */
s32 Card_Access(s32 slot)
{
    s32 result;
    u32 storedCrc;
    if (!g_mcardImageCached)
        Save_InitCardHeader();
    result = CardStub_Status();
    if (result == CARD_PRESENT) {
        result = CARD_READ_OK;
        if (!g_mcardImageCached) {
            result = Card_ReadBlocks(g_mcardFileName, 0x200, g_saveCardHeader);
            if (result == CARD_READ_OK) {
                storedCrc = g_saveImageCrc;
                g_saveImageCrc = 0;
                g_saveImageCrc = Crc32(g_saveCardHeader, 0x200);
                if (g_saveImageCrc != storedCrc)
                    result = CARD_CORRUPT;
            }
        }
        if (result == CARD_READ_OK && slot < 4) {
            if (g_cardBlockState[slot] == 'F') {
                g_pProgress->LoadRecord(g_cardBlocks[slot]);
                g_saveLastSlot = (s8)slot;
            } else
                result = CARD_SLOT_EMPTY;
        }
    }
    return result;
}

/* 0x54aaaa */
s32 Card_CommitBlock(s32 slot, s32 overwrite)
{
    s32 result = Card_Access(4);
    if (!overwrite)
        g_mcardImageCached = 2;
    else
        g_mcardImageCached = 3;
    if (result == CARD_CORRUPT)
        return CARD_CORRUPT;
    g_pProgress->CopyRecord(g_cardBlocks[slot]);
    if (!overwrite && g_cardBlockState[slot] == 'F')
        result = CARD_SLOT_FULL;
    else {
        if (result == CARD_READ_OK) {
            result = CardStub_Status3(g_mcardFileName);
            if (result != CARD_PREWRITE_OK)
                return result;
        }
        if (slot < 4)
            g_cardBlockState[slot] = 'F';
        g_saveImageTimestamp = g_rawTime;
        g_pProgress->SetSaveTimestamp(g_saveImageTimestamp);
        g_saveImageStatus = CARD_IMAGE_STATUS;
        g_saveLastSlot = (s8)slot;
        g_saveImageCrc = 0;
        g_saveImageCrc = Crc32(g_saveCardHeader, 0x200);
        result = Card_WriteFrames(g_mcardFileName, 0x200, g_saveCardHeader);
        if (result != CARD_WRITE_OK)
            return result;
    }
    return result;
}

/* 0x54abe6. The callback's existing text engine type takes u32; only the
 * low byte is used here, matching the original MOVZX from the argument slot. */
const char *MCard_GetString(u32 id)
{
    return g_mcardStrings[g_pProgress->GetLanguage()][(u8)id];
}

/* 0x54ac11. Two observed word-pair copies preserve the local rectangle. */
/* BYTES(view): the rectangle is copied as two dwords through a union, as the original does */
u8 MCard_ShowMessage(char *text)
{
    union {
        s16 v[4];
        u32 words[2];
    } rect;
    rect.words[0] = ((u32 *)g_mcardRectBody)[0]; /* cast kept: the s16[4] rectangle is copied as two words */
    rect.words[1] = ((u32 *)g_mcardRectBody)[1]; /* cast kept: as above */
    rect.v[3] += 48;
    rect.v[1] -= 8;
    Text_SetWindowRect(g_screenLayerBase + 2, rect.v, 1);
    Text_CenterVertically(Text_CountWrappedLines(text));
    Text_PrintfStyled(TEXTALIGN_CENTER, 0, text);
    Hud_EndBox_stub();
    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
        Ui_PlayConfirmSound();
        return MCARD_CUR_LEFT;
    }
    if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
        if (text != g_pGetUiString(MCSTR_OVERWRITE_OK) && text != g_pGetUiString(MCSTR_SAVE_OK) &&
            text != g_pGetUiString(MCSTR_SAVE_CANCELLED))
            Ui_PlayCancelSound();
        return MCARD_IN_CANCEL;
    }
    return MCARD_CUR_NONE;
}

/* 0x54acf6 */
void MCard_DrawValidFooter()
{
    char key[64], format[128];
    Text_SetWindowRect(g_screenLayerBase + 2, g_mcardRectFooter, 1);
    g_inputMgr.Input_GetKeyName(0x1c, key, 64);
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, key);
    strcat(format, "$C_DEFAULT$ %s");
    Text_Printf(TEXTALIGN_CENTER, format, g_pGetUiString(MCSTR_OK));
    Hud_EndBox_stub();
}

/* 0x54ad8a */
void MCard_DrawValidCancelFooter()
{
    char key[64], format[128];
    Text_SetWindowRect(g_screenLayerBase + 2, g_mcardRectFooter, 1);
    g_inputMgr.Input_GetKeyName(0x1c, key, 64);
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, key);
    strcat(format, "$C_DEFAULT$ %s\n");
    Text_Printf(TEXTALIGN_CENTER, format, g_pGetUiString(MCSTR_OK));
    g_inputMgr.Input_GetKeyName(1, key, 64);
    strcpy(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, key);
    strcat(format, "$C_DEFAULT$ %s");
    Text_Printf(TEXTALIGN_CENTER, format, g_pGetUiString(MCSTR_BACK));
    Hud_EndBox_stub();
}

/* 0x54ae88 */
void MCard_DrawOkRetryFooter()
{
    char key[64], format[128];
    Text_SetWindowRect(g_screenLayerBase + 2, g_mcardRectFooter, 1);
    g_inputMgr.Input_GetKeyName(0x1c, key, 64);
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, key);
    strcat(format, "$C_DEFAULT$ %s");
    Text_Printf(TEXTALIGN_LEFT, format, g_pGetUiString(MCSTR_OK));
    g_inputMgr.Input_GetKeyName(1, key, 64);
    strcpy(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, key);
    strcat(format, "$C_DEFAULT$ %s");
    Text_Printf(TEXTALIGN_RIGHT, format, g_pGetUiString(MCSTR_RETRY));
    Hud_EndBox_stub();
}

/* 0x54af86-0x54b176: card-slot navigation. Slot ids are one-based; the address arithmetic keeps that convention
 * against zero-based arrays. */
/* 0x54af86 */
u8 MCard_FindSlotWithState(s32 direction, char state)
{
    u8 current = g_mcardState;
    u8 count = 0;
    do {
        current = g_mcardSlotNav[current - 1][direction];
        ++count;
    } while (g_cardBlockState[current - 1] != state && count < 6);
    if (count == 6)
        current = 1;
    return current;
}

/* 0x54afe0 */
u8 MCard_StepCursor(s32 direction, char skip)
{
    u8 cursor = g_mcardState;
    if (cursor & MCARD_CUR_LEFT)
        return MCARD_CUR_RIGHT;
    if (cursor & MCARD_CUR_RIGHT) {
        if (g_mcardMode != MCARD_MODE_LOAD || Card_AnyBlockUsed())
            return MCARD_CUR_LEFT;
        else
            return MCARD_CUR_RIGHT;
    }
    do {
        cursor = g_mcardSlotNav[cursor - 1][direction];
    } while (g_cardBlockState[cursor - 1] == skip);
    return cursor;
}

/* 0x54b04e: the original stores an unused zero byte before scanning. */
/* BYTES(dead-code): unused is zeroed and never read: the original stores this zero byte */
s32 Card_AnyBlockUsed()
{
    u8 unused = 0;
    u8 index;
    for (index = 0; index < 4; ++index)
        if (g_cardBlockState[index] == 'F')
            return 1;
    return 0;
}

/* 0x54b08c */
u8 MCard_StepBlockScan()
{
    if (g_mcardState == MCARD_CUR_LEFT) {
        if (Card_AnyBlockUsed()) {
            g_mcardNavSkipState = 'E';
            g_mcardChooserVariant = MCARD_CUR_LEFT;
            g_mcardState = MCARD_CUR_SLOT4;
            g_mcardState = MCard_StepCursor(1, g_mcardNavSkipState);
        }
        return MCARD_CUR_NONE;
    } else if (g_mcardState == MCARD_CUR_RIGHT) {
        g_mcardNavSkipState = 0;
        g_mcardChooserVariant = MCARD_CUR_RIGHT;
        g_mcardState = MCARD_CUR_SLOT4;
        g_mcardState = MCard_FindSlotWithState(1, 'E');
        return MCARD_CUR_NONE;
    }
    return g_mcardChooserVariant | g_mcardState;
}

/* 0x54b11b */
u8 MCard_SlotChooserConfirmSave()
{
    if (g_mcardState == MCARD_CUR_LEFT) {
        g_mcardNavSkipState = 0;
        g_mcardChooserVariant = MCARD_CUR_LEFT;
        g_mcardState = MCARD_CUR_SLOT4;
        g_mcardState = MCard_FindSlotWithState(1, 'F');
        return MCARD_CUR_NONE;
    } else if (g_mcardState == MCARD_CUR_RIGHT)
        return MCARD_CUR_RIGHT;
    return g_mcardChooserVariant | g_mcardState;
}

/* 0x54b176 - Card_DrawScreen: the four slot thumbnails */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Card_DrawScreen()
{
    struct Work {
        float right, top, left, bottom;
        u16 unused, index, icon;
        u8 unusedByte, selected;
        s16 screen[4], texture[4];
    } w;
    w.selected = (g_mcardState - 1) & MCARD_CUR_SLOT_MASK;
    Text_SetColor(0x4bccff);
    Text_SetWindow(g_screenLayerBase + 2, (MCardScreenWidth() - 304) / 2, 88, 304, 32, 1);
    for (w.index = 0; w.index < 4; ++w.index) {
        w.icon = 19;
        if (g_cardBlockState[w.index] == 'F')
            w.icon = (s8)g_cardBlocks[w.index][0x10];
        w.texture[0] = ICON(w.icon)->u;
        w.texture[1] = ICON(w.icon)->v;
        w.texture[2] = ICON(w.icon)->width - 1;
        w.texture[3] = ICON(w.icon)->height - 1;
        w.left = Tex_CornerUV(0, w.texture[2], w.texture[0]);
        w.right = Tex_CornerUV(w.texture[2], w.texture[2], w.texture[0]);
        w.top = Tex_CornerUV(0, w.texture[3], w.texture[1]);
        w.bottom = Tex_CornerUV(w.texture[3], w.texture[3], w.texture[1]);
        w.screen[0] = w.index * 80;
        w.screen[1] = 0;
        w.screen[2] = 63;
        w.screen[3] = 31;
        w.screen[0] += g_textWinX;
        w.screen[1] += g_textWinY;
        Ui_DrawRectOutline(w.screen, w.selected == w.index ? 0x4040f0 : 0xb0b0b0);
        Draw2D_TexRect(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 2), g_screen.ScaleX(w.screen[0]),
                       g_screen.ScaleY(w.screen[1]), g_screen.ScaleX(w.screen[0] + w.screen[2]),
                       g_screen.ScaleY(w.screen[1] + w.screen[3]), ICON(w.icon)->page, w.left, w.top, 0xffffff, w.left,
                       w.bottom, 0xffffff, w.right, w.top, 0xffffff, w.right, w.bottom, 0xffffff);
    }
    Hud_EndBox_stub();
}

/* 0x54b459 - Card_StateMachine, reconstructed against the complete code and both original switch tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Card_StateMachine()
{
    struct Work {
        s32 status, result;
        u8 unused[3], event;
    } w;
    w.result = MCARD_RUNNING;
    if (g_mcardMode != g_mcardActiveMode) {
        g_mcardChooserVariant = MCARD_CUR_NONE;
        g_mcardActiveMode = g_mcardMode;
        g_mcardResult = MCARD_RUNNING;
        g_mcardScreen = MCS_FADE_IN;
        g_mcardDelayMs = 2000;
    }
    w.event = MCard_UpdateScreen();
    switch (g_mcardScreen) {
        case MCS_FADE_IN:
            if (g_mcardFade > 0)
                --g_mcardFade;
            else
                g_mcardScreen = MCS_DETECT;
            break;
        case MCS_FADE_OUT:
            if (g_mcardFade < 15)
                ++g_mcardFade;
            else
                w.result = g_mcardResult;
            break;
        case MCS_EXIT:
            g_mcardImageCached = 0;
            g_mcardResult = MCARD_CANCELLED;
            g_mcardScreen = MCS_FADE_OUT;
    }
    if (g_mcardActiveMode != MCARD_MODE_AUTOSAVE) {
        switch (g_mcardScreen) {
            case MCS_EXIT:
            case MCS_FADE_IN:
            case MCS_FADE_OUT:
                break;
            case MCS_DETECT:
                g_mcardImageCached = 0;
                if (MCard_DelayElapsed()) {
                    w.status = CardStub_Status();
                    g_mcardFreeBlocks = CardStub_FreeBlocks();
                    g_mcardChooserVariant = MCARD_CUR_NONE;
                    g_mcardState = MCARD_CUR_LEFT;
                    g_mcardDelayMs = 1000;
                    switch (w.status) {
                        case CARD_UNFORMATTED:
                            g_mcardState = MCARD_CUR_RIGHT;
                            g_mcardScreen = MCS_FORMAT_Q;
                            break;
                        case CARD_PRESENT:
                            g_mcardScreen = MCS_OPEN;
                            break;
                        case CARD_DAMAGED:
                            g_mcardScreen = MCS_DAMAGED;
                            break;
                        default:
                            g_mcardScreen = MCS_NO_CARD;
                    }
                }
                break;
            case MCS_FORMAT_Q:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_FORMATTING;
                }
                if ((w.event & MCARD_CUR_RIGHT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_EXIT;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_EXIT;
                }
                break;
            case MCS_FORMATTING:
                MCard_Stub_54d4f5(0);
                if (g_mcardScreen == MCS_NO_CARD) {
                    g_mcardScreen = MCS_FORMAT_FAILED;
                }
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = CardStub_Status2();
                    if (w.status == CARD_FORMAT_OK) {
                        g_mcardDelayMs = 8000;
                        g_mcardPhase = 0;
                        g_mcardScreen = MCS_FORMAT_OK;
                    } else {
                        g_mcardDelayMs = 3000;
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_FORMAT_FAILED;
                    }
                }
                break;
            case MCS_FORMAT_OK:
                MCard_Stub_54d4f5(0);
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    g_mcardScreen = MCS_CREATING;
                }
                break;
            case MCS_OPEN:
                g_mcardDelayMs = 1000;
                w.status = Card_SaveExists(g_mcardFileName);
                g_mcardState = MCARD_CUR_LEFT;
                if (w.status != CARD_STATUS_NONE) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardDelayMs = 1500;
                        g_mcardScreen = MCS_READ_LOAD;
                    } else
                        g_mcardScreen = MCS_READ_SAVE;
                } else {
                    g_mcardState = MCARD_CUR_LEFT;
                    if (g_mcardFreeBlocks == 0)
                        g_mcardScreen = MCS_CARD_FULL;
                    else if (CardStub_Status() == CARD_NONE)
                        g_mcardScreen = MCS_NO_CARD;
                    else
                        g_mcardScreen = MCS_FILE_NOT_FOUND;
                }
                break;
            case MCS_FILE_NOT_FOUND:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_CHECK_SPACE;
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_EXIT;
                }
                break;
            case MCS_READ_LOAD:
                MCard_Stub_54d4f5(0);
                if (g_mcardScreen == MCS_NO_CARD)
                    g_mcardScreen = MCS_LOAD_FAILED;
                if (MCard_DelayElapsed()) {
                    w.status = Card_Access(4);
                    switch (w.status) {
                        case CARD_READ_OK:
                            g_mcardDelayMs = 1000;
                            MCard_ResetCursor();
                            g_mcardPhase = g_saveLastSlot;
                            g_mcardScreen = MCS_CHOOSE_LOAD;
                            break;
                        case CARD_CORRUPT:
                            if (CardStub_Status() == CARD_NONE)
                                g_mcardScreen = MCS_LOAD_FAILED;
                            else
                                g_mcardScreen = MCS_CORRUPT;
                            break;
                        default:
                            g_mcardScreen = MCS_LOAD_FAILED;
                    }
                }
                break;
            case MCS_READ_SAVE:
                MCard_Stub_54d4f5(0);
                if (MCard_DelayElapsed()) {
                    w.status = Card_Access(4);
                    switch (w.status) {
                        case CARD_READ_OK:
                            g_mcardDelayMs = 1000;
                            g_mcardPhase = g_saveLastSlot;
                            g_mcardState = MCARD_CUR_RIGHT;
                            g_mcardScreen = MCS_CHOOSE_SAVE;
                            break;
                        case CARD_CORRUPT:
                            if (CardStub_Status() == CARD_NONE)
                                g_mcardScreen = MCS_NO_CARD;
                            else
                                g_mcardScreen = MCS_CORRUPT;
                            break;
                        default:
                            g_mcardScreen = MCS_NO_CARD;
                    }
                }
                break;
            case MCS_CHECK_SPACE:
                if (g_mcardFreeBlocks == 0) {
                    g_mcardState = MCARD_CUR_LEFT;
                    g_mcardScreen = MCS_CARD_FULL;
                } else {
                    g_mcardState = MCARD_CUR_RIGHT;
                    g_mcardScreen = MCS_CREATE_Q;
                }
                break;
            case MCS_CREATE_Q:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_CREATING;
                }
                if ((w.event & MCARD_CUR_RIGHT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_DETECT;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_CREATING:
                MCard_Stub_54d4f5(0);
                if (g_mcardScreen == MCS_NO_CARD)
                    g_mcardScreen = MCS_CREATE_FAILED;
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = Card_CommitBlock(4, 0);
                    g_mcardState = MCARD_CUR_LEFT;
                    switch (w.status) {
                        case CARD_WRITE_OK:
                            if (g_mcardMode == MCARD_MODE_LOAD) {
                                MCard_ResetCursor();
                                g_saveLastSlot = 0;
                                g_mcardPhase = g_saveLastSlot;
                                g_mcardScreen = MCS_CHOOSE_LOAD;
                            } else {
                                g_mcardState = MCARD_CUR_LEFT;
                                g_mcardScreen = MCS_CHOOSE_SAVE;
                            }
                            break;
                        default:
                            g_mcardScreen = MCS_CREATE_FAILED;
                    }
                }
                break;
            case MCS_CHOOSE_LOAD:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardPhase = w.event - 1 & MCARD_CUR_SLOT_MASK;
                    g_mcardState = w.event & MCARD_CUR_SLOT_MASK | MCARD_CUR_RIGHT;
                    g_mcardScreen = MCS_LOADING;
                }
                if ((w.event & MCARD_CUR_RIGHT) != 0) {
                    g_mcardPhase = w.event - 1 & MCARD_CUR_SLOT_MASK;
                    g_saveLastSlot = g_mcardPhase;
                    g_mcardState = w.event & MCARD_CUR_SLOT_MASK | MCARD_CUR_RIGHT;
                    if (g_cardBlockState[(char)g_mcardPhase] == 'F') {
                        g_mcardScreen = MCS_CONFIRM_OVERWRITE;
                    } else {
                        g_mcardScreen = MCS_FINISH_LOADED;
                    }
                }
                break;
            case MCS_CHOOSE_SAVE:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardPhase = w.event - 1 & MCARD_CUR_SLOT_MASK;
                    g_mcardState = w.event & MCARD_CUR_SLOT_MASK | MCARD_CUR_RIGHT;
                    if (g_cardBlockState[(char)g_mcardPhase] == 'F') {
                        g_mcardScreen = MCS_CONFIRM_OVERWRITE;
                    } else {
                        g_mcardScreen = MCS_CONFIRM_SAVE;
                    }
                }
                if ((w.event & MCARD_CUR_RIGHT) != 0) {
                    g_mcardState = MCARD_CUR_LEFT;
                    g_mcardScreen = MCS_SAVE_CANCELLED;
                }
                break;
            case MCS_CONFIRM_OVERWRITE:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardScreen = MCS_FINISH_LOADED;
                        g_saveLastSlot = w.event - 1 & MCARD_CUR_SLOT_MASK;
                    } else {
                        g_mcardImageCached = 3;
                        g_mcardScreen = MCS_WRITING;
                    }
                }
                if ((w.event & (MCARD_CUR_RIGHT | MCARD_IN_CANCEL)) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = g_mcardState & MCARD_CUR_SLOT_MASK;
                        g_mcardChooserVariant = MCARD_CUR_RIGHT;
                        g_mcardScreen = MCS_CHOOSE_LOAD;
                    } else {
                        g_mcardState = g_mcardState & MCARD_CUR_SLOT_MASK;
                        g_mcardChooserVariant = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_CHOOSE_SAVE;
                    }
                }
                break;
            case MCS_CONFIRM_SAVE:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_WRITING;
                }
                if ((w.event & (MCARD_CUR_RIGHT | MCARD_IN_CANCEL)) != 0) {
                    g_mcardState = g_mcardState & MCARD_CUR_SLOT_MASK;
                    g_mcardChooserVariant = MCARD_CUR_LEFT;
                    g_mcardScreen = MCS_CHOOSE_SAVE;
                }
                break;
            case MCS_LOADING:
                MCard_Stub_54d4f5(0);
                w.status = Card_Access((s8)g_mcardPhase);
                g_mcardState = MCARD_CUR_LEFT;
                switch (w.status) {
                    case CARD_READ_OK:
                        g_mcardScreen = MCS_FINISH_LOADED;
                        break;
                    default:
                        g_mcardScreen = MCS_LOAD_FAILED;
                }
                break;
            case MCS_WRITING:
                MCard_Stub_54d4f5(0);
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = Card_CommitBlock((s8)g_mcardPhase, g_cardBlockState[(s8)g_mcardPhase] == 'F');
                    g_mcardState = MCARD_CUR_LEFT;
                    switch (w.status) {
                        case CARD_WRITE_OK:
                            if (g_mcardImageCached == 3)
                                g_mcardScreen = MCS_OVERWRITE_OK;
                            else
                                g_mcardScreen = MCS_SAVE_OK;
                            break;
                        default:
                            if (g_mcardImageCached == 3)
                                g_mcardScreen = MCS_OVERWRITE_FAILED;
                            else
                                g_mcardScreen = MCS_SAVE_FAILED;
                            g_mcardImageCached = 0;
                    }
                }
                break;
            case MCS_LOAD_OK:
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_FINISH_LOADED;
                }
                break;
            case MCS_SAVE_OK:
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_FINISH_SAVED;
                }
                break;
            case MCS_OVERWRITE_OK:
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardScreen = MCS_FINISH_LOADED;
                    } else {
                        g_mcardScreen = MCS_FINISH_SAVED;
                    }
                }
                break;
            case MCS_NEW_GAME_NO_AUTOSAVE_Q:
                MCard_Stub_54d4f5(4);
                g_mcardDelayMs = 1000;
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_FINISH_NO_AUTOSAVE;
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardDelayMs = 2000;
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_FINISH_SAVED:
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlot(g_saveLastSlot);
                g_pProgress->SetSaveSlotValid(1);
                g_mcardResult = MCARD_DONE;
                g_mcardScreen = MCS_FADE_OUT;
                break;
            case MCS_FINISH_PLAIN:
                g_mcardImageCached = 0;
                g_mcardResult = MCARD_DONE;
                g_mcardScreen = MCS_FADE_OUT;
                break;
            case MCS_FINISH_LOADED:
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlot(g_saveLastSlot);
                g_pProgress->SetSaveSlotValid(1);
                g_pProgress->SetAutoSaveOn(1);
                g_pProgress->SetSaveTimestamp(g_saveImageTimestamp);
                g_mcardResult = MCARD_DONE;
                g_mcardScreen = MCS_FADE_OUT;
                break;
            case MCS_FINISH_NO_AUTOSAVE:
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlotValid(0);
                g_pProgress->SetAutoSaveOn(0);
                g_mcardResult = MCARD_DONE;
                g_mcardScreen = MCS_FADE_OUT;
                break;
            case MCS_DAMAGED:
            case MCS_CORRUPT:
                MCard_Stub_54d4f5(0);
                g_mcardDelayMs = 1000;
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlotValid(0);
                g_pProgress->SetAutoSaveOn(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_EXIT;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_NO_CARD:
            case MCS_CARD_FULL:
                MCard_Stub_54d4f5(4);
                g_mcardDelayMs = 1000;
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlotValid(0);
                g_pProgress->SetAutoSaveOn(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_FINISH_PLAIN;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_EXIT;
                }
                break;
            case MCS_FORMAT_FAILED:
            case MCS_CREATE_FAILED:
                MCard_Stub_54d4f5(4);
                g_mcardDelayMs = 1000;
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlotValid(0);
                g_pProgress->SetAutoSaveOn(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_EXIT;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_LOAD_FAILED:
            case MCS_SAVE_FAILED:
            case MCS_OVERWRITE_FAILED:
                MCard_Stub_54d4f5(4);
                g_mcardDelayMs = 1000;
                g_mcardImageCached = 0;
                g_pProgress->SetSaveSlotValid(0);
                g_pProgress->SetAutoSaveOn(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    if (g_mcardMode == MCARD_MODE_LOAD) {
                        g_mcardState = MCARD_CUR_LEFT;
                        g_mcardScreen = MCS_NEW_GAME_NO_AUTOSAVE_Q;
                    } else {
                        g_mcardScreen = MCS_FINISH_PLAIN;
                    }
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_SAVE_CANCELLED:
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardScreen = MCS_FINISH_PLAIN;
                }
                break;
            default:
                g_mcardImageCached = 0;
                g_mcardScreen = MCS_DETECT;
        }
    } else {
        switch (g_mcardScreen) {
            case MCS_EXIT:
            case MCS_FADE_IN:
            case MCS_FADE_OUT:
                break;
            case MCS_DETECT:
                g_mcardImageCached = 0;
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = CardStub_Status();
                    g_mcardFreeBlocks = CardStub_FreeBlocks();
                    g_mcardState = MCARD_CUR_LEFT;
                    switch (w.status) {
                        case CARD_PRESENT:
                            g_mcardScreen = MCS_OPEN;
                            break;
                        case CARD_UNFORMATTED:
                        case CARD_DAMAGED:
                            g_mcardDelayMs = 3000;
                            g_mcardScreen = MCS_AUTOSAVE_FAILED;
                            break;
                        default:
                            g_mcardDelayMs = 3000;
                            g_mcardScreen = MCS_AUTOSAVE_NOCARD;
                    }
                }
                break;
            case MCS_OPEN:
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = Card_SaveExists(g_mcardFileName);
                    g_mcardState = MCARD_CUR_LEFT;
                    if (w.status != CARD_STATUS_NONE) {
                        w.status = Card_Access(4);
                        if (w.status == CARD_READ_OK) {
                            if (MCardImageTimestamp() == g_pProgress->GetSaveTimestamp()) {
                                g_mcardDelayMs = 1500;
                                g_mcardScreen = MCS_WRITING;
                            } else {
                                g_mcardDelayMs = 3000;
                                g_mcardScreen = MCS_AUTOSAVE_FAILED;
                            }
                        } else {
                            g_mcardDelayMs = 3000;
                            g_mcardScreen = MCS_AUTOSAVE_CORRUPT;
                        }
                    } else {
                        g_mcardDelayMs = 3000;
                        g_mcardScreen = MCS_AUTOSAVE_FAILED;
                    }
                }
                break;
            case MCS_WRITING:
                MCard_Stub_54d4f5(0);
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    w.status = Card_CommitBlock((int)(char)g_pProgress->saveSlot, 1);
                    g_mcardState = MCARD_CUR_LEFT;
                    if (w.status == CARD_WRITE_OK) {
                        g_mcardDelayMs = 2000;
                        g_mcardScreen = MCS_AUTOSAVE_OK;
                    } else {
                        g_mcardDelayMs = 3000;
                        g_mcardScreen = MCS_AUTOSAVE_FAILED;
                    }
                }
                break;
            case MCS_SAVE_OK:
                MCard_Stub_54d4f5(0);
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardDelayMs = 1000;
                    g_mcardImageCached = 0;
                    g_pProgress->SetSaveSlot(g_saveLastSlot);
                    g_pProgress->SetSaveSlotValid(1);
                    g_mcardResult = MCARD_DONE;
                    g_mcardScreen = MCS_FADE_OUT;
                }
                if ((w.event & MCARD_IN_CANCEL) != 0) {
                    g_mcardScreen = MCS_DETECT;
                }
                break;
            case MCS_AUTOSAVE_OK:
                MCard_Stub_54d4f5(0);
                if (MCard_DelayElapsed()) {
                    g_mcardDelayMs = 1000;
                    g_mcardImageCached = 0;
                    g_pProgress->SetSaveSlot(g_saveLastSlot);
                    g_pProgress->SetSaveSlotValid(1);
                    g_mcardResult = MCARD_DONE;
                    g_mcardScreen = MCS_FADE_OUT;
                }
                break;
            case MCS_AUTOSAVE_FAILED:
            case MCS_AUTOSAVE_NOCARD:
            case MCS_AUTOSAVE_CORRUPT:
                if ((w.event & MCARD_CUR_LEFT) != 0) {
                    g_mcardImageCached = 0;
                    g_pProgress->SetSaveSlotValid(0);
                    g_pProgress->SetAutoSaveOn(0);
                    g_mcardResult = MCARD_FAILED;
                    g_mcardScreen = MCS_FADE_OUT;
                }
                break;
            default:
                g_mcardImageCached = 0;
                g_mcardScreen = MCS_DETECT;
                break;
        }
    }
    if (w.result != MCARD_RUNNING)
        g_mcardActiveMode = MCARD_MODE_NONE;
    return w.result;
}

/* 0x54c93b - MCard_UpdateScreen. The case order preserves the original 42-entry dispatch. */
u8 MCard_UpdateScreen()
{
    u8 result = 0;
    char text[128];
    Ui_DrawMemCardBackdrop((s32)g_mcardFade >> 1);
    Text_SetFont(FONT_GAME);
    switch (g_mcardScreen) {
        case MCS_DETECT:
        case MCS_OPEN:
        case MCS_READ_SAVE:
            PLAIN(MCSTR_LOOKING);
            break;
        case MCS_FILE_NOT_FOUND:
            MESSAGE(MCSTR_FILE_NOT_FOUND, MCard_DrawValidCancelFooter);
            break;
        case MCS_FORMAT_Q:
            result = MCard_PromptYesNo(UI_STRING(MCSTR_FORMAT_Q), UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_FORMATTING:
            PLAIN(MCSTR_FORMATTING);
            break;
        case MCS_FORMAT_OK:
            Text_Sprintf(text, UI_STRING(MCSTR_FORMAT_OK), g_mcardFreeBlocks);
            result = MCard_ShowMessage(text);
            break;
        case MCS_CREATE_Q:
            Text_Sprintf(text, UI_STRING(MCSTR_CREATE_FILE_Q), g_mcardFreeBlocks);
            result = MCard_PromptYesNo(text, UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_CREATING:
            PLAIN(MCSTR_CREATING);
            break;
        case MCS_CREATE_FAILED:
            MESSAGE(MCSTR_CREATE_FAILED, MCard_DrawOkRetryFooter);
            break;
        case MCS_CHOOSE_LOAD:
            if (g_mcardChooserVariant == MCARD_CUR_LEFT)
                result = MCard_SlotChooser(UI_STRING(MCSTR_LOAD_GAME), UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            else if (g_mcardChooserVariant == MCARD_CUR_RIGHT)
                result = MCard_SlotChooser(UI_STRING(MCSTR_CREATE_NEW_GAME), UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            else
                result =
                    MCard_SlotChooser(UI_STRING(MCSTR_LOAD_GAME), UI_STRING(MCSTR_LOAD), UI_STRING(MCSTR_NEW_GAME));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_CHOOSE_SAVE:
            result = MCard_SlotChooser(UI_STRING(MCSTR_SAVE_GAME), UI_STRING(MCSTR_SAVE), UI_STRING(MCSTR_CANCEL));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_CONFIRM_OVERWRITE:
            result = MCard_ChooseOption(UI_STRING(MCSTR_CONFIRM_OVERWRITE), UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_CONFIRM_SAVE:
            result = MCard_ChooseOption(UI_STRING(MCSTR_CONFIRM_SAVE), UI_STRING(MCSTR_YES), UI_STRING(MCSTR_NO));
            MCard_DrawValidCancelFooter();
            break;
        case MCS_READ_LOAD:
            PLAIN(MCSTR_LOADING);
            break;
        case MCS_WRITING:
            if (g_mcardImageCached == 3) {
                PLAIN(MCSTR_OVERWRITING);
            } else
                switch (g_mcardMode) {
                    case MCARD_MODE_LOAD:
                        PLAIN(MCSTR_CREATING);
                        break;
                    case MCARD_MODE_SAVE:
                        PLAIN(MCSTR_SAVING);
                        break;
                    case MCARD_MODE_AUTOSAVE:
                        PLAIN(MCSTR_AUTOSAVING);
                }
            break;
        case MCS_LOAD_OK:
            MESSAGE(MCSTR_LOAD_OK, MCard_DrawValidFooter);
            break;
        case MCS_SAVE_OK:
            MESSAGE(MCSTR_SAVE_OK, MCard_DrawValidFooter);
            break;
        case MCS_OVERWRITE_OK:
            MESSAGE(MCSTR_OVERWRITE_OK, MCard_DrawValidFooter);
            break;
        case MCS_NEW_GAME_NO_AUTOSAVE_Q:
            MESSAGE(MCSTR_NEW_GAME_NO_AUTOSAVE_Q, MCard_DrawValidCancelFooter);
            break;
        case MCS_NO_CARD:
            MESSAGE(MCSTR_NO_CARD, MCard_DrawValidCancelFooter);
            break;
        case MCS_CARD_FULL:
            MESSAGE(MCSTR_CARD_FULL, MCard_DrawValidCancelFooter);
            break;
        case MCS_DAMAGED:
            MESSAGE(MCSTR_DAMAGED, MCard_DrawOkRetryFooter);
            break;
        case MCS_FORMAT_FAILED:
            MESSAGE(MCSTR_FORMAT_FAILED, MCard_DrawOkRetryFooter);
            break;
        case MCS_CORRUPT:
            MESSAGE(MCSTR_CORRUPT, MCard_DrawOkRetryFooter);
            break;
        case MCS_OVERWRITE_FAILED:
            MESSAGE(MCSTR_OVERWRITE_FAILED, MCard_DrawOkRetryFooter);
            break;
        case MCS_LOAD_FAILED:
            MESSAGE(MCSTR_LOAD_FAILED, MCard_DrawOkRetryFooter);
            break;
        case MCS_SAVE_FAILED:
            MESSAGE(MCSTR_SAVE_FAILED, MCard_DrawOkRetryFooter);
            break;
        case MCS_AUTOSAVE_OK:
            PLAIN(MCSTR_AUTOSAVE_OK);
            break;
        case MCS_SAVE_CANCELLED:
            MESSAGE(MCSTR_SAVE_CANCELLED, MCard_DrawValidFooter);
            break;
        case MCS_AUTOSAVE_FAILED:
            result = MCard_PromptConfirm(UI_STRING(MCSTR_AUTOSAVE_FAILED), UI_STRING(MCSTR_OK));
            break;
        case MCS_AUTOSAVE_NOCARD:
            result = MCard_PromptConfirm(UI_STRING(MCSTR_AUTOSAVE_NOCARD), UI_STRING(MCSTR_OK));
            break;
        case MCS_AUTOSAVE_CORRUPT:
            result = MCard_PromptConfirm(UI_STRING(MCSTR_AUTOSAVE_CORRUPT), UI_STRING(MCSTR_OK));
    }
    return result;
}

/* 0x54d02d / 0x54d04b: retain the byte-sized return temporaries. */
u8 MCard_PromptConfirm(char *title, char *prompt)
{
    u8 result = Ui_PromptConfirm(title, prompt);
    return result;
}
s8 MCard_PromptYesNo(char *title, char *left, char *right)
{
    s8 result = Ui_PromptYesNo(title, left, right, &g_mcardState);
    return result;
}

/* 0x54d072. */
void MCard_ResetCursor()
{
    if (Card_AnyBlockUsed())
        g_mcardState = MCARD_CUR_LEFT;
    else
        g_mcardState = MCARD_CUR_RIGHT;
}

/* 0x54d090 */
u8 MCard_ChooseOption(char *title, char *left, char *right)
{
    s16 titleRect[4];
    u8 selection, slot;
    titleRect[0] = 24;
    titleRect[1] = 8;
    titleRect[2] = MCardScreenWidth() - 48;
    titleRect[3] = 70;
    slot = g_mcardState & MCARD_CUR_SLOT_MASK;
    selection = g_mcardState & (u8)~MCARD_CUR_SLOT_MASK;
    Text_SetColor(0x4bccff);
    Ui_DrawTextInRect(titleRect, TEXTALIGN_CENTER, 0, title);
    Card_DrawScreen();
    Ui_DrawTextInRect(g_rectChoiceLeft, TEXTALIGN_CENTER, selection == MCARD_CUR_LEFT, left);
    Ui_DrawTextInRect(g_rectChoiceRight, TEXTALIGN_CENTER, selection == MCARD_CUR_RIGHT, right);
    MCard_DrawValidCancelFooter();
    if (Pad_MenuRepeat((u16)~PAD_LEFT) || Pad_MenuRepeat((u16)~PAD_RIGHT)) {
        Ui_PlayMoveSound();
        if (selection == MCARD_CUR_LEFT)
            selection = MCARD_CUR_RIGHT;
        else
            selection = MCARD_CUR_LEFT;
    }
    g_mcardState = slot | selection;
    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
        Ui_PlayConfirmSound();
        return g_mcardState;
    }
    if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
        Ui_PlayCancelSound();
        return MCARD_IN_CANCEL;
    }
    return MCARD_CUR_NONE;
}

/* 0x54d1d0 - MCard_SlotChooser */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
u8 MCard_SlotChooser(char *title, char *left, char *right)
{
    u32 savedColor;
    struct Work {
        char text[128];
        s16 titleRect[4], detailsRect[4];
    } w;
    w.titleRect[0] = 24;
    w.titleRect[1] = 8;
    w.titleRect[2] = MCardScreenWidth() - 48;
    w.titleRect[3] = 60;
    w.detailsRect[0] = 24;
    w.detailsRect[1] = 136;
    w.detailsRect[2] = MCardScreenWidth() - 48;
    w.detailsRect[3] = 80;
    Card_DrawScreen();
    if (g_mcardState >= MCARD_CUR_SLOT1 && g_mcardState <= MCARD_CUR_SLOT4) {
        Text_SetColor(0x4bccff);
        Ui_DrawTextInRect(w.titleRect, TEXTALIGN_CENTER, 0, title);
    } else {
        if (g_mcardMode == MCARD_MODE_LOAD && !Card_AnyBlockUsed())
            savedColor = 0x808080;
        else
            savedColor = 0x4bccff;
        MCard_DrawTextColored(g_mcardRectChoiceLeft, TEXTALIGN_CENTER, g_mcardState == MCARD_CUR_LEFT, left,
                              savedColor);
        Text_SetColor(0x4bccff);
        Ui_DrawTextInRect(g_mcardRectChoiceRight, TEXTALIGN_CENTER, g_mcardState == MCARD_CUR_RIGHT, right);
    }
    if (Pad_MenuRepeat((u16)~PAD_LEFT)) {
        Ui_PlayMoveSound();
        g_mcardState = MCard_StepCursor(0, g_mcardNavSkipState);
    }
    if (Pad_MenuRepeat((u16)~PAD_RIGHT)) {
        Ui_PlayMoveSound();
        g_mcardState = MCard_StepCursor(1, g_mcardNavSkipState);
    }
    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
        Ui_PlayConfirmSound();
        if (g_mcardMode == MCARD_MODE_LOAD)
            return MCard_StepBlockScan();
        else
            return MCard_SlotChooserConfirmSave();
    }
    if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
        Ui_PlayCancelSound();
        if (g_mcardState == MCARD_CUR_LEFT || g_mcardState == MCARD_CUR_RIGHT) {
            g_mcardChooserVariant = MCARD_CUR_NONE;
            g_mcardScreen = MCS_EXIT;
        } else {
            g_mcardState = g_mcardChooserVariant;
            g_mcardChooserVariant = MCARD_CUR_NONE;
        }
    }
    if (g_mcardState >= MCARD_CUR_SLOT1 && g_mcardState <= MCARD_CUR_SLOT4) {
        Text_SetWindowRect(g_screenLayerBase + 2, w.detailsRect, 1);
        Text_Printf(TEXTALIGN_CENTER, "%s %d:", g_pGetUiString(MCSTR_GAME), g_mcardState);
        if (g_cardBlockState[g_mcardState - 1] == 'F') {
            /* Saved-record +0xe/+0xf are the signed sheep/bonus counts,
             * corresponding to Progress +0x92/+0x93. */
            Text_Sprintf(w.text, g_pGetUiString(MCSTR_SHEEP_TO_CATCH), (s8)g_cardBlocks[g_mcardState - 1][0xe]);
            Text_Printf(TEXTALIGN_CENTER, "\n%s\n%d %s", w.text, (s8)g_cardBlocks[g_mcardState - 1][0xf],
                        g_pGetUiString(MCSTR_BONUS));
        } else
            Text_Printf(TEXTALIGN_CENTER, "\n%s", g_pGetUiString(MCSTR_EMPTY));
        Hud_EndBox_stub();
        MCard_DrawValidCancelFooter();
    }
    return 0;
}

/* 0x54d4cc */
void MCard_DrawTextColored(s16 *rect, u8 align, u32 style, char *text, u32 color)
{
    Text_SetColor(color);
    Ui_DrawTextInRect(rect, align, style, text);
}

/* 0x54d4f5 - empty cdecl function taking one argument, called 22 times by Card_StateMachine 0x54b459 (with 0 or 4),
 * each time as the first statement of a state's case. Presumably a console-side hook with no PC body (inferred). */
void MCard_Stub_54d4f5(s32 arg) {}
