# SustainPedal
EE 264 Project

###No pain No Sustain

This is a guitar sustain pedal optimized for real time performance on a fixed point processor.For every incoming buffer, note onset is detected. Once a note has reached its steady state, we try to detect its pitch and loop it constantly in the output buffer(with phase alignment), as long as a new note isn't played.

To do : the pitch detection needs to be much better.

