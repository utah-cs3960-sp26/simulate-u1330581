#pragma once

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

#include "physics/Config.h"
#include "physics/Simulation.h"

class PhysicsEngine {
public:
    explicit PhysicsEngine(Simulation simulation);

    /** Advances the simulation by one fixed frame. */
    void Step();

    /** Returns the current simulation state. */
    Simulation& GetSimulation();

    /** Returns the current simulation state. */
    const Simulation& GetSimulation() const;

    /** Reports whether every ball is sleeping. */
    bool IsSettled() const;

    /** Returns how many fixed frames have elapsed. */
    std::uint64_t GetFrameCount() const;

    /** Smooths visual positions so settle transitions do not pop on screen. */
    void UpdateVisualState();

private:
    using CellKey = std::pair<int, int>;

    struct CellKeyHasher {
        std::size_t operator()(const CellKey& key) const noexcept;
    };

    /** Applies gravity and integrates all active balls for one substep. */
    void Integrate(double dt);

    /** Rebuilds the uniform grid for the current ball positions. */
    void RebuildSpatialGrid();

    /** Resolves ball-ball collisions using the spatial grid. */
    void ResolveBallCollisions();

    /** Resolves wall collisions for every ball. */
    void ResolveWallCollisions();

    /** Settles balls into their deterministic resting slots when ready. */
    void UpdateSettledState();

    /** Returns true when the ball can safely lock into its target slot. */
    bool CanSettle(const Ball& ball) const;

    /** Returns true when the target slot has stable support beneath it. */
    bool HasSupport(const Ball& ball) const;

    /** Returns the ball assigned to a packed slot, or nullptr when missing. */
    const Ball* FindBallAtSlot(std::size_t row, std::size_t column) const;

    /** Returns mutable access to a packed slot owner, or nullptr when missing. */
    Ball* FindBallAtSlot(std::size_t row, std::size_t column);

    /** Returns the number of columns in a packed row. */
    static std::size_t ColumnsForRow(std::size_t row);

    /** Resolves a collision between two balls if needed. */
    void ResolveBallPair(Ball& a, Ball& b);

    /** Resolves a collision between a ball and a wall. */
    void ResolveWall(Ball& ball, const Wall& wall);

    /** Clamps velocity to keep the simulation bounded. */
    void ClampVelocity(Ball& ball) const;

    Simulation simulation_;
    std::unordered_map<CellKey, std::vector<std::size_t>, CellKeyHasher> spatialGrid_;
    std::uint64_t frameCount_ {0};
};
