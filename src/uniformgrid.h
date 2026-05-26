// uniform_grid.h
#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H

#include <stddef.h>

typedef struct GridCell
{
    int* indices;      // boid indices stored in this cell
    int count;         // current number of boids in cell
    int capacity;      // allocated capacity
} GridCell;

typedef struct UniformGrid
{
    float worldWidth;
    float worldHeight;
    float cellSize;

    int cols;
    int rows;
    int cellCount;

    GridCell* cells;
} UniformGrid;

typedef struct GridQuery
{
    int minCol;
    int maxCol;
    int minRow;
    int maxRow;
} GridQuery;

// lifecycle
int  UniformGrid_Init(UniformGrid* grid, float worldWidth, float worldHeight, float cellSize);
void UniformGrid_Free(UniformGrid* grid);
void UniformGrid_Clear(UniformGrid* grid);

// insertion
int  UniformGrid_Insert(UniformGrid* grid, int boidIndex, float x, float y);

// coordinate helpers
int  UniformGrid_GetColumn(const UniformGrid* grid, float x);
int  UniformGrid_GetRow(const UniformGrid* grid, float y);
int  UniformGrid_GetCellIndex(const UniformGrid* grid, int col, int row);
int  UniformGrid_IsValidCell(const UniformGrid* grid, int col, int row);

// query helpers
GridQuery UniformGrid_GetQueryRange(const UniformGrid* grid, float x, float y, float radius);

// cell access
GridCell*       UniformGrid_GetCell(UniformGrid* grid, int col, int row);
const GridCell* UniformGrid_GetCellConst(const UniformGrid* grid, int col, int row);

#endif