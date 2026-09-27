#ifndef SDW_OBJECTS_LIGHTSPOT_H
#define SDW_OBJECTS_LIGHTSPOT_H

/* The functions and globals lightspot.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct IDirect3DVertexBuffer7;
class RenderPoly;
class ScnObject;

extern RenderPoly g_lightSpotBeamFace;           /* 0x6cfadc the one triangle every beam face is pushed through */
extern u16 g_lightSpotSamBoxCount;               /* 0x6cfb00 */
extern Box **g_lightSpotSamBoxes;                /* 0x6cfafc id list 0x46: where Sam can be told about the robot */
extern IDirect3DVertexBuffer7 *g_lightSpotVbXf;  /* 0x6cfb4c D3DFVF_XYZRHW: ProcessVertices' destination */
extern IDirect3DVertexBuffer7 *g_lightSpotXyzVb; /* 0x6cfb48 D3DFVF_XYZ, 41 vertices: the cone in object space */
ScnObject *LightSpot_Create(void *record);

#endif
