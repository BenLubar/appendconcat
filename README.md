appendconcat
============

[![Build Status](https://drone.io/github.com/BenLubar/appendconcat/status.png)](https://drone.io/github.com/BenLubar/appendconcat/latest)

Ok, so here's the plan:

1. When you play this game, there is randomly generated stuff.
2. This stuff is saved to the end of a [gzipped](http://www.gzip.org/) [protobuf](https://developers.google.com/protocol-buffers/) file. That's the append part.
3. If you and a friend have saved games, you can concatenate them to create a new saved game containing a combination of the storylines from both original saves.
