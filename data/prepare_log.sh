#!/bin/bash
cp autopilot.log autopilot.csv
sed -i -- 's/,/./g' autopilot.csv
sed -i -- 's/\t/,/g' autopilot.csv
