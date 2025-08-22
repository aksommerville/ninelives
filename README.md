# Nine Lives

Cloned from [Shovel](https://github.com/aksommerville/shovel) at `de91a464728448405762a9e067c9b0839aaf4ce7`.

Entry for js13k 2025, theme "BLACK CAT".

## TODO

## Notes

Preliminary sizes:
 -  986 caught_the_mouse
 - 1564 get_over_here
 -  636 nine_lives
 -  560 graphics
 - 9625 zipped bundle

That's about 3 kB of music. Could thin it out if needed.

## Map Format

Input is text, a picture of the map using two characters per tile:
 - `.,` = empty
 - `Xx` = wall
 - `Ss` = spikes
 - `Ll` = laser
 - `Hh` = hero spawn

There's 220 tiles per map, of which the overwhelming majority will be empty and wall, often in long runs.

```
00cccccc : (c+1) empty.
01cccccc : (c+1) wall.
1000cccc : (c+1) spikes.
10xxxxxx : Reserved.
11000000 : Spawn point (empty).
11000001 : Laser.
11xxxxxx : Reserved.
```

Neighbor-joining is the runtime's problem, that keeps the encoded data tighter.
Unspecified tiles are wall, so encoder can safely trim those from the end.
First trial map, admittedly a sparse one, is only 30 bytes. Looking good even without a vertical filter.
