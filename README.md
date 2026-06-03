# SDL2 Boids

A high-performance boids simulation written in C using SDL2.

This project started as a simple implementation of Craig Reynolds' Boids algorithm and gradually evolved into an exploration of optimization techniques including spatial partitioning, memory arenas, cache-friendly data layouts, and multithreading.

![Boids simulation demo](boids.gif)

![Boids obstacle demo](obstacles.gif)

---

# Features

- Classic Boids flocking behavior
  - Separation
  - Alignment
  - Cohesion
- Points of Interest (POIs)
- Obstacle avoidance
- Obstacle drawing and erasing with the mouse
- Uniform grid spatial partitioning
- Persistent worker thread pool
- Performance benchmarking
- Configurable simulation parameters
- Arena allocator experiments
- Optional Structure-of-Arrays implementation

---

# Controls

| Input | Action |
|---------|---------|
| Left Mouse Drag | Draw obstacle |
| Right Mouse Drag | Erase obstacles intersecting drag line |
| Mouse Wheel | Adjust POI strength (if enabled) |

---

# Performance

Test system:

- CPU: Ryzen 9 5950X
- Build: Release
- Precision: float

Example benchmark:

```text
Boids: 25000
FPS avg: ~60
Update avg: ~8ms
Draw avg: ~2ms
```

The simulation can maintain roughly 25,000 boids at 60 FPS on a modern desktop CPU without using GPU compute shaders.

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

- Border avoidance
- POI attraction
- Obstacle avoidance

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

---
## Uniform Grid Querying

Neighbor lookup uses a broad-phase / narrow-phase approach.

For each boid, the grid computes a rectangular range of cells that could contain neighbors within the boid’s vision radius:

```text
minCol = floor((x - visionRadius) / cellSize)
maxCol = floor((x + visionRadius) / cellSize)

minRow = floor((y - visionRadius) / cellSize)
maxRow = floor((y + visionRadius) / cellSize)

### Benefits

- Avoids linked lists entirely
- Requires no per-cell allocations
- Produces cache-friendly memory access patterns
- Stores all cell contents in a single contiguous array
- Rebuilds efficiently every frame
- Similar to techniques used in counting sort, radix sort, and GPU spatial partitioning systems

---

## Neighbor Search

For each boid:

1. Determine its grid cell
2. Search nearby cells
3. Perform exact distance tests
4. Apply flocking behavior

This reduces the effective complexity from:

```text
O(n²)
```

to approximately:

```text
O(n)
```

for typical distributions.

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

- Avoids thread creation overhead
- Scales across many cores
- Reduces synchronization costs

---

## Work Distribution

Boids are divided into chunks.

Each worker receives a range:

```text
[startIndex, endIndex)
```

and updates only that range.

Worker results are synchronized using:

- SDL mutexes
- SDL condition variables

---

# Obstacles

Obstacles are represented as line segments.

```c
x1, y1
x2, y2
```

Boids avoid obstacles by:

1. Finding the closest point on the segment
2. Measuring distance
3. Applying a repulsion force

No collision physics are currently used.

This provides convincing obstacle avoidance while remaining inexpensive.

---

# Memory Management

The uniform grid uses a custom arena allocator that was originally written for a separate C project.

In this simulation, the arena is used to provide temporary per-frame storage for the grid’s contiguous boid index array.

Each frame:

1. The arena is reset.
2. A new index buffer is pushed from the arena.
3. The uniform grid rebuilds its cell ranges into that buffer.

This avoids repeated heap allocation during the simulation loop and gives the grid a simple frame-based memory lifetime.
---

# Structure of Arrays Experiment

The project contains an experimental SoA implementation.

Goal:

```text
AoS:
[Boid][Boid][Boid]

SoA:
[x][y][vx][vy]
```

In this project, the performance improvement was smaller than expected because the original Boid structure was already fairly cache-friendly.

This was still a useful exercise for understanding data-oriented design.

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

- SDL2
- SDL2_image
- CMake
- Visual Studio 2022 (Windows)

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

- Obstacle line-of-sight
- Dynamic work scheduling
- SIMD optimization
- GPU compute shader implementation
- Flow fields
- RTS-style crowd movement
- Spatial partitioning for obstacles
- Full data-oriented rewrite

---

# References

- Craig Reynolds, "Boids"
- Data-Oriented Design
- SDL2
- Uniform Grid Spatial Partitioning