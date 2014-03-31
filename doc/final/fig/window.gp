f(t) = 0.5*(1-cos(2*pi*t)) * 0.25 * (sgn(t)+1) * (sgn(1-t)+1)
 
set multiplot layout 2, 1 

set xrange [0:4]
set yrange [-.3:1.3]
set samples 1000

unset key
unset border
unset ytics
set xtics axis

set style line 1 lw 2 lc rgb "red"

set title "$L=0.5N$"
plot f(x) ls 1, f(x-0.5) ls 1, f(x-1) ls 1, f(x-1.5) ls 1, \
      f(x-2) ls 1, f(x-2.5) ls 1, f(x-3) ls 1, 0 lw 3 lc rgb "black"

set title "$L=1.2N$"
plot f(x) ls 1, f(x-1.2) ls 1, f(x-2.4) ls 1, f(x-3.6) ls 1, 0 lw 3 lc rgb "black"

unset multiplot

