/* Level patches: a mod's levels\<Level>.txt (for example levels\Lvl-03.txt, or levels\Scene.txt for the studio)
 * changes the level's objects in memory, right after the game has read the .WAR and before it creates any object. One
 * change per line; `#` starts a comment. <res> is the object's resource index, as `python3 tools/war_objects.py
 * Lvl-03` lists it.
 *
 *     <res> pos <x> <y> <z>          place the object (y points down)
 *     <res> move <dx> <dy> <dz>      move it by that much
 *     <res> rot <rx> <ry> <rz>       its rotation (4096 = one turn)
 *     <res> model <res|name>         the model resource it shows (or a model named by `import`)
 *     <res> class <id>               its class (a mod's class: SdwModApi.register_class)
 *     <res> prop <NAME|offset> <v>   a designer property: its name in Scenaric_Classes.h, or its byte offset
 *     <res> remove                   the object is not created
 *     new <name> from <res>          a new object, a copy of object <res> (its class, model, properties); later lines
 *                                    change it by its name: `<name> pos ...`. Names are local to the file.
 *     new <name> from <Level> <res>  the same with an object of another level: its model is imported with it
 *     sound <id> from <Level>        that level's sound <id> added to this level's sound bank (done by the loader
 *                                    when the game opens the level's .SND). An object brought from another level
 *                                    brings its sounds without these lines (the loader works them out).
 *     text <class> "<string>" ...    the name and help the map screen shows for a class: one or more strings, with
 *                                    \n for a new line (the game's own: "Fan:\nThe fan moves air around.\n...").
 *     text <class> <n> "<string>"    string <n> of the class's list, changed, or added after its last (a sign shows
 *                                    string INDEXTEXT of its class's list). Letters with accents are written as
 *                                    they are (UTF-8); the game's text is Latin-1.
 *     import <name> from <Level> <res>   a model of another level (its resource <res>, as `python3 tools/war_meshes.py
 *                                    <Level>` lists them), added to this one: `<res> model <name>` shows it. A model
 *                                    objects use (type 0x43 static or 0x44 animated), with its texture pages.
 *                                    <Level> can also be mod:<file>: <file>.WAR / .DAV in the mod's own folder, made by
 *                                    tools/make_model.py.
 *
 * Numbers are decimal or 0x hex. Removing an object that others refer to (a trajectory, a switch target) can crash. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "levelpatch.h"

#define REC_MODEL 0x00
#define REC_POS 0x04
#define REC_CLASS 0x0A
#define REC_ROT 0x0C
#define REC_PROPS 0x14
#define TYPE_SCENARIC 5
#define TYPE_IGNORED 1 /* GetResourceType class 4: skipped by every install loop */
#define TYPE_MESH 3    /* a static model (with bit 0x40, 0x43: one for objects) */
#define TYPE_MODEL 4   /* an animated model (0x44) */

/* bytes per element of each geometry entry kind (BsFile_SkipEntry 0x41fc89, data/enums/BsEntryKind.csv) */
static const unsigned char kEntryStride[23] = {8,  8,  16, 20, 24, 24, 40, 48, 48, 48, 48, 48,
                                               24, 24, 24, 24, 12, 12, 20, 24, 24, 40, 20};

static void lstrcpyn_(char *d, const char *s, size_t n)
{
    strncpy(d, s, n - 1);
    d[n - 1] = 0;
}

static int word(char **p, char *out, int n)
{
    int i = 0;
    while (**p == ' ' || **p == '\t')
        (*p)++;
    while (**p && **p != ' ' && **p != '\t' && **p != '#' && i < n - 1)
        out[i++] = *(*p)++;
    out[i] = 0;
    return i;
}

/* a "..." string with \n, \" and \\ inside, into out (zero-terminated, UTF-8 letters U+0080-U+00FF made the
 * game's Latin-1): its length, or -1 */
static int quoted(char **p, char *out, int size)
{
    int n = 0;
    while (**p == ' ' || **p == '\t')
        (*p)++;
    if (**p != '"')
        return -1;
    (*p)++;
    while (**p && **p != '"' && n < size - 1) {
        unsigned char ch = (unsigned char)*(*p)++;
        if (ch == '\\' && **p)
            ch = **p == 'n' ? ((*p)++, '\n') : (unsigned char)*(*p)++;
        else if ((ch == 0xC2 || ch == 0xC3) && ((unsigned char)**p & 0xC0) == 0x80)
            ch = (unsigned char)(((ch & 3) << 6) | ((unsigned char)*(*p)++ & 0x3F));
        out[n++] = (char)ch;
    }
    if (**p != '"')
        return -1;
    (*p)++;
    out[n] = 0;
    return n;
}

static int number(char **p, long *v)
{
    char w[32], *end;
    if (!word(p, w, sizeof w))
        return 0;
    *v = strtol(w, &end, 0);
    return *end == 0;
}

typedef struct {
    char name[32];
    unsigned res;
} Named;

/* the object index a line names: a number, or a name given by an earlier `new` */
static int object_index(const char *w, const Named *names, int nnames, long *res)
{
    char *end;
    int i;
    *res = strtol(w, &end, 0);
    if (!*end && *w)
        return 1;
    for (i = 0; i < nnames; i++)
        if (!strcmp(names[i].name, w)) {
            *res = names[i].res;
            return 1;
        }
    return 0;
}

