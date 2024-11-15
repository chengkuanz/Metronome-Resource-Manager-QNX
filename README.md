# Metronome Resource Manager - QNX Neutrino

## Overview

This project is a metronome simulation designed as part of the CST8244 - Real-Time Programming course. The metronome is implemented as a QNX resource manager and operates on the QNX Neutrino RTOS. It simulates rhythmic patterns based on user-specified beats per minute (BPM) and time signatures. The system supports commands for starting, stopping, pausing, and adjusting settings via a device file interface, ensuring precise timing and real-time responsiveness.

The project demonstrates the use of **multi-threading**, **pulse messaging**, **resource managers**, **timers**, and **message passing**. These technologies are employed to create a responsive and accurate metronome that can be controlled via simple command-line interactions.

## Demo Video



https://github.com/user-attachments/assets/d40793ac-ab0c-4f8b-8dad-7abf7b41f732



## Authors

[Chengkuan Zhao](https://github.com/chengkuanz)  
[Thi Thanh Van Le](https://github.com/Le-Vivian)  
May 2024 - August 2024

## System Components

- **Metronome**: The core functionality, which maintains a rhythmic pattern based on BPM and time signature settings.
- **Command Interface**: Provides commands to start, stop, pause, and configure the metronome via `/dev/local/metronome`.
- **Resource Manager**: Handles all interactions with the metronome through the QNX filesystem.

## Project Components

The project consists of a single program that implements a metronome as a QNX resource manager:

- **`metronome`**: The main program that creates a metronome resource manager. It handles incoming commands to control the metronome and maintains accurate timing using real-time timers. The logic for the metronome is implemented in `metronome.c`, while the command interface is managed via the resource manager functions.

## Technologies and Concepts Used

- **QNX Neutrino RTOS**: The real-time operating system used to run the metronome.
- **C Programming Language**: The language used to implement the project.
- **Resource Manager**: Implements the metronome as a resource manager that interacts with the QNX filesystem.
- **Pulse Messaging**: Used to handle timing and command execution in real-time.
- **Timers**: Provides accurate timing for the metronome beats based on BPM and time signature.
- **Multi-Threading**: Separates the metronome timing logic from command handling to ensure responsiveness.
- **State Machine Design**: Manages the state transitions of the metronome (e.g., start, stop, pause) based on user commands.
- **Message Passing**: Facilitates communication between the resource manager and the metronome logic.

## Usage

### Commands

- **cat /dev/local/metronome**: Displays the current metronome settings, including BPM, time signature, and timing intervals.
- **cat /dev/local/metronome-help**: Displays help information regarding the available commands and their usage.
- **echo start > /dev/local/metronome**: Starts the metronome.
- **echo stop > /dev/local/metronome**: Stops the metronome while keeping the resource manager running.
- **echo pause [1-9] > /dev/local/metronome**: Pauses the metronome for a specified number of seconds (1-9).
- **echo set <bpm> <ts-top> <ts-bottom> > /dev/local/metronome**: Configures the metronome with a new BPM and time signature.
- **echo quit > /dev/local/metronome**: Gracefully terminates the metronome resource manager.

### Running the Metronome

1. **Start the Metronome**:
    - Navigate to `/tmp` and run the metronome with the desired parameters:

    ```
    ./metronome <bpm> <ts-top> <ts-bottom>
    ```

   Example:
    ```
    ./metronome 120 4 4
    ```

2. **Interact with the Metronome**:
    - Use the `echo` commands provided above to control the metronome.

3. **Monitor and Modify**:
    - Use `cat` commands to check the current settings and modify them as needed using `echo` commands.

### Exiting the Program

- To terminate the metronome, use:

    ```
    echo quit > /dev/local/metronome
    ```

## Installation and Setup

### Prerequisites

- **QNX Momentics IDE**: Required to build and run the project.
- **QNX Neutrino RTOS**: The target operating system for running the metronome.

### Setting Up the Project

1. **Clone the Project**: Download or clone the project from the repository.

2. **Import the Project into QNX Momentics**:
    - Open QNX Momentics IDE.
    - Create a new workspace.
    - Import the project by selecting `File > Import... > General > Existing Projects into Workspace`.

3. **Build the Project**:
    - Clean and build the project by selecting `Project > Clean...` and then `Project > Build All`.

4. **Copy the binary files** to the target machine's `/tmp` folder.

## Sample Scenarios

- **Start the Metronome**: Initialize the metronome with a BPM of 120 and a 4/4 time signature, then start it.
- **Pause the Metronome**: Pause the metronome for 3 seconds and observe how it resumes on the next beat.
- **Change Settings**: Set a new BPM of 200 and a 5/4 time signature while the metronome is running.
- **Stop and Start**: Stop the metronome, then start it again with the previous settings.

## Acceptance Test Script
The acceptance-test.sh script automates the testing of the metronome's functionality. The script covers various scenarios, including starting, stopping, pausing, and changing settings.

To run the acceptance test script, copy the 'acceptance-test.sh' file to the target machine `tmp` folder and execute the following command:
```
./acceptance-test.sh
```
This script verifies the correct behavior of the metronome in response to different inputs and scenarios, ensuring the system meets the specified requirements.

<details>
  <summary>acceptance-test.sh</summary>
    <pre><code>
#######################################################################
## - Acceptance Test Script
## -  Metronome
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
    </code></pre>
</details>
