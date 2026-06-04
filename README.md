# SDL2 Boids

A high-performance boids simulation written in C using SDL2.

This project began as a way to learn C and experiment with flocking behavior, but gradually evolved into an exploration of data-oriented design and performance optimization. Over time it grew to include uniform-grid spatial partitioning, arena allocation, multithreading, dynamic work scheduling, rendering optimizations, obstacle avoidance, and extensive benchmarking while scaling from a few hundred boids to hundreds of thousands of simulated agents.

![Boids simulation demo](boids.gif)

![Boids obstacle demo](obstacles.gif)

---

# Features

* Classic Boids flocking behavior

  * Separation
  * Alignment
  * Cohesion
* Points of Interest (POIs)
* Obstacle avoidance
* Interactive obstacle drawing and erasing
* Uniform grid spatial partitioning
* Persistent worker thread pool
* Performance benchmarking
* Configurable simulation parameters
* Arena-backed grid storage
* Experimental Structure-of-Arrays implementation

---

# Controls

| Input            | Action                       |
| ---------------- | ---------------------------- |
| Left Mouse Drag  | Draw obstacle                |
| Right Mouse Drag | Erase intersecting obstacles |

---

# Performance

Test system:

* CPU: AMD Ryzen 9 5950X
* Build: Release
* Precision: float

Example benchmark:

```text
Boids: 300,000
FPS avg: ~60
Grid Build avg: ~3 ms
Update avg: ~8.6 ms
Draw avg: ~1.4 ms
```

The simulation can maintain approximately 300,000 boids at 60 FPS* on a modern desktop CPU without GPU acceleration.

*assuming the boids don't all cluster together in one area, which would increase neighbor search costs

---

# Implementation

## Boids Algorithm

Each boid computes three steering forces:

### Separation

Steer away from nearby neighbors.

```text
Force ∝ inverse distance
```

### Alignment

Match velocity with nearby neighbors.

```text
Average neighbor velocity
```

### Cohesion

Move toward the center of nearby neighbors.

```text
Average neighbor position
```

These forces are combined with:

* Border avoidance
* POI attraction
* Obstacle avoidance

to produce the final steering force.

---

# Spatial Partitioning

A naive boids implementation is:

```text
O(n²)
```

At 300,000 boids, a naive implementation would require:

90,000,000,000

potential pair comparisons per frame.

At 60 FPS:

5,400,000,000,000

potential comparisons per second.

Even under an unrealistically optimistic model:

- 16 CPU cores
- 5 GHz clock speed
- 6 instructions retired per cycle
- One boid comparison completed in a single instruction
- Perfect cache behavior
- No memory stalls

The theoretical maximum throughput would be:

16 × 5,000,000,000 × 6
≈ 480,000,000,000 instructions/second

This is still more than an order of magnitude below the work required by a naive O(n²) implementation. 
In practice, a boid comparison requires many instructions and multiple memory accesses, making the gap even larger. 
The purpose of the uniform grid was therefore not to make the simulation faster, it was to make it computationally feasible at all.

To reduce the search space, the simulation uses a Uniform Grid.

---

## Uniform Grid Construction

Each frame, the uniform grid is rebuilt using a two-pass counting-sort style algorithm with an intermediate prefix-sum step.

### Pass 1: Count Boids Per Cell

The simulation first determines how many boids belong to each grid cell.

```text
cellCounts[cell]++
```

This produces a histogram of boid occupancy and determines how much storage each cell requires.

### Prefix Sum

The cell counts are converted into offsets within a single contiguous index array.

```text
offset[i + 1] = offset[i] + count[i]
```

After this step, every cell owns a unique slice of the array:

```text
Cell 0 -> indices[0..14]
Cell 1 -> indices[15..27]
Cell 2 -> indices[28..41]
...
```

### Pass 2: Populate Cell Ranges

The boids are traversed a second time and their indices are written into their cell's assigned range.

```text
indices[cell.start + writeIndex] = boidIndex
```

The result is a compact structure where all boids belonging to a cell are stored contiguously in memory.

### Benefits

* Avoids linked lists entirely
* Requires no per-cell allocations
* Produces cache-friendly memory access patterns
* Stores all cell contents in a single contiguous array
* Rebuilds efficiently every frame
* Similar to techniques used in counting sort, radix sort, and GPU spatial partitioning systems

---

## Uniform Grid Querying

Neighbor lookup uses a broad-phase / narrow-phase approach.

For each boid, the grid computes a rectangular range of cells that could contain neighbors within the boid's vision radius:

```text
minCol = floor((x - visionRadius) / cellSize)
maxCol = floor((x + visionRadius) / cellSize)

minRow = floor((y - visionRadius) / cellSize)
maxRow = floor((y + visionRadius) / cellSize)
```

The query range is clamped to the grid bounds and every cell in the resulting rectangle is examined.

This is the broad phase. It dramatically reduces the number of boids that need to be considered without performing any expensive distance calculations.

Because the query shape is rectangular while a boid's vision radius is circular, some candidates may still be outside the true vision range. These are rejected using an exact squared-distance check:

```text
distanceSq <= visionRadiusSq
```

The grid therefore returns candidate boids rather than visible boids.

The flocking system performs the final distance test before applying separation, alignment, or cohesion.

The number of queried cells depends on the ratio between vision radius and cell size.

Examples:

```text
cellSize = visionRadius      -> usually 3x3 = 9 cells
cellSize = visionRadius / 4  -> usually 9x9 = 81 cells
```

