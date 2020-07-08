gprof $1 | python gprof2dot.py | dot -Tpng -o gprof_$1.png
eog gprof_$1.png &

