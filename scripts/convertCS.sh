# Script to extract context switch overhead values in CSV format 
# Replace 'CSPFAIR0' with the scheduler being traced 

./ft2csv CXS_START Trace0 > CSPFAIR0
./ft2csv CXS_START Trace1 > CSPFAIR1
./ft2csv CXS_START Trace2 > CSPFAIR2
./ft2csv CXS_START Trace3 > CSPFAIR3
./ft2csv CXS_START Trace4 > CSPFAIR4
./ft2csv CXS_START Trace5 > CSPFAIR5
./ft2csv CXS_START Trace6 > CSPFAIR6
./ft2csv CXS_START Trace7 > CSPFAIR7
