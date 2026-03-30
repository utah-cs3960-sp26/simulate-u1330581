#pragma once

#include <cstddef>

struct SimulationConfig {
    static constexpr int windowWidth = 1280;
    static constexpr int windowHeight = 960;
    static constexpr double ballRadius = 12.25;
    static constexpr double gravity = 1680.0;
    static constexpr double restitution = 0.08;
    static constexpr double maxSpeed = 2600.0;
    static constexpr double fixedTimeStep = 1.0 / 120.0;
    static constexpr int solverIterations = 10;
    static constexpr int substeps = 3;
    static constexpr std::size_t rows = 21;
    static constexpr std::size_t evenRowColumns = 21;
    static constexpr std::size_t oddRowColumns = 20;
    static constexpr double containerInnerSize = 560.0;
    static constexpr double sleepSpeed = 220.0;
    static constexpr int sleepFrames = 36;
    static constexpr double supportTolerance = 0.45;
    static constexpr double linearDamping = 0.998;
    static constexpr std::size_t releaseIntervalFrames = 10;
    static constexpr double launchFlightTime = 0.48;
    static constexpr double launchSpawnOffsetX = 135.0;
    static constexpr double launchSpawnOffsetY = 170.0;
    static constexpr double settleDistance = 220.0;
    static constexpr double wallThickness = 18.0;
};
