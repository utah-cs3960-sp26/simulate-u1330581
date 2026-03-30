You are being prompted in a loop by an automated system. Each time you are invoked, you should read this file in full before doing anything else.

**On the first iteration only:** Set up the project from scratch — install dependencies, scaffold the codebase, and implement the initial physics simulator as described below. If this step has already been completed, continue.

**On every subsequent iteration:** Do NOT recreate or rewrite the simulator. Instead, review this file, cross-reference it with the current state of the codebase and `README.md`, identify what is missing or broken, and make only the changes necessary to bring the project into alignment with these instructions. Check `ITERATIONNOTES.md` and `README.md` to understand what has already been done.

## Project Goal

Implement a deterministic 2D physics simulator in C++ using SDL3. The simulation features ~1000 circular balls free falling into the top of a ceiling-less square container (floor + two walls) centered in the window. Balls are affected by gravity and collide with each other and the walls using non-elastic collisions with configurable restitution. All balls eventually come to rest, stacking naturally inside the container. The container should act as a physical container, so balls should be bound exactly by the container- balls shouldn't exceed the walls of the container, but some should be in direct contact with the walls and/or floor of the container. The balls shouldn't fall in a single horizontal line, but rather two arched projectiles in single file each. Because the focus of this simulation is physics, balls should be affected by a constant gravity. They should appear to be in free-fall (not falling too fast or too slow).

## Functional Requirements

- **Container:** A square container collision object with a floor and two side walls (no ceiling), centered in the simulation window. The container must be large enough to hold ~1000 balls without overflow, accounting for the natural gaps between packed circles. Balls should fill the container, which means balls at rest should be situated against the walls and floor. 
- **Balls:** Exactly enough balls to fill the container when settled (~1000). Balls must never overlap each other or breach the walls/floor at any point during the simulation — not even for a single frame. They should interact with the container, treating the container as a collision object. Balls should also be collision objects to prevent them from overlapping one another as well as the container. Balls should be consistent: they don't change color or size. They shouldn't "teleport" from one place to another, including from free-fall to at rest. Each ball is an object acted upon by gravity. It is the main focus of physics in the simulator. Following physics, it should fall until it is acted upon by another force, which, in this case, is going to be the container, other balls, or both. Balls are non-elastic, however collisions with other balls and the container should still be realistic: balls should still bounce, even if it is minor.
- **Physics:**
  - Constant gravity applied to all balls each frame.
  - Non-elastic collisions with restitution (coefficient should be configurable via a constant or config value).
  - Lower restitution = faster settling; but the final settled state (space occupied) should be the same regardless of restitution value.
  - Collision resolution must prevent overlap entirely — no tunneling, no phasing through walls.
  - Watch for and prevent "velocity explosion" — balls should never vibrate faster and faster until they shoot off to infinity.
- **Determinism:** The simulation must be fully deterministic. Every run must produce the exact same result. Ball starting positions should be calculated directly (not randomly), since their final positions are physically predictable.
- **Start condition:** A button click triggers the simulation to begin. Before the button is clicked, the scene is initialized but not running.
- **Performance:** The simulation should run smoothly and be pleasant to watch — aim for a stable, reasonable frame rate.

## Technical Requirements

- **Language/Framework:** C++ with SDL3.
- **SDL3 Installation:** If SDL3 is not installed, use `apt`, `brew`, or the appropriate package manager for the current system, or ask ChatGPT/Amp for the correct install steps for this environment.
- **Build system:** Use CMake.
- **Code design principles:**
  - **Separation of concerns:** Walls, balls, physics engine, and renderer should be distinct components. Do not conflate responsibilities.
  - **OOP:** Apply all four pillars (encapsulation, abstraction, inheritance, polymorphism) where appropriate. For example, walls and balls might share a common `CollisionObject` base class.
  - **Efficiency:** Use spatial partitioning (e.g., a grid or spatial hash) to avoid O(n²) collision checks across all 1000 balls every frame.

## Image Generation
To add to the deterministic behavior of the physics simulator, the balls should end up creating the Barcelona_Logo.png image in their final location.

## Testing
- Aim for ~100% code coverage on physics logic (collision detection, resolution, gravity application).
- Use a test-first approach where practical.
- Write tests that verify:
  - No ball ever overlaps another ball or a wall.
  - Velocity explosion does not occur (velocities remain bounded).
  - Determinism — two runs produce identical ball positions at each frame.
  - Balls settle correctly and the simulation reaches a stable resting state.
- Tests should be runnable via a single command (e.g., `cmake --build . && ctest`).

## Documentation & Tracking
- **`README.md`:** Keep this accurate and up to date at all times. It should reflect the current actual state of the project — not aspirational goals. Include: build instructions, run instructions, how to configure restitution, known bugs, and test instructions. Cross-reference with the codebase each iteration.
- **`ITERATIONNOTES.md`:** At the end of each iteration, append a brief entry noting: what was changed, why, and whether the change was triggered by a `PROMPT.md` update. Do not overwrite previous entries.
- **Code comments:** Comment at the method/function level (purpose, parameters, return value). Do not over-comment obvious logic. Keep comments accurate — outdated comments are worse than none.

## Constraints
- **Never modify `PROMPT.md`.** This is the only file that is off-limits.
- Do not create unnecessary files. Keep the repo clean.
- Do not create multiple stray `.md` files — documentation lives in `README.md` and `ITERATIONNOTES.md` only.
- Commit working changes at the end of each iteration with a descriptive commit message. Do not commit broken builds.
- When you think the simulator is complete, you can break the loop.