/* a record's length: up to the next resource of the image (records are laid out one after another) */
static unsigned record_length(const unsigned *table, unsigned count, unsigned off, unsigned end)
{
    unsigned i, next = off + LEVELPATCH_MAX_RECORD < end ? off + LEVELPATCH_MAX_RECORD : end;
    for (i = 0; i < count; i++) {
        unsigned o = table[i] & 0xFFFFFF;
        if (o > off && o < next)
            next = o;
    }
    return next - off;
}

static int words_in_line(const char *s)
{
    int n = 0;
    while (*s && *s != '\n' && *s != '#') {
        while (*s == ' ' || *s == '\t' || *s == '\r')
            s++;
        if (!*s || *s == '\n' || *s == '#')
            break;
        n++;
        while (*s && *s != ' ' && *s != '\t' && *s != '\r' && *s != '\n' && *s != '#')
            s++;
    }
    return n;
}

/* the resources (0) and the room in the image (1: for records, 2: for imported models) the lines of a patch need */
static unsigned count_lines(const char *text, int which)
{
    unsigned n = 0;
    const char *s = text;
    while (*s) {
        while (*s == ' ' || *s == '\t')
            s++;
        if (!strncmp(s, "new", 3) && (s[3] == ' ' || s[3] == '\t')) { /* from another level: its model too */
            int other = words_in_line(s) >= 5;
            if (which != 2)
                n += which ? LEVELPATCH_MAX_RECORD : 1;
            if (which != 1 && other)
                n += which ? LEVELPATCH_MAX_IMPORT : 1;
        }
        if (which != 1 && !strncmp(s, "import", 6) && (s[6] == ' ' || s[6] == '\t'))
            n += which ? LEVELPATCH_MAX_IMPORT : 1;
        s = strchr(s, '\n');
        if (!s)
            break;
        s++;
    }
    return n;
}

int levelpatch_count_new(const char *text)
{
    return (int)count_lines(text, 0);
}

unsigned levelpatch_room(const char *text)
{
    return count_lines(text, 1) + count_lines(text, 2);
}

/* the .DAV's tables: {u16 entry count, u16 rectangle count, u16 page count, u32 entries, u32 rectangles, u32 pages}
 * at the offset the u32 at 0x14 gives */
static int dav_tables(const unsigned char *dav, unsigned size, unsigned *ne, unsigned *nr, unsigned *np, unsigned *ent,
                      unsigned *rec, unsigned *pages)
{
    unsigned h;
    if (size < 0x18 || memcmp(dav, "VDX7", 4) || (h = *(const unsigned *)(dav + 0x14)) > size - 18)
        return 0;
    *ne = *(const unsigned short *)(dav + h);
    *nr = *(const unsigned short *)(dav + h + 2);
    *np = *(const unsigned short *)(dav + h + 4);
    *ent = *(const unsigned *)(dav + h + 6);
    *rec = *(const unsigned *)(dav + h + 10);
    *pages = *(const unsigned *)(dav + h + 14);
    return *ent <= size && *ne * 2 <= size - *ent && *rec <= size && *nr * 10 <= size - *rec && *pages <= size;
}

unsigned levelpatch_dav_rects(const unsigned char *dav, unsigned size)
{
    unsigned ne, nr, np, ent, rec, pages;
    return dav_tables(dav, size, &ne, &nr, &np, &ent, &rec, &pages) ? nr : 0;
}

int levelpatch_dav_page(const unsigned char *dav, unsigned size, unsigned page, LevelPatchPage *out)
{
    unsigned ne, nr, np, ent, rec, o, i;
    if (!dav_tables(dav, size, &ne, &nr, &np, &ent, &rec, &o) || page >= np)
        return 0;
    for (i = 0;; i++) { /* pages follow one another: {u16 x0, x1, y0, y1, format; u16 pixels[w * h]} */
        const unsigned short *hd = (const unsigned short *)(dav + o);
        unsigned w, h;
        if (o + 10 > size)
            return 0;
        w = hd[0] > hd[1] ? hd[0] - hd[1] : hd[1] - hd[0];
        h = hd[2] > hd[3] ? hd[2] - hd[3] : hd[3] - hd[2];
        if (w * h * 2 > size - o - 10)
            return 0;
        if (i == page) {
            out->w = w;
            out->h = h;
            out->format = hd[4];
            out->pixels = hd + 5;
            return 1;
        }
        o += 10 + w * h * 2;
    }
}

/* the entry table of the geometry record at `rec` (+4 its offset, +0xa its count), checked: its end, or 0 */
static unsigned entries_end(const unsigned char *d, unsigned size, unsigned rec)
{
    unsigned o, i, ne;
    if (rec + 0x0C > size)
        return 0;
    o = *(const unsigned *)(d + rec + 4);
    ne = *(const unsigned short *)(d + rec + 0x0A);
    for (i = 0; i < ne; i++) { /* {u16 kind; u16 count; payload[count * stride]} */
        unsigned kind;
        if (o > size || size - o < 4 || (kind = *(const unsigned short *)(d + o)) >= sizeof kEntryStride)
            return 0;
        o += 4 + *(const unsigned short *)(d + o + 2) * kEntryStride[kind];
    }
    return o <= size ? o : 0;
}

