#ifndef SDW_LEVELPATCH_H
#define SDW_LEVELPATCH_H

typedef struct {
    unsigned short classId;
    unsigned short offset; /* from the start of the property block (record + 0x14) */
    const char *name;
} LevelPatchProp;

/* A model an `import` line copied from another level: its new resource index here, and where it came from. The
 * loader builds its Mesh (and adds the texture pages it uses) once the game has built the level's own meshes. */
#define LEVELPATCH_PATH 260

typedef struct {
    unsigned index;
    char source[LEVELPATCH_PATH]; /* the other file, as `resolve` names it: its path without the .WAR / .DAV */
    unsigned res;
    unsigned type; /* its type there: 0x43 static, 0x44 animated */
} LevelPatchImport;

/* The other levels' .WAR files that imports read, kept in memory (read-only) while this level is loaded: an imported
 * model's record, copied into this level's image, points into its file for its vertices, polygons, boxes, joints and
 * animations. The loader frees them when the next level loads. */
typedef struct {
    char source[LEVELPATCH_PATH];
    unsigned char *data;
    unsigned size;
} LevelPatchImage;

#define LEVELPATCH_MAX_IMPORTS 64
#define LEVELPATCH_TEXT 1024
#define LEVELPATCH_STRING 512 /* one string of `text <class> <n> "..."` */
#define LEVELPATCH_MAX_IMAGES 8

/* What a level patch changes: the level's .WAR image as Load_WAR leaves it (blob offsets = file offsets, the resource
 * count at blob + 0x0C), the resource table the game uses (war.table; room for `capacity` entries), and free room for
 * new records inside the image, at blob offsets [room_start, room_start + room_size). For `import`, `resolve` turns
 * the name a patch gives (a level, `Lvl-11`, or `mod:<file>`, a model file in the mod's own folder) into the path of
 * its files without the extension (0 if it cannot), and `load_war` reads <path>.WAR (a malloc'd copy, or 0); the
 * imports made are listed in `imports`, the files read in `images`. */
typedef struct {
    unsigned char *blob;
    unsigned *table;
    unsigned capacity;
    unsigned room_start, room_size, room_used;
    int (*resolve)(const char *name, char *path, unsigned n);
    unsigned char *(*load_war)(const char *path, unsigned *size);
    LevelPatchImport imports[LEVELPATCH_MAX_IMPORTS];
    int nimports;
    LevelPatchImage images[LEVELPATCH_MAX_IMAGES];
    int nimages;
    /* the object classes brought from other levels (`new ... from <Level>`), for the loader to bring their inventory
     * icons and their text too; and the text a patch gives a class (`text <class> "..."`) */
    struct {
        unsigned short classId;
        char source[LEVELPATCH_PATH];
    } classes[LEVELPATCH_MAX_IMPORTS];
    int nclasses;
    struct {
        unsigned short classId;
        unsigned short length;
        char list[LEVELPATCH_TEXT]; /* as the .MLT holds it: u8 count, then that many zero-terminated strings */
    } texts[16];
    int ntexts;
    /* single strings a patch changes or adds in a class's list (`text <class> <n> "..."`: a sign's text), applied
     * over its list in the language the game runs in */
    struct {
        unsigned short classId, index;
        char string[LEVELPATCH_STRING];
    } strings[64];
    int nstrings;
    /* the files whose characters' animations were filled in (fill_anims), once per source */
    char animsFrom[LEVELPATCH_MAX_IMAGES][LEVELPATCH_PATH];
    int nanimsFrom;
} LevelPatchTarget;

#define LEVELPATCH_MAX_RECORD 4096  /* the room one `new` object may take */
#define LEVELPATCH_MAX_IMPORT 16384 /* the room one imported model may take: its record and its animation lists */
#define LEVELPATCH_TYPE_IMPORTED 0x08 /* an imported model's type until its Mesh is built (then 0x43) */
#define LEVELPATCH_TYPE_UNPLACED 0x40 /* with type 3 or 4: a model for objects, built as a Mesh but not placed as level
                                       * geometry (GetResourceType class 3) */

/* How many resources the patch text adds (its `new` and `import` lines) and the room they need in the image, to size
 * the table and the image before loading. */
int levelpatch_count_new(const char *text);
unsigned levelpatch_room(const char *text);

/* Applies a level patch (levelpatch.c). The number of changes made; each line it cannot apply is reported through log. */
int levelpatch_apply(LevelPatchTarget *t, const char *text, const LevelPatchProp *props, int nprops,
                     void (*log)(const char *fmt, ...));

/* A texture page of a .DAV (a VDX7 file; PolyBatcher_LoadTexturePages 0x416fe2 reads the same): its size, its format
 * word (format & 3 the pixel format, format & 0x1c its kind: 4 opaque, 8 blended, 0x10 additive) and its pixels
 * (16 bits each, row by row). 0 when the file or the page is not there. */
typedef struct {
    unsigned w, h, format;
    const unsigned short *pixels;
} LevelPatchPage;
int levelpatch_dav_page(const unsigned char *dav, unsigned size, unsigned page, LevelPatchPage *out);

/* The number of texture rectangles of a .DAV (the Vdx7's records: {u16 x, w, y, h, page}), 0 if it is not one. */
unsigned levelpatch_dav_rects(const unsigned char *dav, unsigned size);

/* The texture pages of the .DAV that resource `res` of a .WAR draws with (its textured entries' ids, looked up as
 * Vdx7 0x41b410 does), sorted, at most `max`; the number, or -1 when the files do not hold such a model. */
int levelpatch_model_pages(const unsigned char *war, unsigned warSize, unsigned res, const unsigned char *dav,
                           unsigned davSize, unsigned short *pages, int max);

/* The sounds an object of another level needs: those its class's code names (`classSounds`, pairs {class, sound}
 * generated from the source by tools/build_mods.py), and those the animations it brings name in their keys: its own
 * model's, and the characters' animations that level has and this one lacks (fill_anims). From the two levels' .WAR
 * files as they are on disc. The number found (at most `max`), or -1. */
int levelpatch_object_sounds(const unsigned char *src, unsigned srcSize, const unsigned char *dst, unsigned dstSize,
                             unsigned res, const unsigned short (*classSounds)[2], int nClassSounds,
                             unsigned short *ids, int max);

/* A text bank (.MLT: u32; "v1.2"; u16 blocks; u16 lists; u32; then per language block, per list: u8 count and that
 * many zero-terminated strings): list `list` of block `block`, and its length in bytes. 0 if there is no such list. */
const unsigned char *levelpatch_mlt_list(const unsigned char *mlt, unsigned size, unsigned block, unsigned list,
                                         unsigned *length);
/* the block of a text bank whose list `list` is these bytes (the language the game loaded), or -1 */
int levelpatch_mlt_block(const unsigned char *mlt, unsigned size, unsigned list, const unsigned char *bytes);

#endif
