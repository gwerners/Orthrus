#!/bin/bash
ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/bench
#ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/sms
#ab -n 10000 -c 1 -k http://127.0.0.1:8085/tefmock/ussd
#ab -n 10000 -c 1000 -k http://127.0.0.1:8085/tefmock/ussd
