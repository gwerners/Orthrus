#!/bin/bash
gprof server_jit.exe gmon.out > analysis.txt
rm gmon.out