int levelpatch_model_pages(const unsigned char *war, unsigned warSize, unsigned res, const unsigned char *dav,
                           unsigned davSize, unsigned short *pages, int max)
{
    unsigned ne, nr, np, entAt, recAt, pagesAt, count, rec, o, i, k;
    int n = 0;
    if (!dav_tables(dav, davSize, &ne, &nr, &np, &entAt, &recAt, &pagesAt) || warSize < 0x10 ||
        (count = *(const unsigned *)(war + 0x0C)) > (warSize - 0x10) / 4 || res >= count)
        return -1;
    rec = ((const unsigned *)(war + 0x10))[res] & 0xFFFFFF;
    if (!entries_end(war, warSize, rec))
        return -1;
    o = *(const unsigned *)(war + rec + 4);
    for (i = *(const unsigned short *)(war + rec + 0x0A); i; i--) {
        unsigned kind = *(const unsigned short *)(war + o), cnt = *(const unsigned short *)(war + o + 2), at;
        /* the texture id: at element offset 12 for FT3/FT4/GT3/GT4/BFT3/BGT3, 4 for the fixed-square kinds 8-0xf
         * (the decoders in src/engine/bs_file.cpp) */
        at = (kind >= 4 && kind <= 7) || kind == 0x14 || kind == 0x15 ? 12 : kind >= 8 && kind <= 0x0F ? 4 : 0;
        for (k = 0; at && k < cnt; k++) {
            unsigned id = *(const unsigned *)(war + o + 4 + k * kEntryStride[kind] + at), r, page, j;
            if (id >= ne || (r = ((const unsigned short *)(dav + entAt))[id]) >= nr)
                return -1;
            page = *(const unsigned short *)(dav + recAt + r * 10 + 8);
            for (j = 0; j < (unsigned)n && pages[j] != page; j++)
                ;
            if (j == (unsigned)n) {
                if (n >= max)
                    return -1;
                for (j = n++; j && pages[j - 1] > page; j--) /* kept sorted */
                    pages[j] = pages[j - 1];
                pages[j] = (unsigned short)page;
            }
        }
        o += 4 + cnt * kEntryStride[kind];
    }
    return n;
}

/* the other file, read once per patch target and kept */
static const LevelPatchImage *import_image(LevelPatchTarget *t, const char *source)
{
    LevelPatchImage *im;
    int i;
    for (i = 0; i < t->nimages; i++)
        if (!strcmp(t->images[i].source, source))
            return &t->images[i];
    if (t->nimages >= LEVELPATCH_MAX_IMAGES || !t->load_war)
        return 0;
    im = &t->images[t->nimages];
    if (!(im->data = t->load_war(source, &im->size)))
        return 0;
    lstrcpyn_(im->source, source, sizeof im->source);
    t->nimages++;
    return im;
}

/* import <name> from <Level> <res>: model <res> of that level (type 0x43 static or 0x44 animated), added here. Its
 * record is copied into the room and relocated as Load_WAR relocates the level's own (its offsets made addresses), but
 * against the other level's file, which stays loaded: vertices, polygons, boxes, joints and animations are read there.
 * A static record is 0x14 bytes {vertices, polygons, u16 vertex count, u16 entry count, boxes or 0, "GEOM"}; an animated
 * one 0x20, words 0, 1, 3 (if set), 4 and 5 relocated, and word 4 points at two counted lists of animation offsets
 * {u32 n; u32 off[n]; u32 m; u32 off[m]} (0 = none), which are copied after the record and relocated too, so the
 * other file is never changed. The table entry is LEVELPATCH_TYPE_IMPORTED until the loader has built the Mesh.
 * `level` is the name the patch gives; t->resolve gives the file's path. 0, or the reason it cannot. */
/* the file a patch names (a level, or mod:<file>): its path in `source` and its image, or the reason it cannot */
static const char *source_image(LevelPatchTarget *t, const char *level, char *source, const LevelPatchImage **img)
{
    const char *name = strncmp(level, "mod:", 4) ? level : level + 4;
    unsigned i;
    /* a level's folder name, or mod:<file> with folders: letters, digits, - _ \ /, and no ".." */
    for (i = 0; name[i]; i++)
        if (!(name[i] == '-' || name[i] == '_' || (name[i] >= '0' && name[i] <= '9') ||
              ((name[i] | 0x20) >= 'a' && (name[i] | 0x20) <= 'z') ||
              (name != level && (name[i] == '\\' || name[i] == '/') && i && name[i + 1])))
            return "a level is a name like Lvl-11, a model file mod:<file>";
    if (!i || i >= 64)
        return "a level is a name like Lvl-11, a model file mod:<file>";
    if (!t->resolve || !t->resolve(level, source, LEVELPATCH_PATH) || !(*img = import_image(t, source)))
        return "its .WAR cannot be read";
    return 0;
}

