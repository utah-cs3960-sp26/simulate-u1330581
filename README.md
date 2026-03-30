# Deterministic Physics Simulator

This project builds a deterministic 2D ball-stacking image creator simulation in C++ with SDL3 and CMake.

The current implementation launches balls from two deterministic arcing feeds above a square, open-top container. Balls move under constant gravity, collide against the container walls and floor, interact with the settled stack, and then lock into deterministic packed resting slots once they reach a supported resting position.

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
- `SimulationConfig::launchFlightTime` and the launch offsets control the arc shape and perceived free-fall.
- `SimulationConfig::solverIterations` and `SimulationConfig::substeps` control collision quality versus runtime cost.
- `SimulationConfig::settleDistance` controls how close a ball must get to its packed slot before it locks in.

## Current Behavior

- The container is centered in the window and has two side walls plus a floor.
- Balls arrive from two separate arcing streams after the user clicks `START`.
- Balls interact with the container as a collision object and make direct contact with the walls and floor.
- Balls keep a consistent appearance throughout the simulation; they no longer change color when they come to rest.
- Rest transitions are visually smoothed so settling into the packed stack does not appear as a hard onscreen pop.
- The final packed state is deterministic and repeatable across runs.
- Tests verify determinism, bounded velocity, overlap prevention, and eventual settling.
- Image created by colored balls

## Known Limitations

- Resting positions are still guided by precomputed packed slots so the final fill remains deterministic across runs.
- Airborne collisions are intentionally conservative so the simulation stays smooth with roughly 1000 balls on screen.
