# Nine Lives

Cloned from [Shovel](https://github.com/aksommerville/shovel) at `de91a464728448405762a9e067c9b0839aaf4ce7`.

Entry for js13k 2025, theme "BLACK CAT".

## TODO

- [x] Rephrase sprite position as ints (x,y,w,h). They're mostly going to be the same size, and recording the center position is i think more trouble than it's worth.
- [x] Hero sprite: Spawn, motion, collisions, jump.
- [x] Proper jumping.
- [x] Flying.
- [ ] Spikes.
- [ ] Laser.
- [x] Lay eggs.
- [x] Use corpses as platforms.
- [ ] Exit map.
- [ ] With the core mechanics above implemented, make the call on removing text and hello.
- [ ] Decorative mouse at start of each map.
- [ ] Gore under the corpse. If impaled, have it drip blood onto the floor.
- [ ] Show remaining eggs at the bottom of the screen.
- [ ] Breathing fire.
- [ ] Game over. Win only; if you die, just the map resets.
- [ ] Output video sizing is imperfect. Visible especially at 1x. (xegl). Update Shovel if we fix it.
- [ ] Remove AUX1-to-quit before release, at least for web.
- [x] Eliminate multiplayer input, definitely don't need it.
- [ ] Eliminate persistence if we're not keeping a high score. Are we?

## Notes

Preliminary sizes:
 -  986 caught_the_mouse
 - 1564 get_over_here
 -  636 nine_lives
 -  560 graphics
 - 9625 zipped bundle

That's about 3 kB of music. Could thin it out if needed.
If we're going to reduce it to just one song, as I think we will, why not make that one song ridiculously long?
If we keep it reasonably thin, we could probly get like 5 or 10 minutes of music out of it.

We could probably drop text. Might still need numbers, for presenting the score or clock.

I assumed there would be Hello and Gameover modals. Don't necessarily need those.

Jump: Can ascend 2 meters, not 3. With a single corpse, can ascend 3.
Laterally at same elevation, can clear a 5-meter gap, not 6.

## Map Format

Input is text, a picture of the map using two characters per tile:
 - `.,` = empty
 - `Xx` = wall
 - `Ss` = spikes
 - `Ll` = laser
 - `Hh` = hero spawn
 - `Dd` = door

There's 220 tiles per map, of which the overwhelming majority will be empty and wall, often in long runs.

```
00cccccc : (c+1) empty.
01cccccc : (c+1) wall.
1000cccc : (c+1) spikes.
10xxxxxx : Reserved.
11000000 : Spawn point (empty).
11000001 : Laser.
11000010 : Door.
11xxxxxx : Reserved.
```

Neighbor-joining is the runtime's problem, that keeps the encoded data tighter.
Unspecified tiles are wall, so encoder can safely trim those from the end.
First trial map, admittedly a sparse one, is only 30 bytes. Looking good even without a vertical filter.