static const char *import_model(LevelPatchTarget *t, const char *level, unsigned res, unsigned *outIndex)
{
    unsigned char *blob = t->blob;
    const LevelPatchImage *img;
    const unsigned char *src;
    unsigned size, count, rec, e, type, recLen, listLen = 0, n = 0, m = 0, lists = 0, off, k, base, *w;
    unsigned *pcount = (unsigned *)(blob + 0x0C);
    char source[LEVELPATCH_PATH];
    const char *why;
    int i;
    if (t->nimports >= LEVELPATCH_MAX_IMPORTS || *pcount >= t->capacity)
        return "no room for another import";
    if ((why = source_image(t, level, source, &img)) != 0)
        return why;
    for (i = 0; i < t->nimports; i++) /* imported already (two objects of that level showing it) */
        if (t->imports[i].res == res && !strcmp(t->imports[i].source, source)) {
            *outIndex = t->imports[i].index;
            return 0;
        }
    src = img->data;
    size = img->size;
    base = (unsigned)(size_t)src;
    if (size < 0x10 || (count = *(const unsigned *)(src + 0x0C)) > (size - 0x10) / 4 || res >= count)
        return "no such resource in that level";
    e = ((const unsigned *)(src + 0x10))[res];
    rec = e & 0xFFFFFF;
    type = e >> 24;
    if (type != (TYPE_MESH | LEVELPATCH_TYPE_UNPLACED) && type != (TYPE_MODEL | LEVELPATCH_TYPE_UNPLACED))
        return "not a model objects use (type 0x43 or 0x44; tools/war_meshes.py lists them)";
    recLen = type == (TYPE_MODEL | LEVELPATCH_TYPE_UNPLACED) ? 0x20 : 0x14;
    if (rec + recLen > size || !entries_end(src, size, rec))
        return "a damaged model";
    if (recLen == 0x20) { /* the animation lists */
        lists = ((const unsigned *)(src + rec))[4];
        if (lists > size - 8 || (n = *(const unsigned *)(src + lists)) > (size - lists - 8) / 4 ||
            (m = *(const unsigned *)(src + lists + 4 + n * 4)) > (size - lists - 8 - n * 4) / 4)
            return "a damaged model";
        listLen = 8 + (n + m) * 4;
    }
    off = (t->room_start + t->room_used + 3) & ~3u;
    if (recLen + listLen > LEVELPATCH_MAX_IMPORT || off + recLen + listLen > t->room_start + t->room_size ||
        off + recLen + listLen > 0xFFFFFF)
        return "no room for another import";
    memcpy(blob + off, src + rec, recLen);
    w = (unsigned *)(blob + off);
    w[0] += base;
    w[1] += base;
    if (w[3])
        w[3] += base;
    if (recLen == 0x20) {
        unsigned *l = (unsigned *)(blob + off + recLen);
        memcpy(l, src + lists, listLen);
        for (k = 0; k < n + m + 1; k++) /* both lists; the second count (l[n + 1]) is not an offset */
            if (k != n && l[1 + k])
                l[1 + k] += base;
        w[4] = (unsigned)(size_t)l;
        w[5] += base;
    }
    t->room_used = off + recLen + listLen - t->room_start;
    t->table[*pcount] = ((unsigned)LEVELPATCH_TYPE_IMPORTED << 24) | off;
    t->imports[t->nimports].index = *pcount;
    lstrcpyn_(t->imports[t->nimports].source, source, sizeof t->imports[0].source);
    t->imports[t->nimports].res = res;
    t->imports[t->nimports++].type = type;
    *outIndex = (*pcount)++;
    return 0;
}

/* ---- models and animations, in a level's image (relocated: its words are addresses) or in a file (offsets) ---- */
typedef struct {
    const unsigned char *data;
    unsigned size;
    int relocated;
    const unsigned *table;
    unsigned count;
} Img;

/* `len` bytes at a record's word `v`: an address in a relocated image, an offset in a file (checked) */
static const unsigned char *img_at(const Img *m, unsigned v, unsigned len)
{
    if (m->relocated)
        return v ? (const unsigned char *)(size_t)v : 0;
    return v && v <= m->size && len <= m->size - v ? m->data + v : 0;
}

static int img_file(Img *m, const unsigned char *data, unsigned size)
{
    m->data = data;
    m->size = size;
    m->relocated = 0;
    if (size < 0x10 || (m->count = *(const unsigned *)(data + 0x0C)) > (size - 0x10) / 4)
        return 0;
    m->table = (const unsigned *)(data + 0x10);
    return 1;
}

/* an animated model's record (type 4, with or without bit 0x40), or 0 */
static const unsigned *model_record(const Img *m, unsigned index)
{
    unsigned e;
    if (index >= m->count || (((e = m->table[index]) >> 24) & ~0x40u) != TYPE_MODEL ||
        (!m->relocated && (e & 0xFFFFFF) + 0x20 > m->size))
        return 0;
    return (const unsigned *)(m->data + (e & 0xFFFFFF));
}

/* the model of the first object of a class, or -1 */
static int class_model(const Img *m, unsigned classId)
{
    unsigned i, e;
    for (i = 0; i < m->count; i++)
        if ((((e = m->table[i]) >> 24) & ~0x40u) == TYPE_SCENARIC && (m->relocated || (e & 0xFFFFFF) + 0x14 <= m->size) &&
            *(const unsigned short *)(m->data + (e & 0xFFFFFF) + REC_CLASS) == classId)
            return *(const unsigned short *)(m->data + (e & 0xFFFFFF) + REC_MODEL);
    return -1;
}

