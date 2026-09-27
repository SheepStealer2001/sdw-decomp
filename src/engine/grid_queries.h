#ifndef SDW_GRID_QUERIES_H
#define SDW_GRID_QUERIES_H

#include "sdw_classes.h"

#include "scenaric.h"
s32 ObjGrid_QueryBoxPoints(const CollBox *query, ScnObject **out);
s32 ObjGrid_QueryBoxOverlap(CollBox *query, ScnObject **out);

#endif
