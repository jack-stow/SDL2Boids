#include "uniformgrid.h"
//#include <assert.h>


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
int UniformGrid_Init(UniformGrid* grid, float worldWidth, float worldHeight, float cellSize, int maxBoids) {
    if (grid == NULL) return 0;
    if (worldWidth <= 0.0f) return 0;
    if (worldHeight <= 0.0f) return 0;
    if (cellSize <= 0.0f) return 0;
    if (maxBoids <= 0) return 0;

    grid->worldWidth = worldWidth;
    grid->worldHeight = worldHeight;
    grid->cellSize = cellSize;

    grid->cols = (int)(worldWidth / cellSize) + 1;
    grid->rows = (int)(worldHeight / cellSize) + 1;
    grid->cellCount = grid->cols * grid->rows;
    grid->maxBoids = maxBoids;

    grid->cells = malloc(sizeof(GridCell) * grid->cellCount);
    if (grid->cells == NULL) {
        return 0;
    }

    for (int i = 0; i < grid->cellCount; i++) {
        grid->cells[i].start = 0;
        grid->cells[i].count = 0;
        grid->cells[i].writeIndex = 0;
    }

    size_t arenaSize = sizeof(int) * maxBoids;

    grid->arena = ArenaAlloc(arenaSize);
    if (grid->arena == NULL) {
        free(grid->cells);
        grid->cells = NULL;
        return 0;
    }

    grid->indices = NULL;

    return 1;
}

/*
    Frees all memory owned by the grid.

    Must be called before shutdown.
*/
void UniformGrid_Free(UniformGrid* grid) {
    ArenaRelease(grid->arena);
    free(grid->cells);
    free(grid);
}

/*
    Clears the grid for a new frame.

    IMPORTANT:
        This does NOT free memory.

    It simply resets all cell counts back to 0
    so boids can be reinserted next frame.
*/
bool UniformGrid_PrepareBuild(UniformGrid* grid, int boidCount) {
    if (grid == NULL) return false;

    if (grid->arena != NULL) {
        ArenaClear(grid->arena);
    }

    grid->indices = NULL;

    if (grid->cells == NULL) return false;

    for (int i = 0; i < grid->cellCount; i++) {
        grid->cells[i].start = 0;
        grid->cells[i].count = 0;
        grid->cells[i].writeIndex = 0;
    }
    grid->indices = ArenaPush(grid->arena, sizeof(int) * boidCount);

    return grid->indices != NULL;
}

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
int UniformGrid_Build(UniformGrid* grid, const Boid* boids, int boidCount) {
    if (grid == NULL) return 0;
    if (boids == NULL) return 0;
    if (boidCount < 0) return 0;
    if (boidCount > grid->maxBoids) return 0;

    UniformGrid_PrepareBuild(grid, boidCount);

    grid->indices = ArenaPush(grid->arena, sizeof(int) * boidCount);
    if (grid->indices == NULL) return 0;

    // Pass 1: count boids per cell
    for (int i = 0; i < boidCount; i++) {
        int col = UniformGrid_GetColumn(grid, boids[i].x);
        int row = UniformGrid_GetRow(grid, boids[i].y);

        if (!UniformGrid_IsValidCell(grid, col, row)) {
            continue;
        }

        int cellIndex = UniformGrid_GetCellIndex(grid, col, row);
        grid->cells[cellIndex].count++;
    }

    // Prefix sum: assign each cell a slice in grid->indices
    int runningTotal = 0;

    for (int i = 0; i < grid->cellCount; i++) {
        grid->cells[i].start = runningTotal;
        grid->cells[i].writeIndex = 0;

        runningTotal += grid->cells[i].count;
    }

    // Pass 2: fill cell slices
    for (int i = 0; i < boidCount; i++) {
        int col = UniformGrid_GetColumn(grid, boids[i].x);
        int row = UniformGrid_GetRow(grid, boids[i].y);

        if (!UniformGrid_IsValidCell(grid, col, row)) {
            continue;
        }

        int cellIndex = UniformGrid_GetCellIndex(grid, col, row);
        GridCell* cell = &grid->cells[cellIndex];

        int writeOffset = cell->start + cell->writeIndex;

        grid->indices[writeOffset] = i;

        cell->writeIndex++;
    }

    return 1;
}