/* a model's animation lists {u32 n; off[n]; u32 m; off[m]} (the second indexed by the game's animation id): the
 * second list's count and its entries, or 0 */
static const unsigned *mapped_list(const Img *m, const unsigned *rec, unsigned *count)
{
    const unsigned char *l = img_at(m, rec[4], 8);
    unsigned n;
    if (!l || (n = *(const unsigned *)l) > 0x10000 || !img_at(m, rec[4], 8 + 4 * n) ||
        !img_at(m, rec[4], 8 + 4 * n + 4 * (*count = ((const unsigned *)l)[1 + n])))
        return 0;
    return (const unsigned *)l + 2 + n;
}

/* the same skeleton: as many joints, and the same joint table (parents and offsets) */
static int same_skeleton(const Img *a, const unsigned *ra, const Img *b, const unsigned *rb)
{
    unsigned nj = ra[6] & 0xFFFF;
    const unsigned char *ja, *jb;
    return nj && nj == (rb[6] & 0xFFFF) && (ja = img_at(a, ra[5], nj * 10)) && (jb = img_at(b, rb[5], nj * 10)) &&
           !memcmp(ja, jb, nj * 10);
}

/* the sound ids the keys of an animation name (AnimKey.eventId) */
static int anim_events(const Img *m, unsigned anim, unsigned short *ids, int n, int max)
{
    const unsigned char *a = img_at(m, anim, 10);
    unsigned keys, k, o = 10;
    if (!a)
        return n;
    keys = *(const unsigned short *)(a + 8);
    for (k = 0; k < keys && img_at(m, anim, o + 8); k++) {
        unsigned short ev = *(const unsigned short *)(a + o + 6);
        int j;
        for (j = 0; ev && j < n && ids[j] != ev; j++)
            ;
        if (ev && j == n && n < max)
            ids[n++] = ev;
        o += 8 + 2 * *(const unsigned short *)(a + o + 4);
    }
    return n;
}

/* the characters' animations `src` has and `dst` lacks: for each class with an animated model in both levels and the
 * same skeleton, the entries of the animation-id list `dst` has empty and `src` has. With `fill`, dst's entries are
 * set (dst relocated: src's offsets made addresses against `base`); with `ids`, the sounds those animations name are
 * collected. The number of animations. */
static int missing_anims(const Img *dst, const Img *src, unsigned base, int fill, unsigned short *ids, int *nids,
                         int max, void (*log)(const char *fmt, ...), const char *from)
{
    unsigned c, i, total = 0;
    for (c = 0; c < 200; c++) {
        int md = class_model(dst, c), ms = class_model(src, c);
        const unsigned *rd, *rs, *ld, *ls;
        unsigned nd, ns, got = 0;
        if (md < 0 || ms < 0 || !(rd = model_record(dst, md)) || !(rs = model_record(src, ms)) ||
            !same_skeleton(dst, rd, src, rs) || !(ld = mapped_list(dst, rd, &nd)) || !(ls = mapped_list(src, rs, &ns)))
            continue;
        for (i = 0; i < nd && i < ns; i++)
            if (!ld[i] && ls[i] && img_at(src, ls[i], 10)) {
                if (fill)
                    ((unsigned *)ld)[i] = ls[i] + base;
                if (ids)
                    *nids = anim_events(src, ls[i], ids, *nids, max);
                got++;
            }
        if (got && log)
            log("class %u: %u animation(s) from %s", c, got, from);
        total += got;
    }
    return (int)total;
}

/* fills in this level's characters' missing animations from a file an object was brought from (once per file) */
static void fill_anims(LevelPatchTarget *t, const LevelPatchImage *img, void (*log)(const char *fmt, ...))
{
    Img dst, src;
    int i;
    for (i = 0; i < t->nanimsFrom; i++)
        if (!strcmp(t->animsFrom[i], img->source))
            return;
    if (t->nanimsFrom < LEVELPATCH_MAX_IMAGES)
        lstrcpyn_(t->animsFrom[t->nanimsFrom++], img->source, LEVELPATCH_PATH);
    dst.data = t->blob;
    dst.size = 0;
    dst.relocated = 1;
    dst.table = t->table;
    dst.count = *(unsigned *)(t->blob + 0x0C);
    if (img_file(&src, img->data, img->size))
        missing_anims(&dst, &src, (unsigned)(size_t)img->data, 1, 0, 0, 0, log, img->source);
}

