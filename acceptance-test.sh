#######################################################################
## - Acceptance Test Script
## - Assignment 2 - Metronome
#######################################################################
echo "Unit Test [C] \n"
echo "/dev/local/metronome\n"
## Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]
cat /dev/local/metronome
sleep 3

echo "Unit Test [D] \n"
echo "/dev/local/metronome-help\n"
## Expected: information regarding the metronome resmgr’s API
cat /dev/local/metronome-help
sleep 3

echo "Unit Test [E] \n"
echo "set 100 2 4 > /dev/local/metronome\n"
## Expected: metronome regmgr changes settings to: 100 bpm in 2/4 time; 
##           run-time behaviour of metronome changes to 100 bpm in 2/4 time.
echo set 100 2 4 > /dev/local/metronome
sleep 3


echo "Unit Test [F] \n"
## Expected: [metronome: 100 beats/min, time signature 2/4, secs-per- interval: 0.30, nanoSecs: 300000000]
cat /dev/local/metronome
sleep 3
echo "/dev/local/metronome\n"

echo "Unit Test [G] \n"
## Expected: metronome regmgr changes settings to: 200 bpm in 5/4 time; 
##           run-time behaviour of metronome changes to 200 bpm in 5/4 time.
echo set 200 5 4 > /dev/local/metronome
sleep 3
echo "set 200 5 4 > /dev/local/metronome\n"

echo "Unit Test [H] \n"
## Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]
cat /dev/local/metronome
sleep 3
echo "/dev/local/metronome\n"

echo "Unit Test [I] \n"
## Expected: metronome stops running; 
##           metronome resmgr is still running as a process: pidin | grep metronome
echo stop > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "stop > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [j] \n"
## Expected: metronome starts running again at 200 bpm in 5/4 time, which is the last setting; metronome 
##           resmgr is still running as a process: pidin | grep metronome
echo start > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "start > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [k] \n"
## Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]
cat /dev/local/metronome
sleep 3
echo "/dev/local/metronome\n"

echo "Unit Test [l] \n"
## Expected: metronome stops running; 
##           metronome resmgr is still running as a process: pidin | grep metronome
echo stop > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "stop > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [m] \n"
## Expected: metronome remains stopped; 
##           metronome resmgr is still running as a process: pidin | grep metronome.
echo stop > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "stop > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [n] \n"
## Expected: metronome starts running again at 200 bpm in 5/4 time, which is the last setting; 
##           metronome resmgr is still running as a process: pidin | grep metronome
echo start > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "start > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [o] \n"
## Expected: metronome is still running again at 200 bpm in 5/4 time, which is the last setting; 
##           metronome resmgr is still running as a process: pidin | grep metronome
echo start > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "start > /dev/local/metronome && pidin | grep metronome\n"

echo "Unit Test [p] \n"
## Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]
cat /dev/local/metronome
sleep 3
echo "/dev/local/metronome\n"

echo "Unit Test [q] \n"
## Expected: metronome continues on next beat (not next measure)
echo pause 3 > /dev/local/metronome
sleep 3
echo "pause 3 > /dev/local/metronome\n"

echo "Unit Test [r] \n"
## Expected: properly formatted error message, and metronome continues to run.
echo pause 10 > /dev/local/metronome
sleep 3
echo "pause 10 > /dev/local/metronome\n"

echo "Unit Test [s] \n"
## Expected: properly formatted error message, and metronome continues to run.
echo bogus > /dev/local/metronome
sleep 3
echo "bogus > /dev/local/metronome\n"

echo "Unit Test [t] \n"
## Expected: 1 measure per second.
echo set 120 2 4 > /dev/local/metronome
sleep 3
echo "set 120 2 4 > /dev/local/metronome\n"

echo "Unit Test [u] \n"
## Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]
cat /dev/local/metronome
sleep 3
echo "/dev/local/metronome\n"

echo "Unit Test [v] \n"
## Expected: information regarding the metronome resmgr’s API, as seen above.
cat /dev/local/metronome-help
sleep 3
echo "/dev/local/metronome-help\n"

echo "Unit Test [w] \n"
## Expected: properly formatted error message, and metronome continues to run.
echo Writes-Not-Allowed > /dev/local/metronome-help
sleep 3
echo "Writes-Not-Allowed > /dev/local/metronome-help\n"

echo "Unit Test [x] \n"
## Expected: metronome gracefully terminates
echo quit > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "quit > /dev/local/metronome && pidin | grep metronome\n"
echo "ALL TEST COMPLETE\n"