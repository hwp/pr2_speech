# Sampling and Quantization of a Audio Signal

f(t) = .3*sin(t)+.05*sin(4*t+3)+.4*sin(5*t+1)

set xrange [0: 4]
set yrange [-1: 1]

set multiplot

set samples 1000
plot f(x)

set samples 20
plot f(x) with impulses

unset multiplot