Smaller cells result in more cells being queried but fewer boids per cell. In testing, smaller cells frequently performed better because reducing candidate boids mattered more than reducing the number of cells visited.

---

# Multithreading

The simulation uses a persistent worker thread pool.

Threads are created once during initialization and reused every frame.

Workflow:

```text
Main Thread
    |
    v
Build Uniform Grid
    |
    v
Wake Worker Threads
    |
    v
Update Boids
    |
    v
Wait For Completion
    |
    v
Render
```

Benefits:

* Avoids thread creation overhead
* Scales across many CPU cores
* Reduces synchronization costs

---

## Work Distribution

Boids are divided into contiguous chunks of the boid array.

Workers pull chunks from a shared work queue.

```text
Worker 0: grabs next chunk
Worker 1: grabs next chunk
Worker 2: grabs next chunk
...
```
When a worker finishes a chunk, it immediately claims another until all boids have been processed.

This improved load balancing and reduced the impact of uneven boid distributions.

Worker results are synchronized using SDL mutexes and condition variables.

---

# Obstacles

Obstacles are represented as line segments:

```text
x1, y1
x2, y2
```

Boids avoid obstacles by:

1. Finding the closest point on the line segment
2. Measuring the distance to that point
3. Applying a repulsion force when within a configurable avoidance radius

No collision physics are currently used.

This approach provides convincing obstacle avoidance while remaining inexpensive enough to evaluate for every boid.

---

# Memory Management

The uniform grid uses a custom arena allocator that was originally written for a separate C project.

In this simulation, the arena provides temporary per-frame storage for the grid's contiguous boid index array.

Each frame:

1. The arena is reset
2. A new index buffer is allocated from the arena
3. The uniform grid rebuilds its cell ranges into that buffer

This avoids repeated heap allocation during the simulation loop and gives the grid a simple frame-based memory lifetime.

---

# Structure of Arrays Experiment

The project contains an experimental Structure-of-Arrays implementation.

Goal:

```text
AoS:
[Boid][Boid][Boid]

SoA:
[x][y][vx][vy]
```

The expectation was that separating frequently accessed data into contiguous arrays would improve cache utilization and simulation performance.

In practice, the performance improvement was smaller than expected because the original Boid structure was already fairly cache-friendly and the simulation was dominated by neighbor-search costs rather than memory bandwidth.

The experiment was still valuable for understanding data-oriented design and evaluating where the actual bottlenecks existed.

---

# Interesting Findings

## Uniform Grid

The uniform grid produced the largest performance improvement in the entire project.

This optimization alone enabled simulations that would otherwise be impractical.

---

## Cell Size Matters

Grid size has a significant impact on performance.

Testing showed that no single cell size is universally optimal.

Smaller cells reduce the number of boids examined during neighbor searches but increase:

- Grid construction cost
- Number of queried cells
- Memory traffic

At large vision radii, smaller cells often performed best.

At high boid counts with small vision radii, larger cells became more efficient because grid construction overhead became a significant portion of frame time.

The optimal cell size depends on both boid density and query radius.

---

## SoA Was Not A Silver Bullet

Converting to Structure-of-Arrays produced much smaller gains than expected.

The simulation was already dominated by neighbor search cost rather than memory bandwidth.

---

## Thread Count

Increasing thread count improved performance only when enough boids were present.

Small simulations often ran faster with fewer threads due to synchronization overhead.

---

## Rendering Optimizations

Initially, each boid was rendered using SDL_RenderCopyEx with a rotated sprite.

As boid counts increased, rendering became a significant bottleneck.

Several rendering approaches were tested:

| Method | Result |
|----------|----------|
| Rotated sprite | Slowest |
| Unrotated sprite | ~1 ms improvement |
| Filled rectangles | Small improvement |
| SDL_RenderDrawPoint | Large improvement |
| SDL_RenderDrawPoints (batched) | Fastest |

The final implementation batches all visible boids into a single SDL_Point array and renders them using SDL_RenderDrawPoints.

At 150,000 boids this reduced average draw time from several milliseconds to well under 1 millisecond.

---

## Stale Grid Experiments

The simulation was modified to rebuild the uniform grid less frequently.

Configurations tested:

- Every frame
- Every other frame
- Every fourth frame

Reducing rebuild frequency substantially reduced grid construction cost.

However, stale spatial data eventually produced visible artifacts.

Updating every other frame appeared mostly acceptable, while updating every fourth frame introduced noticeable visual stuttering despite significantly better benchmark numbers.

This demonstrated that benchmark improvements do not always translate into better perceived simulation quality.

---

## Batching Matters More Than Expected

Switching from individual SDL draw calls to SDL_RenderDrawPoints dramatically reduced rendering cost.

At high boid counts, rendering became one of the largest bottlenecks in the simulation.

Batching transformed rendering from a major cost into a relatively minor one.

---

## Dynamic Work Scheduling

Dynamic chunk scheduling outperformed fixed worker assignments.

Smaller chunk sizes generally produced better average frame times because they improved load balancing between worker threads.

Larger chunk sizes sometimes produced more consistent worst-case timings, suggesting a tradeoff between throughput and frame-time stability.

---

# Building

Requirements:

* SDL2
* SDL2_image
* CMake
* Visual Studio 2022
* vcpkg

Configure:

```bash
cmake --preset vs2022-x64-vcpkg
```

Build:

```bash
cmake --build build --config Release
```

Run:

```bash
Release/Boids.exe
```