/*
    Converts an x-coordinate into a grid column.

    Example:
        x = 130
        cellSize = 64

        result = 2
*/
int UniformGrid_GetColumn(const UniformGrid* grid, float x)
{
    if (grid == NULL) {
        return -1;
    }

    return (int)(x / grid->cellSize);
}

/*
    Converts a y-coordinate into a grid row.
*/
int UniformGrid_GetRow(const UniformGrid* grid, float y)
{
    if (grid == NULL) {
        return -1;
    }

    return (int)(y / grid->cellSize);
}

/*
    Converts (col,row) into a 1D array index.

    Typical formula:
        index = row * cols + col
*/
int UniformGrid_GetCellIndex(const UniformGrid* grid, int col, int row)
{
    return row * grid->cols + col;
}


/*
    Returns nonzero if the cell coordinates are valid.

    Useful for bounds checking.
*/
int  UniformGrid_IsValidCell(const UniformGrid* grid, int col, int row) {
    return col >= 0 && col < grid->cols && row >= 0 && row < grid->rows;
}

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
) {
    GridQuery query = { 0 };

    if (grid == NULL) {
        return query;
    }

    query.minCol = UniformGrid_GetColumn(grid, x - radius);
    query.maxCol = UniformGrid_GetColumn(grid, x + radius);

    query.minRow = UniformGrid_GetRow(grid, y - radius);
    query.maxRow = UniformGrid_GetRow(grid, y + radius);

    if (query.minCol < 0) query.minCol = 0;
    if (query.minRow < 0) query.minRow = 0;

    if (query.maxCol >= grid->cols) query.maxCol = grid->cols - 1;
    if (query.maxRow >= grid->rows) query.maxRow = grid->rows - 1;

    return query;
}

/*
    Returns a mutable pointer to a cell.

    Returns NULL if out of bounds.
*/
GridCell* UniformGrid_GetCell(UniformGrid* grid, int col, int row)
{
    if (grid == NULL) return NULL;

    if (!UniformGrid_IsValidCell(grid, col, row)) {
        return NULL;
    }

    int index = UniformGrid_GetCellIndex(grid, col, row);
    return &grid->cells[index];
}

/*
    Const version of UniformGrid_GetCell.
*/
const GridCell* UniformGrid_GetCellConst(const UniformGrid* grid, int col, int row)
{
    if (grid == NULL) return NULL;

    if (!UniformGrid_IsValidCell(grid, col, row)) {
        return NULL;
    }

    int index = UniformGrid_GetCellIndex(grid, col, row);
    return &grid->cells[index];
}



void UniformGrid_RunGridCountJob(void* data, int start, int end, int threadIndex)
{
    GridCountJobData* job = (GridCountJobData*)data;

    UniformGrid* grid = job->grid;
    Boid* boids = job->boids;

    int partitionIndex = start / job->partitionSize;

    int* myCounts = &job->localCounts[partitionIndex * job->cellCount];
    int* myTouched = &job->touchedCells[partitionIndex * job->maxTouchedPerThread];
    int touchedCount = job->touchedCounts[partitionIndex];

    real cellSize = grid->cellSize;
    int cols = grid->cols;
    int rows = grid->rows;

    for (int i = start; i < end; i++)
    {
        int col = (int)(boids[i].x / cellSize);
        int row = (int)(boids[i].y / cellSize);

        if ((unsigned)col >= (unsigned)cols ||
            (unsigned)row >= (unsigned)rows)
        {
            continue;
        }

        int cellIndex = row * cols + col;

        if (myCounts[cellIndex] == 0)
        {
           /* if (touchedCount >= job->maxTouchedPerThread)
            {
                printf(
                    "Touched overflow: thread=%d touched=%d max=%d\n",
                    threadIndex,
                    touchedCount,
                    job->maxTouchedPerThread
                );
                __debugbreak();
            }*/

            myTouched[touchedCount++] = cellIndex;
        }

        myCounts[cellIndex]++;
    }

    job->touchedCounts[partitionIndex] = touchedCount;
}

