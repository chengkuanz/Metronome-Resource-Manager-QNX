# Metronome Resource Manager - QNX Neutrino

## Overview

This project is a metronome simulation developed as part of the CST8244 - Real-Time Programming course. The metronome operates using timers to produce a rhythmic pattern based on specified beats per minute (BPM) and time signatures. The metronome supports commands to pause, start, stop, and configure its settings through a custom API. It is implemented as a QNX resource manager, allowing it to interact with the system through a designated device file `/dev/local/metronome`.

The project demonstrates the use of **multi-threading**, **pulse messaging**, and real-time **timer implementation** for in a QNX environment. It also handles concurrent command inputs while maintaining the metronome's timing accuracy.

## Author
[Chengkuan Zhao](https://github.com/chengkuanz)     
[Thi Thanh Van Le](https://github.com/Le-Vivian)    
May 2024 - August 2024

## Functional Requirements

- **Multi-Threaded Design**: The metronome is implemented as a multi-threaded application, with a main thread managing the resource manager and a separate thread handling the timing and output of the metronome beats.
- **Custom Commands**: The metronome supports commands via the `echo` command and interacts with the resource manager through `/dev/local/metronome`.
- **Precision Timing**: Accurate timing is maintained based on the BPM and time signature provided, ensuring the correct rhythmic pattern is produced.

## Metronome API

### Commands

- **cat /dev/local/metronome**: Displays the current settings of the metronome, including BPM, time signature, and timing intervals.
- **cat /dev/local/metronome-help**: Displays help information regarding the available commands and their usage.
- **echo pause [1-9] > /dev/local/metronome**: Pauses the metronome for a specified number of seconds (1-9). The metronome resumes on the next beat after the pause duration.
- **echo quit > /dev/local/metronome**: Terminates the metronome resource manager gracefully.
- **echo set <bpm> <ts-top> <ts-bottom> > /dev/local/metronome**: Configures the metronome with a new BPM and time signature. The metronome adjusts its behavior accordingly.
- **echo start > /dev/local/metronome**: Starts the metronome if it is in a stopped state.
- **echo stop > /dev/local/metronome**: Stops the metronome while keeping the resource manager running.
- **echo quit > /dev/local/metronome**: Gracefully terminates the metronome.
- **echo help > /dev/local/metronome**: Displays help information regarding the available commands and their usage.


### Error Handling

- The metronome resource manager provides formatted error messages for invalid commands and continues operation without interruption.

### Usage Example

```
# Start the metronome with 120 BPM and a 2/4 time signature
metronome 120 2 4

# Display current settings
cat /dev/local/metronome

# Pause the metronome for 3 seconds
echo pause 3 > /dev/local/metronome

# Change settings to 200 BPM and 5/4 time signature
echo set 200 5 4 > /dev/local/metronome

# Stop the metronome
echo stop > /dev/local/metronome

# Start the metronome again
echo start > /dev/local/metronome

# Terminate the metronome resource manager
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

### Running the Project

1. **Start the Metronome**:
    - Navigate to `/tmp` and run the metronome with the desired parameters:

```
cd ../../../
cd tmp
./metronome <bpm> <ts-top> <ts-bottom> 
```


2. **Interact with the Metronome**:
    - Use the `echo` commands provided in the API section to control the metronome.

3. **Monitor and Modify**:
    - Use `cat` commands to check the current settings and modify them as needed using `echo` commands.

### Exiting the Program

- To terminate the metronome, use:

```
echo quit > /dev/local/metronome
```

## Acceptance Test Script

The acceptance-test.sh script automates the testing of the metronome's functionality. The script covers various scenarios, including starting, stopping, pausing, and changing settings.

To run the acceptance test script:
```
./acceptance-test.sh
```
This script verifies the correct behavior of the metronome in response to different inputs and scenarios, ensuring the system meets the specified requirements.
