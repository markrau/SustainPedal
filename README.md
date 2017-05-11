<h1>Sustain Pedal</h1>
<h3>EE 264 Project</h3>

<h2>No pain No Sustain</h2>

<p>This is a guitar sustain pedal optimized for real time performance on a fixed point processor.For every incoming buffer, note onset is detected. Once a note has reached its steady state, we try to detect its pitch and loop it constantly in the output buffer(with phase alignment), as long as a new note isn't played.</p>

<p>Onset detection is done by calculating spectral energy change in consecutive buffers. The FFT implementations in TI C55x DSP processor are utilized for this. Pitch detection is done in time domain by detecting candidate peaks in the audio buffer and finding the difference between their indices. Both onset and pitch detection require minimal calculation and can run real time on a constrained fixed point processor. They are pretty accurate.</p>

<p>Here is a picture of the setup. The button on the extreme right with the LED turns sustain on and off. The potentiometer to the left of the on/off button adjusts a threshold for onset detection.</p>

<img src = "https://github.com/markrau/SustainPedal/blob/master/controller.jpg"></img>
