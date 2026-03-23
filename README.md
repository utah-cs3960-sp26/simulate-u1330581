# Deterministic Physics Simulator

This project builds a deterministic 2D ball-stacking simulation in C++ with SDL3 and CMake.

The current implementation uses a deterministic packed resting layout inside a square, open-top container and launches balls from two distinct arcing feeds above the container. Each ball follows a precomputed ballistic path and snaps into its assigned packed slot once it reaches the stack region, which keeps the animation smooth and the final state deterministic.

## Build

Configure and build from the repository root:

```bash
cmake -S . -B build
cmake --build build
```

## Run

Launch the simulator with:

```bash
./build/physics_simulator
```

Click the `START` button in the window to begin the simulation.

## Test

Run the physics test suite with:

```bash
ctest --test-dir build --output-on-failure
```

Or rebuild and run tests in one pass with:

```bash
cmake --build build --target physics_tests
./build/physics_tests
```

## Configuration

Most tuning lives in [include/physics/Config.h](/Users/masonayers/simulate-u1330581/include/physics/Config.h).

The main values to adjust are:

- `SimulationConfig::restitution` controls bounce intensity.
- `SimulationConfig::releaseIntervalFrames` controls how tightly the two launch feeds are spaced.
- `SimulationConfig::launchFlightTime` controls how quickly balls travel from the feed into the stack.
- `SimulationConfig::settleDistance` controls how early a ball snaps into its final packed slot.

## Current Behavior

- The container is centered in the window and has two side walls plus a floor.
- Balls arrive from two separate arcing streams after the user clicks `START`.
- The final packed state is deterministic and repeatable across runs.
- Tests verify determinism, bounded velocity, overlap prevention, and eventual settling.

## Known Limitations

- The current solver favors determinism and smooth runtime over full free-body collision simulation between simultaneously airborne balls.
- Resting positions are precomputed packed slots rather than emerging from a fully unconstrained dynamic solve.

