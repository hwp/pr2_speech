#!/usr/bin/env python
import numpy
from pylab import *

subplot(411)
waveform = numpy.fromfile("sep_0.sw", numpy.int16)   
plot(waveform)
ylabel('Channel 0')

subplot(412)
waveform = numpy.fromfile("sep_1.sw", numpy.int16)   
plot(waveform)
ylabel('Channel 1')

subplot(413)
waveform = numpy.fromfile("sep_2.sw", numpy.int16)   
plot(waveform)
ylabel('Channel 2')

subplot(414)
waveform = numpy.fromfile("sep_3.sw", numpy.int16)   
plot(waveform)
ylabel('Channel 3')

xlabel('Time')
show()