int levelpatch_object_sounds(const unsigned char *srcData, unsigned srcSize, const unsigned char *dstData,
                             unsigned dstSize, unsigned res, const unsigned short (*classSounds)[2], int nClassSounds,
                             unsigned short *ids, int max)
{
    Img src, dst;
    unsigned e, rec, cls, i, n, lists;
    const unsigned *mr;
    int nids = 0;
    if (!img_file(&src, srcData, srcSize) || !img_file(&dst, dstData, dstSize) || res >= src.count ||
        (((e = src.table[res]) >> 24) & ~0x40u) != TYPE_SCENARIC || (rec = e & 0xFFFFFF) + 0x14 > srcSize)
        return -1;
    cls = *(const unsigned short *)(srcData + rec + REC_CLASS);
    for (i = 0; i < (unsigned)nClassSounds; i++) /* what its class's code plays */
        if (classSounds[i][0] == cls) {
            int j;
            for (j = 0; j < nids && ids[j] != classSounds[i][1]; j++)
                ;
            if (j == nids && nids < max)
                ids[nids++] = classSounds[i][1];
        }
    if ((mr = model_record(&src, *(const unsigned short *)(srcData + rec + REC_MODEL))) != 0 &&
        img_at(&src, mr[4], 4)) { /* what its own model's animations name (both lists: every animation) */
        n = *(const unsigned *)(srcData + mr[4]);
        for (lists = 0; lists < 2; lists++) {
            const unsigned char *l = img_at(&src, mr[4], 8 + 4 * n);
            unsigned k, count = lists ? (l ? ((const unsigned *)l)[1 + n] : 0) : n;
            const unsigned *entries = l ? (const unsigned *)l + (lists ? 2 + n : 1) : 0;
            if (!entries || (lists && !img_at(&src, mr[4], 8 + 4 * n + 4 * count)))
                break;
            for (k = 0; k < count; k++)
                nids = anim_events(&src, entries[k], ids, nids, max);
        }
    }
    missing_anims(&dst, &src, 0, 0, ids, &nids, max, 0, 0); /* what the characters' animations it brings name */
    return nids;
}

/* ---- text banks (.MLT) ---- */
const unsigned char *levelpatch_mlt_list(const unsigned char *mlt, unsigned size, unsigned block, unsigned list,
                                         unsigned *length)
{
    unsigned blocks, lists, o = 16, b, l, k;
    if (size < 16 || memcmp(mlt + 4, "v1.2", 4))
        return 0;
    blocks = *(const unsigned short *)(mlt + 8);
    lists = *(const unsigned short *)(mlt + 10);
    if (block >= blocks || list >= lists)
        return 0;
    for (b = 0; b <= block; b++)
        for (l = 0; l < lists; l++) {
            unsigned start = o, count;
            if (o >= size)
                return 0;
            count = mlt[o++];
            for (k = 0; k < count; k++) {
                const unsigned char *z = memchr(mlt + o, 0, size - o);
                if (!z)
                    return 0;
                o = (unsigned)(z - mlt) + 1;
            }
            if (b == block && l == list) {
                *length = o - start;
                return mlt + start;
            }
        }
    return 0;
}

int levelpatch_mlt_block(const unsigned char *mlt, unsigned size, unsigned list, const unsigned char *bytes)
{
    unsigned b, len;
    const unsigned char *l;
    for (b = 0; (l = levelpatch_mlt_list(mlt, size, b, list, &len)) != 0; b++)
        if (!memcmp(l, bytes, len))
            return (int)b;
    return -1;
}

/* new <name> from <Level> <res>: object <res> of another level (or of a mod's model file), added here: its record is
 * copied into the room (up to the next resource of its file, as for a `new` from this level) and its model imported
 * as `import` does (once per model), the record's model index changed to the imported one. Other resources its
 * properties name are not brought across: an object whose class refers to such (a trajectory, a switch's target)
 * needs them changed by later lines. 0, or the reason it cannot. */
static const char *import_object(LevelPatchTarget *t, const char *level, unsigned res, unsigned *outIndex,
                                 void (*log)(const char *fmt, ...))
{
    unsigned char *blob = t->blob;
    unsigned *pcount = (unsigned *)(blob + 0x0C), count, rec, next, i, len, off, model, e;
    const LevelPatchImage *img;
    const unsigned *table;
    char source[LEVELPATCH_PATH];
    const char *why;
    static char reason[96];
    if ((why = source_image(t, level, source, &img)) != 0)
        return why;
    if (img->size < 0x10 || (count = *(const unsigned *)(img->data + 0x0C)) > (img->size - 0x10) / 4 || res >= count)
        return "no such resource in that level";
    table = (const unsigned *)(img->data + 0x10);
    if (((table[res] >> 24) & ~0x40u) != TYPE_SCENARIC)
        return "not an object of that level";
    rec = table[res] & 0xFFFFFF;
    next = rec + LEVELPATCH_MAX_RECORD < img->size ? rec + LEVELPATCH_MAX_RECORD : img->size;
    for (i = 0; i < count; i++) /* its length: up to the next resource of its file */
        if ((e = table[i] & 0xFFFFFF) > rec && e < next)
            next = e;
    if (next < rec + REC_PROPS)
        return "a damaged object";
    len = next - rec;
    model = *(const unsigned short *)(img->data + rec + REC_MODEL);
    if ((why = import_model(t, level, model, &model)) != 0) {
        _snprintf(reason, sizeof reason, "its model %u: %s", *(const unsigned short *)(img->data + rec + REC_MODEL), why);
        return reason;
    }
    off = (t->room_start + t->room_used + 3) & ~3u;
    if (*pcount >= t->capacity || off + len > t->room_start + t->room_size || off > 0xFFFFFF)
        return "no room for another new object";
    memcpy(blob + off, img->data + rec, len);
    *(unsigned short *)(blob + off + REC_MODEL) = (unsigned short)model;
    t->room_used = off + len - t->room_start;
    t->table[*pcount] = (TYPE_SCENARIC << 24) | off;
    *outIndex = (*pcount)++;
    for (i = 0; i < (unsigned)t->nclasses; i++) /* its class: the loader brings its icon and text */
        if (t->classes[i].classId == *(unsigned short *)(blob + off + REC_CLASS) && !strcmp(t->classes[i].source, source))
            break;
    if (i == (unsigned)t->nclasses && t->nclasses < LEVELPATCH_MAX_IMPORTS) {
        t->classes[t->nclasses].classId = *(unsigned short *)(blob + off + REC_CLASS);
        lstrcpyn_(t->classes[t->nclasses++].source, source, LEVELPATCH_PATH);
    }
    fill_anims(t, img, log);
    return 0;
}

