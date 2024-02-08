#!/bin/bash

for i in $(ls scriptSARP*.sh) ; do screen ./$i & ; done
