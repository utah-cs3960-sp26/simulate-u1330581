#pragma once

#include <cstddef>

struct SimulationConfig {
    static constexpr int windowWidth = 1280;
    static constexpr int windowHeight = 960;
    static constexpr double ballRadius = 8.3;
    static constexpr double gravity = 1450.0;
    static constexpr double restitution = 0.06;
    static constexpr double maxSpeed = 4200.0;
    static constexpr double fixedTimeStep = 1.0 / 120.0;
    static constexpr int solverIterations = 1;
    static constexpr int substeps = 1;
    static constexpr std::size_t rows = 33;
    static constexpr std::size_t evenRowColumns = 31;
    static constexpr std::size_t oddRowColumns = 30;
    static constexpr double containerInnerSize = 560.0;
    static constexpr double sleepSpeed = 18.0;
    static constexpr int sleepFrames = 24;
    static constexpr double supportTolerance = 0.3;
    static constexpr double linearDamping = 1.0;
    static constexpr std::size_t releaseIntervalFrames = 8;
    static constexpr double launchFlightTime = 0.05;
    static constexpr double launchSpawnOffsetX = 90.0;
    static constexpr double launchSpawnOffsetY = 40.0;
    static constexpr double settleDistance = 26.0;
};
