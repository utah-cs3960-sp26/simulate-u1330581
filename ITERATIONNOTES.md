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
