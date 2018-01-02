#!/bin/bash
make
time ./ngrams -i small/small_static/small_static.init -q small/small_static/small_static.work >small_static_result.txt
diff small/small_static/small_static.result small_static_result.txt
echo ok small_static
time ./ngrams -i small/small_dynamic/small_dynamic.init -q small/small_dynamic/small_dynamic.work >small_dynamic_result.txt
diff small/small_dynamic/small_dynamic.result small_dynamic_result.txt
echo ok small_dynamic
time ./ngrams -i medium/medium_static/medium_static.init -q medium/medium_static/medium_static.work >medium_static_result.txt
diff medium/medium_static/medium_static.result medium_static_result.txt
echo ok medium_static
time ./ngrams -i medium/medium_dynamic/medium_dynamic.init -q medium/medium_dynamic/medium_dynamic.work >medium_dynamic_result.txt
diff medium/medium_dynamic/medium_dynamic.result medium_dynamic_result.txt
echo ok medium_dynamic