static int prop_offset(const LevelPatchProp *props, int nprops, unsigned classId, const char *name)
{
    int i;
    for (i = 0; i < nprops; i++)
        if (props[i].classId == classId && !strcmp(props[i].name, name))
            return props[i].offset;
    return -1;
}

int levelpatch_apply(LevelPatchTarget *t, const char *text, const LevelPatchProp *props, int nprops,
                     void (*log)(const char *fmt, ...))
{
    unsigned char *blob = t->blob;
    unsigned *count = (unsigned *)(blob + 0x0C);
    unsigned *table = t->table;
    static Named names[256];
    int nnames = 0, applied = 0, lineNo = 0;
    const char *line = text;
    while (*line) {
        char buf[256], op[16], *p = buf;
        const char *eol = strchr(line, '\n');
        size_t len = eol ? (size_t)(eol - line) : strlen(line);
        long res, a, b, c;
        unsigned char *rec;
        char first[32];
        lineNo++;
        if (len >= sizeof buf)
            len = sizeof buf - 1;
        memcpy(buf, line, len);
        buf[len] = 0;
        if (len && buf[len - 1] == '\r')
            buf[len - 1] = 0;
        line = eol ? eol + 1 : line + len;
        while (*p == ' ' || *p == '\t')
            p++;
        if (!*p || *p == '#')
            continue;
        word(&p, first, sizeof first);
        if (!strcmp(first, "sound")) /* sound <id> from <Level>: the loader adds it to the level's sound bank */
            continue;
        if (!strcmp(first, "text")) { /* text <class> "<string>" ...: the class's name and help (the map screen) */
            unsigned char list[LEVELPATCH_TEXT];
            unsigned len = 1, cls, k;
            int bad = 0;
            if (!number(&p, &a) || a < 0 || a > 0xFFFE) {
                log("line %d: expected text <class id> \"<string>\" ...", lineNo);
                continue;
            }
            cls = (unsigned)a;
            while (*p == ' ' || *p == '\t')
                p++;
            if (*p >= '0' && *p <= '9') { /* text <class> <n> "<string>": one string of the class's list */
                char one[LEVELPATCH_STRING];
                if (!number(&p, &a) || a < 0 || a > 254 || quoted(&p, one, sizeof one) < 0) {
                    log("line %d: expected text <class id> <n> \"<string>\"", lineNo);
                    continue;
                }
                for (k = 0; k < (unsigned)t->nstrings && (t->strings[k].classId != cls || t->strings[k].index != a); k++)
                    ;
                if (k == (unsigned)t->nstrings) {
                    if (t->nstrings >= (int)(sizeof t->strings / sizeof t->strings[0])) {
                        log("line %d: too many text lines", lineNo);
                        continue;
                    }
                    t->nstrings++;
                }
                t->strings[k].classId = (unsigned short)cls;
                t->strings[k].index = (unsigned short)a;
                memcpy(t->strings[k].string, one, sizeof one);
                applied++;
                continue;
            }
            list[0] = 0;
            while (*p) { /* each "..." a string; \n, \" and \\ inside */
                while (*p == ' ' || *p == '\t')
                    p++;
                if (!*p || *p == '#')
                    break;
                if (*p++ != '"' || list[0] == 255) {
                    bad = 1;
                    break;
                }
                while (*p && *p != '"' && len < sizeof list - 1) {
                    char ch = *p++;
                    if (ch == '\\' && *p)
                        ch = *p == 'n' ? (p++, '\n') : *p++;
                    list[len++] = (unsigned char)ch;
                }
                if (*p != '"') {
                    bad = 1;
                    break;
                }
                p++;
                list[len++] = 0;
                list[0]++;
            }
            if (bad || !list[0]) {
                log("line %d: expected text <class id> \"<string>\" ...", lineNo);
                continue;
            }
            for (k = 0; k < (unsigned)t->ntexts && t->texts[k].classId != cls; k++)
                ;
            if (k == (unsigned)t->ntexts) {
                if (t->ntexts >= (int)(sizeof t->texts / sizeof t->texts[0])) {
                    log("line %d: too many text lines", lineNo);
                    continue;
                }
                t->ntexts++;
            }
            t->texts[k].classId = (unsigned short)cls;
            t->texts[k].length = (unsigned short)len;
            memcpy(t->texts[k].list, list, len);
            applied++;
            continue;
        }
        if (!strcmp(first, "import")) { /* import <name> from <Level> <res> */
            char name[32], from[16], level[80];
            const char *why;
            unsigned index;
            if (!word(&p, name, sizeof name) || !word(&p, from, sizeof from) || strcmp(from, "from") ||
                !word(&p, level, sizeof level) || !number(&p, &res) || res < 0) {
                log("line %d: expected import <name> from <Level> <res>", lineNo);
                continue;
            }
            if (nnames >= 256)
                why = "too many names";
            else
                why = import_model(t, level, (unsigned)res, &index);
            if (why) {
                log("line %d: %s %ld: %s", lineNo, level, res, why);
                continue;
            }
            lstrcpyn_(names[nnames].name, name, sizeof names[nnames].name);
            names[nnames++].res = index;
            applied++;
            continue;
        }
        if (!strcmp(first, "new")) { /* new <name> from <res>, or new <name> from <Level> <res> */
            char name[32], from[16], src[80], other[32];
            unsigned len, off;
            if (!word(&p, name, sizeof name) || !word(&p, from, sizeof from) || strcmp(from, "from") ||
                !word(&p, src, sizeof src)) {
                log("line %d: expected new <name> from [<Level>] <res>", lineNo);
                continue;
            }
            if (word(&p, other, sizeof other)) { /* from another level */
                const char *why;
                unsigned index;
                char *end;
                res = strtol(other, &end, 0);
                if (*end || res < 0)
                    why = "expected new <name> from <Level> <res>";
                else if (nnames >= 256)
                    why = "too many names";
                else
                    why = import_object(t, src, (unsigned)res, &index, log);
                if (why) {
                    log("line %d: %s %s: %s", lineNo, src, other, why);
                    continue;
                }
                lstrcpyn_(names[nnames].name, name, sizeof names[nnames].name);
                names[nnames++].res = index;
                applied++;
                continue;
            }
            if (!object_index(src, names, nnames, &res)) {
                log("line %d: expected new <name> from [<Level>] <res>", lineNo);
                continue;
            }
            if (res < 0 || (unsigned long)res >= *count || ((table[res] >> 24) & ~0x40u) != TYPE_SCENARIC) {
                log("line %d: resource %ld is not an object of this level", lineNo, res);
                continue;
            }
            len = record_length(table, *count, table[res] & 0xFFFFFF, t->room_start);
            off = (t->room_start + t->room_used + 3) & ~3u;
            if (*count >= t->capacity || nnames >= 256 || off + len > t->room_start + t->room_size || off > 0xFFFFFF) {
                log("line %d: no room for another new object", lineNo);
                continue;
            }
            memcpy(blob + off, blob + (table[res] & 0xFFFFFF), len);
            t->room_used = off + len - t->room_start;
            table[*count] = (TYPE_SCENARIC << 24) | off;
            lstrcpyn_(names[nnames].name, name, sizeof names[nnames].name);
            names[nnames++].res = (*count)++;
            applied++;
            continue;
        }
        if (!object_index(first, names, nnames, &res) || !word(&p, op, sizeof op)) {
            log("line %d: expected <res> <change>: %s", lineNo, buf);
            continue;
        }
        if (res < 0 || (unsigned long)res >= *count || ((table[res] >> 24) & ~0x40u) != TYPE_SCENARIC) {
            log("line %d: resource %ld is not an object of this level", lineNo, res);
            continue;
        }
        rec = blob + (table[res] & 0xFFFFFF);
        if (!strcmp(op, "pos") || !strcmp(op, "move") || !strcmp(op, "rot")) {
            short *v = (short *)(rec + (op[0] == 'r' ? REC_ROT : REC_POS));
            if (!number(&p, &a) || !number(&p, &b) || !number(&p, &c)) {
                log("line %d: %s needs three numbers", lineNo, op);
                continue;
            }
            if (!strcmp(op, "move")) {
                a += v[0];
                b += v[1];
                c += v[2];
            }
            v[0] = (short)a;
            v[1] = (short)b;
            v[2] = (short)c;
        } else if (!strcmp(op, "model")) {
            char m[32];
            if (!word(&p, m, sizeof m) || !object_index(m, names, nnames, &a) || a < 0 || (unsigned long)a >= *count) {
                log("line %d: model needs a resource index of this level or an imported model's name", lineNo);
                continue;
            }
            *(unsigned short *)(rec + REC_MODEL) = (unsigned short)a;
        } else if (!strcmp(op, "class")) {
            if (!number(&p, &a) || a < 0 || a > 0xFFFF) {
                log("line %d: class needs a class id", lineNo);
                continue;
            }
            *(unsigned short *)(rec + REC_CLASS) = (unsigned short)a;
        } else if (!strcmp(op, "prop")) {
            char name[64], *end;
            long off;
            if (!word(&p, name, sizeof name) || !number(&p, &a)) {
                log("line %d: prop needs a name or offset and a value", lineNo);
                continue;
            }
            off = strtol(name, &end, 0);
            if (*end) /* a name: look it up for this object's class */
                off = prop_offset(props, nprops, *(unsigned short *)(rec + REC_CLASS), name);
            if (off < 0 || (off & 3)) {
                log("line %d: class %u has no property %s", lineNo, *(unsigned short *)(rec + REC_CLASS), name);
                continue;
            }
            *(long *)(rec + REC_PROPS + off) = a;
        } else if (!strcmp(op, "remove")) {
            table[res] = (table[res] & 0x00FFFFFF) | ((unsigned)TYPE_IGNORED << 24);
        } else {
            log("line %d: unknown change '%s'", lineNo, op);
            continue;
        }
        applied++;
    }
    return applied;
}
