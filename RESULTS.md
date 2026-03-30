Because of the credit shortage, I did this entirely in codex. 

### 1: 
The agent made the simulator and it doesn't have any major bugs- no balls phasing through walls or one another. However, the current style looks
awful. It's really slow, and the balls fall in a controlled row, not really looking like physics is acting on it at all. I prompted it to be 
deterministic, as we discussed in class, and I think this was taken into account too well. 

### 2: 
Updated the prompt to specify that the balls shouldn't fall in a single horizontal line, and that physics should be more of an emphasis in the
simulation. It looks better now, balls are falling into the container from two directions at a much faster rate. However, now the balls don't stick
to the bounds of the container well at all. The right side of the container has a large gap. The balls also are still not really looking as though 
they are solely acted on by gravity, but rather they look like they're being shot directly to their predetermined position. I'm okay with this
current state, but would like to improve the physics feel and look.

### 3:
The physics appear to have been improved, but the program has run into a new issue. In hindsight, I should've foreseen this happening. The
balls initially appear to be in free-fall, but when they get close to the container and what will be their resting place, they skip frames and
immediately show up at rest. They are also two different colors when in free-fall versus at rest which is obviously not correct, especially when
considering the next phase of the simulator.
Another minor issue is the balls aren't really conforming to the container, especially on the right side of the container.
The update I will be making is emphasizing the continuity of a ball object. A ball object shouldn't really change at all. It should be a
collision object. It should be treated as though it is being acted on by gravity until bound by the container and/or other balls.

### 4:
Current version, the physics still kind of suck when the balls fall into their final location. I don't know if codex just doesn't understand the looping quite as well, but it didn't iterate much. In fact, each time I have given it the iterative prompt, it loops less and less. This time around, it only changed a couple dozen lines which isn't much of a change. That being said, it did fix the major bug from the previous iterations. However, the balls break free-fall at the end and slow down and move to their resting point, rather than accelerating form gravity until it collides with balls or the floor or wall and then resting. There is still a gap between the balls and the wall on the right side so that bug was completely ignored as well.

### 5: 
After adding instructions to create the image, it iterated several times, changing the color of the balls. After it finished, the program was unbelievably slow, so I actually made a change to the prompt and reran. My change was focused on improving efficiency, and a key aspect of this was to change the number of balls used to make the image. My goal with this was to get the simulation to be smooth again and to get the general image created. The first was certainly accomplished- it ended up using about 400 balls (I didn't specify how many to reduce to) and the simulation looks significantly better. The image, however is pretty off. There are a couple random white balls scattered around the image that are unneccessary. Next step will be focused on correcting the final image. Physics could also use a slight update. I'm considering starting a new simulator from scratch, as I wonder if it will be actually be able to improve at all.
