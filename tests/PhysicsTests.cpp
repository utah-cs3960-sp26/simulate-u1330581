#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "SimulationFactory.h"
#include "physics/PhysicsEngine.h"

namespace {
using TestCase = std::function<void()>;

void Require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string CaptureFrameSignature(const PhysicsEngine& engine) {
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(6);
    for (const Ball& ball : engine.GetSimulation().GetBalls()) {
        stream << ball.position.x << ',' << ball.position.y << ';';
    }
    return stream.str();
}

void AssertNoOverlapOrEscape(const PhysicsEngine& engine, int frame) {
    const auto& simulation = engine.GetSimulation();
    const auto& balls = simulation.GetBalls();
    const auto& bounds = simulation.GetBounds();
    constexpr double kContactTolerance = 0.35;

    for (const Ball& ball : balls) {
        if (!ball.active && !ball.sleeping) {
            continue;
        }
        Require(ball.position.x - ball.radius >= bounds.left - kContactTolerance,
            "Ball " + std::to_string(ball.id) + " escaped left wall at frame " + std::to_string(frame));
        Require(ball.position.x + ball.radius <= bounds.right + kContactTolerance,
            "Ball " + std::to_string(ball.id) + " escaped right wall at frame " + std::to_string(frame));
        Require(ball.position.y + ball.radius <= bounds.bottom + kContactTolerance,
            "Ball " + std::to_string(ball.id) + " escaped floor at frame " + std::to_string(frame));
    }

    for (std::size_t i = 0; i < balls.size(); ++i) {
        if (!balls[i].active && !balls[i].sleeping) {
            continue;
        }
        for (std::size_t j = i + 1; j < balls.size(); ++j) {
            if (!balls[j].active && !balls[j].sleeping) {
                continue;
            }
            const Vec2 delta = balls[j].position - balls[i].position;
            const double minDistance = balls[i].radius + balls[j].radius;
            const bool oppositeLaunchLanes = !balls[i].sleeping
                && !balls[j].sleeping
                && balls[i].launchFromLeft != balls[j].launchFromLeft;
            if (oppositeLaunchLanes) {
                const double laneHandoffY = std::min(balls[i].targetPosition.y, balls[j].targetPosition.y)
                    - (SimulationConfig::ballRadius * 4.0);
                if (balls[i].position.y < laneHandoffY && balls[j].position.y < laneHandoffY) {
                    continue;
                }
            }
            if (std::sqrt(LengthSquared(delta)) + kContactTolerance < minDistance) {
                std::ostringstream message;
                message << "Ball overlap detected at frame " << frame
                        << " between " << i << " and " << j
                        << " distance=" << std::sqrt(LengthSquared(delta))
                        << " a=(" << balls[i].position.x << "," << balls[i].position.y << ")"
                        << " targetA=(" << balls[i].targetPosition.x << "," << balls[i].targetPosition.y << ")"
                        << " launchA=(" << balls[i].launchVelocity.x << "," << balls[i].launchVelocity.y << ")"
                        << " slotA=(" << balls[i].slotRow << "," << balls[i].slotColumn << ")"
                        << " activeA=" << balls[i].active << " sleepA=" << balls[i].sleeping
                        << " b=(" << balls[j].position.x << "," << balls[j].position.y << ")"
                        << " targetB=(" << balls[j].targetPosition.x << "," << balls[j].targetPosition.y << ")"
                        << " launchB=(" << balls[j].launchVelocity.x << "," << balls[j].launchVelocity.y << ")"
                        << " slotB=(" << balls[j].slotRow << "," << balls[j].slotColumn << ")"
                        << " activeB=" << balls[j].active << " sleepB=" << balls[j].sleeping;
                throw std::runtime_error(message.str());
            }
        }
    }
}

void TestDeterminism() {
    PhysicsEngine first(SimulationFactory::CreateSimulation());
    PhysicsEngine second(SimulationFactory::CreateSimulation());

    for (int frame = 0; frame < 180; ++frame) {
        first.Step();
        second.Step();
        Require(CaptureFrameSignature(first) == CaptureFrameSignature(second), "Simulation lost determinism");
    }
}

void TestNoOverlapEver() {
    PhysicsEngine engine(SimulationFactory::CreateSimulation());
    for (int frame = 0; frame < 2400; ++frame) {
        engine.Step();
        AssertNoOverlapOrEscape(engine, frame);
    }
}

void TestVelocitiesStayBounded() {
    PhysicsEngine engine(SimulationFactory::CreateSimulation());
    const double maxSpeedSquared = SimulationConfig::maxSpeed * SimulationConfig::maxSpeed + 1e-4;
    for (int frame = 0; frame < 1600; ++frame) {
        engine.Step();
        for (const Ball& ball : engine.GetSimulation().GetBalls()) {
            if (!ball.active && !ball.sleeping) {
                continue;
            }
            Require(LengthSquared(ball.velocity) <= maxSpeedSquared, "Velocity explosion detected");
        }
    }
}

void TestLaunchPreservesHorizontalVelocityInFreeFlight() {
    PhysicsEngine engine(SimulationFactory::CreateSimulation());
    Ball initialBall = engine.GetSimulation().GetBalls().front();
    Require(initialBall.activationFrame == 0, "Expected the first slot to activate immediately");

    for (int frame = 0; frame < 6; ++frame) {
        engine.Step();
    }

    const Ball& ball = engine.GetSimulation().GetBalls().front();
    Require(ball.active, "Expected the first ball to be active during free flight");
    Require(std::abs(ball.velocity.x - ball.launchVelocity.x) <= 1e-6,
        "Horizontal launch speed changed before any collision");
}

void TestPackedBoundsMatchRestingLayout() {
    const Simulation simulation = SimulationFactory::CreateSimulation();
    const auto& balls = simulation.GetBalls();
    const auto& bounds = simulation.GetBounds();

    double leftMost = balls.front().targetPosition.x - balls.front().radius;
    double rightMost = balls.front().targetPosition.x + balls.front().radius;
    for (const Ball& ball : balls) {
        leftMost = std::min(leftMost, ball.targetPosition.x - ball.radius);
        rightMost = std::max(rightMost, ball.targetPosition.x + ball.radius);
    }

    Require(std::abs(bounds.left - leftMost) <= 1e-6, "Packed layout no longer aligns with the left wall");
    Require(std::abs(bounds.right - rightMost) <= 1e-6, "Packed layout no longer aligns with the right wall");
}

void TestSettlesToRest() {
    PhysicsEngine engine(SimulationFactory::CreateSimulation());
    for (int frame = 0; frame < 6000 && !engine.IsSettled(); ++frame) {
        engine.Step();
    }

    Require(engine.IsSettled(), "Simulation did not settle");
    AssertNoOverlapOrEscape(engine, static_cast<int>(engine.GetFrameCount()));
}

int RunAllTests() {
    const std::vector<std::pair<std::string, TestCase>> tests {
        {"determinism", TestDeterminism},
        {"no_overlap_ever", TestNoOverlapEver},
        {"launch_preserves_horizontal_velocity_in_free_flight", TestLaunchPreservesHorizontalVelocityInFreeFlight},
        {"packed_bounds_match_resting_layout", TestPackedBoundsMatchRestingLayout},
        {"velocities_stay_bounded", TestVelocitiesStayBounded},
        {"settles_to_rest", TestSettlesToRest}
    };

    for (const auto& [name, test] : tests) {
        test();
        std::cout << "[PASS] " << name << '\n';
    }
    return 0;
}
}

int main() {
    try {
        return RunAllTests();
    } catch (const std::exception& exception) {
        std::cerr << "[FAIL] " << exception.what() << '\n';
        return 1;
    }
}
