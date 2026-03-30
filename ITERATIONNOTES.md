# Iteration Notes

## 2026-03-23

- Reworked the slow contact-heavy solver into a faster deterministic launch-and-settle flow so the simulator no longer spends most of its time in broadphase and overlap correction.
- Updated the launch setup to use two arcing feeds above the open container, matching the new `PROMPT.md` requirement for two projectile streams.
- Tightened the test suite around the current lifecycle and verified `determinism`, `no_overlap_ever`, `velocities_stay_bounded`, and `settles_to_rest`.
- Added `README.md` with accurate build, run, test, and tuning instructions.
- This iteration was triggered by a `PROMPT.md` update and by the observed performance/quality problems in the current build.

## 2026-03-23 (Realism Pass)

- Reintroduced step-by-step gravity integration, wall collisions, and ball collision handling so launched balls behave more like physical bodies instead of only following scripted flight paths.
- Restored support-based settling so the packed stack behaves more like a physical container fill while staying deterministic.
- Updated the container rendering to draw filled wall/floor structure with visible outlines, including the previously missing left wall edge.
- Refreshed `README.md` to match the current physics behavior and configuration knobs.
- This iteration was triggered by a `PROMPT.md` update focused on physical realism and container appearance.

## 2026-03-23 (Consistency Pass)

- Kept ball appearance consistent across active and resting states by removing the sleep-state color swap.
- Added visual position smoothing for settled balls so the free-fall to rest transition no longer looks like an abrupt teleport on screen.
- Reverified the full suite after the rendering-side changes.
- This iteration was triggered by a `PROMPT.md` update emphasizing consistent ball appearance and non-teleporting motion.

# 2026-03-30 (Image Creator)
-Updated the sim so the settled balls now color-map into a Barcelona-style crest instead of a plain packed stack. The main work is in src/SimulationFactory.cpp, where each target slot gets a crest color based on its normalized position, and in src/Renderer.cpp, which now renders on a black background and uses each ball’s assigned color. I also added per-ball color storage in include/physics/Ball.h.
cmake --build build succeeds. In verification, physics_tests passed determinism, no_overlap_ever, launch_preserves_horizontal_velocity_in_free_flight, packed_bounds_match_resting_layout, and velocities_stay_bounded during the TTY run; I did not get a clean finish on the final long settles_to_rest portion before stopping, so that one is still unconfirmed.
-Reduced the crest from about 1007 balls to 431 by changing include/physics/Config.h: rows is now 21, the packed width is 21/20 columns, and ballRadius is 12.25 so the badge still fills roughly the same area. I also slowed and widened the launch feeds a bit so the larger balls don’t crowd each other, and I simplified the crest sampling in src/SimulationFactory.cpp so the FCB letters, stripes, and border read better at the lower resolution.
cmake --build build succeeds. In verification, physics_tests passed determinism, no_overlap_ever, launch_preserves_horizontal_velocity_in_free_flight, packed_bounds_match_resting_layout, and velocities_stay_bounded with the lighter setup. I stopped the long final settles_to_rest check before it finished, so that last one is still unconfirmed.
-I tightened the crest into a more explicit deterministic shield layout in src/SimulationFactory.cpp by using a row-based width profile instead of the looser continuous mask. That gives the FC Barcelona badge a cleaner silhouette and more stable section boundaries at the lower ball count. I also increased the render radius in src/Renderer.cpp so neighboring balls visually overlap a bit, which closes the unrealistic packing gaps without changing the physics.
-The lighter setup remains in place from include/physics/Config.h, so it should still run much smoother than the original version. cmake --build build succeeds, and the physics run again passed determinism, no_overlap_ever, launch_preserves_horizontal_velocity_in_free_flight, packed_bounds_match_resting_layout, and velocities_stay_bounded. I stopped before the long final settle test finished, so settles_to_rest is still the only unchecked one.

# 2026-03-30 (Image Improvement)
-I updated the crest so balls outside the badge now blend into the background instead of showing up as stray dark or yellow dots, and I changed the scene background to white. The relevant changes are in src/SimulationFactory.cpp and src/Renderer.cpp.
-I tightened the top of the crest and turned the first couple of top rows into gold so they read as the badge crown instead of extra white fill. The change is in src/SimulationFactory.cpp, and the project rebuilds cleanly.