void UniformGrid_GridCountReduce(UniformGrid* grid, GridCountJobData* job, int numThreads)
{
    GridCell* cells = grid->cells;

    for (int t = 0; t < numThreads; t++)
    {
        int* counts = &job->localCounts[t * job->cellCount];
        int* touched = &job->touchedCells[t * job->maxTouchedPerThread];
        int touchedCount = job->touchedCounts[t];

        for (int i = 0; i < touchedCount; i++)
        {
            int cell = touched[i];
            cells[cell].count += counts[cell];
        }

        // DO NOT clear touchedCounts here
    }
}

void UniformGrid_ComputeWriteOffsets(UniformGrid* grid, GridCountJobData* job, int numThreads)
{
    //long long totalTouched = 0;
    for (int t = 0; t < numThreads; t++)
    {
        int* touched = &job->touchedCells[t * job->maxTouchedPerThread];
        int touchedCount = job->touchedCounts[t];
        //totalTouched += job->touchedCounts[t];
        for (int i = 0; i < touchedCount; i++)
        {
            int cell = touched[i];
            int offset = grid->cells[cell].start;

            for (int prev = 0; prev < t; prev++)
            {
                offset += job->localCounts[prev * job->cellCount + cell];
            }

            job->writeOffsets[t * job->cellCount + cell] = offset;
        }
    }
    //printf("totalTouched=%lld\n", totalTouched);
}


void UniformGrid_PrefixSum(UniformGrid* grid)
{
    int runningTotal = 0;

    for (int i = 0; i < grid->cellCount; i++)
    {
        GridCell* cell = &grid->cells[i];

        cell->start = runningTotal;
        runningTotal += cell->count;

        if (cell->count > 0)
        {
            cell->writeIndex = 0;
        }
    }
}



void UniformGrid_RunBuildJob(void* data, int start, int end, int threadIndex)
{
    GridCountJobData* job = (GridCountJobData*)data;

    UniformGrid* grid = job->grid;
    Boid* boids = job->boids;

    int partitionIndex = start / job->partitionSize;

    int* myOffsets =
        &job->writeOffsets[partitionIndex * job->cellCount];

    real cellSize = grid->cellSize;
    int cols = grid->cols;
    int rows = grid->rows;

    for (int i = start; i < end; i++)
    {
        int col = (int)(boids[i].x / cellSize);
        int row = (int)(boids[i].y / cellSize);

        if ((unsigned)col >= (unsigned)cols ||
            (unsigned)row >= (unsigned)rows)
        {
            continue;
        }

        int cellIndex = row * cols + col;

        int writeOffset = myOffsets[cellIndex]++;
        //assert(writeOffset >= 0 && writeOffset < job->grid->maxBoids);
        grid->indices[writeOffset] = i;
    }
}

void UniformGrid_ClearLocalCounts(GridCountJobData* job, int numThreads)
{
    for (int t = 0; t < numThreads; t++)
    {
        int* counts = &job->localCounts[t * job->cellCount];
        int* touched = &job->touchedCells[t * job->maxTouchedPerThread];
        int touchedCount = job->touchedCounts[t];

        for (int i = 0; i < touchedCount; i++)
        {
            counts[touched[i]] = 0;
        }

        job->touchedCounts[t] = 0;
    }
}


void UniformGrid_Count(UniformGrid* grid, const Boid* boids, int boidCount)
{
    if (grid == NULL) return;
    if (boids == NULL) return;
    if (grid->cells == NULL) return;
    if (boidCount < 0) return;
    if (boidCount > grid->maxBoids) return;

    for (int i = 0; i < boidCount; i++)
    {
        int col = UniformGrid_GetColumn(grid, boids[i].x);
        int row = UniformGrid_GetRow(grid, boids[i].y);

        if (!UniformGrid_IsValidCell(grid, col, row))
        {
            continue;
        }

        int cellIndex = UniformGrid_GetCellIndex(grid, col, row);
        grid->cells[cellIndex].count++;
    }
}

