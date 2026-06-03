# SDL2 Boids

A high-performance boids simulation written in C using SDL2.

This project started as a simple implementation of Craig Reynolds' Boids algorithm and gradually evolved into an exploration of optimization techniques including spatial partitioning, custom memory management, cache-friendly data layouts, and multithreading.

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
Boids: 25000
FPS avg: ~60
Update avg: ~8 ms
Draw avg: ~2 ms
```

The simulation can maintain approximately 25,000 boids at 60 FPS on a modern desktop CPU without GPU acceleration.

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

A naive boids implementation requires:

```text
O(n²)
```

neighbor comparisons.

At 25,000 boids:

```text
625,000,000
```

potential pair checks.

This quickly becomes impractical.

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

Each worker receives a subset of boids to update:

```text
Worker 0: 0 - 6249
Worker 1: 6250 - 12499
Worker 2: 12500 - 18749
Worker 3: 18750 - 24999
```

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

Testing showed that cells smaller than the vision radius often performed better because fewer irrelevant boids were examined.

One of the best-performing configurations was:

```text
cellSize = visionRadius / 4
```

---

## SoA Was Not A Silver Bullet

Converting to Structure-of-Arrays produced much smaller gains than expected.

The simulation was already dominated by neighbor search cost rather than memory bandwidth.

---

## Thread Count

Increasing thread count improved performance only when enough boids were present.

Small simulations often ran faster with fewer threads due to synchronization overhead.

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

---

# Future Work

Potential future directions:

* Obstacle line-of-sight
* Dynamic work scheduling
* SIMD optimization
* GPU compute shader implementation
* Flow fields
* RTS-style crowd movement
* Spatial partitioning for obstacles
* Full data-oriented rewrite

---

# References

* Craig Reynolds, "Boids"
* Data-Oriented Design
* SDL2
* Uniform Grid Spatial Partitioning
