# SustainPedal
EE 264 Project

###No pain No Sustain

This is a guitar sustain pedal optimized for real time performance on a fixed point processor.For every incoming buffer, note onset is detected. Once a note has reached its steady state, we try to detect its pitch and loop it constantly in the output buffer(with phase alignment), as long as a new note isn't played.

To do : the pitch detection needs to be improved.

Here is a picture of the setup. The button on the extreme right with the LED turns sustain on and off. The potentiometer to the left of the on/off button adjusts a threshold for onset detection.

<img src = "https://github.com/markrau/SustainPedal/blob/master/IMG_20170308_161513.jpg"></img>
