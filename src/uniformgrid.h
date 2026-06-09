// uniform_grid.h
#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H

#include <stddef.h>
#include "arena.h"
#include "boid.h"

/*
    A single grid cell.

    This does not own its own array.

    Instead, it stores a slice into UniformGrid.indices:
        indices[start] through indices[start + count - 1]

    writeIndex is temporary state used only while rebuilding the grid.
*/
typedef struct GridCell
{
    int start;      // offset into grid->indices
    int count;      // number of boids in cell
    int writeIndex; // temp cursor
} GridCell;


/*
    Uniform spatial grid.

    The world is divided into equally-sized rectangular cells.

    Example:
        worldWidth  = 1920
        worldHeight = 1080
        cellSize    = 64

    Produces roughly:
        30 cols
        17 rows
*/
typedef struct UniformGrid
{
    float worldWidth;
    float worldHeight;
    float cellSize;

    int cols;
    int rows;
    int cellCount;

    int maxBoids;

    GridCell* cells;

    int* indices;
    Arena* arena;
} UniformGrid;
/*
    Represents a rectangular range of cells
    that should be searched for neighbors.

    Typically generated from:
        boid position + perception radius
*/
typedef struct GridQuery
{
    int minCol;
    int maxCol;

    int minRow;
    int maxRow;
} GridQuery;



/*
===============================================================================
    LIFECYCLE
===============================================================================
*/

/*
    Initializes the grid.

    Allocates all grid cells.

    Parameters:
        worldWidth  - width of simulation world
        worldHeight - height of simulation world
        cellSize    - size of each square cell

    Returns:
        1 on success
        0 on failure
*/
int UniformGrid_Init(UniformGrid* grid, float worldWidth, float worldHeight, float cellSize, int maxBoids);

/*
    Frees all memory owned by the grid.

    Must be called before shutdown.
*/
void UniformGrid_Free(UniformGrid* grid);

/*
    Clears the grid for a new frame.

    IMPORTANT:
        This does NOT free memory.

    It simply resets all cell counts back to 0
    so boids can be reinserted next frame.
*/
bool UniformGrid_PrepareBuild(UniformGrid* grid, int boidCount);


/*
===============================================================================
    INSERTION
===============================================================================
*/

/*
    Rebuilds the grid from the current boid positions.

    This performs the full two-pass/prefix-sum build:
        1. Count boids per cell
        2. Compute each cell's start offset
        3. Fill grid->indices with boid indices

    Returns:
        1 on success
        0 on failure
*/
int UniformGrid_Build(UniformGrid* grid, const Boid* boids, int boidCount);



/*
===============================================================================
    POSITION / CELL HELPERS
===============================================================================
*/

/*
    Converts an x-coordinate into a grid column.

    Example:
        x = 130
        cellSize = 64

        result = 2
*/
int UniformGrid_GetColumn(
    const UniformGrid* grid,
    float x
);

/*
    Converts a y-coordinate into a grid row.
*/
int UniformGrid_GetRow(
    const UniformGrid* grid,
    float y
);

/*
    Converts (col,row) into a 1D array index.

    Typical formula:
        index = row * cols + col
*/
int UniformGrid_GetCellIndex(
    const UniformGrid* grid,
    int col,
    int row
);

/*
    Returns nonzero if the cell coordinates are valid.

    Useful for bounds checking.
*/
int UniformGrid_IsValidCell(
    const UniformGrid* grid,
    int col,
    int row
);


/*
===============================================================================
    QUERYING
===============================================================================
*/

/*
    Computes which cells should be searched
    for nearby neighbors.

    Example:
        If a boid has perception radius 50,
        this function determines which surrounding
        cells could possibly contain neighbors.

    NOTE:
        This does NOT perform exact distance checks.
        It only narrows the search space.
*/
GridQuery UniformGrid_GetQueryRange(
    const UniformGrid* grid,
    float x,
    float y,
    float radius
);


/*
===============================================================================
    CELL ACCESS
===============================================================================
*/

/*
    Returns a mutable pointer to a cell.

    Returns NULL if out of bounds.
*/
GridCell* UniformGrid_GetCell(
    UniformGrid* grid,
    int col,
    int row
);

/*
    Const version of UniformGrid_GetCell.
*/
const GridCell* UniformGrid_GetCellConst(
    const UniformGrid* grid,
    int col,
    int row
);


typedef struct
{
    UniformGrid* grid;
    Boid* boids;

    int* localCounts;      // numThreads * cellCount
    int* touchedCells;     // numThreads * maxTouchedPerThread
    int* touchedCounts;    // numThreads

    int cellCount;
    int maxTouchedPerThread;
} GridCountJobData;

typedef struct
{
    int* counts;       // numThreads * cellCount
    int* touched;      // numThreads * maxTouchedPerThread
    int* touchedCount; // numThreads
    int cellCount;
    int maxTouchedPerThread;
} GridThreadCounts;

void UniformGrid_RunGridCountJob(void* data, int start, int end, int threadIndex);

void UniformGrid_GridCountReduce(UniformGrid* grid, GridCountJobData* job, int numThreads);

void UniformGrid_PrefixSum(UniformGrid* grid);
void UniformGrid_RunBuildJob(void* data, int start, int end, int threadIndex);
#endif