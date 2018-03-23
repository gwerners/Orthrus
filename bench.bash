#!/bin/bash
DATE=`date +%Y%m%d`
echo "######################## testing lua" > ${DATE}_bench.txt
ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/bench >> ${DATE}_bench.txt
echo "######################## testing c++" >> ${DATE}_bench.txt
ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/sms >> ${DATE}_bench.txt
echo "######################## testing java" >> ${DATE}_bench.txt
ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/ussd >> ${DATE}_bench.txt
#ab -n 10000 -c 1000 -k http://127.0.0.1:8085/tefmock/ussd
