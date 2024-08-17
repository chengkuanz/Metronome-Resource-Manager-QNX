///Reference:
//Writing Resource Managers - Part 3 and time_resmgr.c,available on Brightspace 24S_CST8244_010 Real-Time Programming
//PulsesAndTimers and timer_example.c, available on Brightspace 24S_CST8244_010 Real-Time Programming
struct io_attr_t;
#define IOFUNC_ATTR_T struct io_attr_t
struct io_ocb_t;
#define IOFUNC_OCB_T struct io_ocb_t
#include <stdio.h>
#include <stdlib.h>
#include <sys/iofunc.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dispatch.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define METRONOME_H_

#define ROWS 8
#define COLLUMNS 4

#define START 0
#define STOP 1
#define PAUSE 2

#define MIN_PULSE_CODE			_PULSE_CODE_MINAVAIL
#define METRONOME_PULSE_CODE	(_PULSE_CODE_MINAVAIL + 1)
#define STOP_PULSE_CODE			(_PULSE_CODE_MINAVAIL + 2)
#define SET_PULSE_CODE			(_PULSE_CODE_MINAVAIL + 3)
#define QUIT_PULSE_CODE			(_PULSE_CODE_MINAVAIL + 4)
#define PAUSE_PULSE_CODE		(_PULSE_CODE_MINAVAIL + 5)

#define NumDevices 2
#define DEV_LOCAL_M 0
#define DEV_LOCAL_M_HELP 1
char *devnames[NumDevices] = { "/dev/local/metronome",
		"/dev/local/metronome-help" };

struct io_attr_t {
	iofunc_attr_t attr;
	int device;
}typedef io_attr_t;

struct io_ocb_t {
	iofunc_ocb_t ocb;
	char valuefer[50];
}typedef io_ocb_t;

char *metronomeData[ROWS][COLLUMNS] = { { 2, 4, 4, "|1&2&" }, { 3, 4, 6,
		"|1&2&3&" }, { 4, 4, 8, "|1&2&3&4&" }, { 5, 4, 10, "|1&2&3&4-5-" }, { 3,
		8, 6, "|1-2-3-" }, { 6, 8, 6, "|1&a2&a" }, { 9, 8, 9, "|1&a2&a3&a" }, {
		12, 8, 12, "|1&a2&a3&a4&a" } };

typedef union {
	struct _pulse pulse;
	char msg[255];
} my_message_t;

typedef struct {
	int bpm;
	int ts_top;
	int ts_bot;
	int interval;
	int metronomeState;
} metronomeSetting;

int findPattern(metronomeSetting*);
void* metronome_thread(void *arg);
int io_read(resmgr_context_t *ctp, io_read_t *msg, io_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, io_ocb_t *ocb);
int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra);
void printInterval(metronomeSetting*);
io_ocb_t* ioocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *ioattr);
void ioocb_free(io_ocb_t *ioocb);

struct itimerspec calculateTimer(metronomeSetting*);

char data[255];
int server_coid;
int dataRow;
int quit;

metronomeSetting settings;

void* metronome_thread(void *arg) {
	my_message_t msg;
	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	name_attach_t *attach = arg;
	int rcvid;
	int status;
	dataRow = findPattern(&settings);

	if (dataRow == -1) {
		perror("Invalid metronome settings\n");
		exit(EXIT_FAILURE);
	}

	// Phase I - create a named channel to receive pulses
	if ((attach = name_attach(NULL, "metronome", 0)) == NULL) {
		perror("Error attaching to namespace");
		exit(EXIT_FAILURE);
	}

	// create timer to trigger a pulse on interval
	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, attach->chid,
	_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
	event.sigev_code = MIN_PULSE_CODE;
	timer_create(CLOCK_MONOTONIC, &event, &timer_id);

	//Setting and starting a timer
	itime = calculateTimer(&settings);
	timer_settime(timer_id, 0, &itime, NULL);

	for (;;) {
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("Message Receive Pulse Failed\n");
			exit(EXIT_FAILURE);
		}

		if (rcvid == 0) {
			switch (msg.pulse.code) {
			case MIN_PULSE_CODE: //Initial pulse to start the metronome.
				status = START;
				printInterval(&settings); //handle 3 cases here
				break;

			case METRONOME_PULSE_CODE: //Pulse for each beat interval.
				if (status == STOP) {
					status = START;

					itime = calculateTimer(&settings);
					itime.it_value.tv_sec = 1;
					itime.it_value.tv_nsec = 0;

					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;

			case STOP_PULSE_CODE: //Stops the metronome.
				if (status == START || status == PAUSE) {
					status = STOP;

					itime = calculateTimer(&settings);
					itime.it_value.tv_sec = 0;
					itime.it_value.tv_nsec = 0;
					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;

			case PAUSE_PULSE_CODE: //Pauses the metronome for a specified duration.
				if (status == START) {
					status = PAUSE;

					itime = calculateTimer(&settings);
					itime.it_value.tv_sec = msg.pulse.value.sival_int;

					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;

			case SET_PULSE_CODE: //Changes metronome settings.
				dataRow = findPattern(&settings);
				if (dataRow == -1) {
					perror("Invalid metronome settings\n");
					settings.metronomeState = STOP;

					itime = calculateTimer(&settings);
					timer_settime(timer_id, 0, &itime, NULL);
					break;
				}

				calculateTimer(&settings);

				status = START;
				printf("\n");
				break;

			case QUIT_PULSE_CODE: //Quits the metronome, deletes the timer, and cleans up
				// implement Phase III:
				timer_delete(timer_id);				//  delete interval timer
				name_detach(attach, 0);				//  call name_detach()
				name_close(server_coid);				//  call name_close()
				exit(EXIT_SUCCESS);				//  exit with SUCCESS
			} //end of switch
		} //end of if
	} //end of for

} //end of metronome_thread

io_ocb_t* ioocb_calloc(resmgr_context_t *ctp, io_attr_t *ioattr) {
	io_ocb_t *ioocb;
	ioocb = calloc(1, sizeof(io_ocb_t));
	ioocb->ocb.offset = 0;
	return (ioocb);
}

void ioocb_free(io_ocb_t *ioocb) {
	free(ioocb);
}

//Determines the pattern for the specified time signature.
int findPattern(metronomeSetting *settings) {
	for (int i = 0; i < ROWS; i++) {
		if (settings->ts_top == (intptr_t) metronomeData[i][0]
				&& settings->ts_bot == (intptr_t) metronomeData[i][1]) {
			//set interval
			settings->interval = (intptr_t) metronomeData[i][2];
			//return row number of data
			return i;
		}
	}
	return -1; //did not found
}

//Displays the current beat of the metronome.
void printInterval(metronomeSetting *settings) {
	static int sequenceStart = 0;
	//start-of-measure:
	if (settings->metronomeState == 0) {
		printf("%.2s", metronomeData[dataRow][3]); //Print the first two characters of the pattern "|1"
		fflush(stdout);
		settings->metronomeState += 1; //
		sequenceStart += 2;
	} else if (settings->metronomeState != settings->interval) {
		//mid-measure
		printf("%.1s", metronomeData[dataRow][3] + sequenceStart);
		fflush(stdout);
		settings->metronomeState += 1; //move to the next beat
		sequenceStart += 1; //Move the sequence pointer to the next character
	} else if (settings->metronomeState == settings->interval) {
		// End of measure
		printf("%.1s\n", metronomeData[dataRow][3] + sequenceStart);
		fflush(stdout);
		settings->metronomeState = 0;
		sequenceStart = 0;
	}
}

// Calculates the timer interval based on the metronome settings
struct itimerspec calculateTimer(metronomeSetting *settings) {
	double calcBpm, measure, seconds, decimalSec, nanoSeconds;
	struct itimerspec itime;

	calcBpm = (double) 60 / settings->bpm; // Converts BPM to beats per second.
	measure = (double) calcBpm * settings->ts_top; // Time duration of one measure
	seconds = (double) measure / settings->interval; // Time interval between each beat.
	decimalSec = seconds - (int) seconds; //  Converts the fractional part of seconds to nanoseconds.

	// Calculate the nanoSeconds for the timer = decimal value x 10^9
	for (int i = 0; i < 9; i++) {
		nanoSeconds = (double) (decimalSec *= 10);
	}

	// Set starting and interval times
	itime.it_value.tv_sec = (int) seconds;
	itime.it_value.tv_nsec = nanoSeconds;
	itime.it_interval.tv_sec = (int) seconds;
	itime.it_interval.tv_nsec = nanoSeconds;

	return itime;
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, io_ocb_t *ocb) {
	int nb;		//Number of bytes in message
	int i = findPattern(&settings);

	// Check if message has been populated
	if (data == NULL)
		return 0;

	//Check which device we have
	if (ocb->ocb.attr->device == DEV_LOCAL_M) {
		double calcBpm, measure, seconds, decimalSec, nanoSeconds;

		calcBpm = (double) 60 / settings.bpm;// Calculate metronome beats per min to beats per second
		measure = (double) calcBpm * (intptr_t) metronomeData[i][0];// Calculate time between each measure
		seconds = (double) measure / (intptr_t) metronomeData[i][1];// Calculate seconds between each interval
		decimalSec = seconds - (int) seconds;// Get the decimal value from double seconds
		// Calculate the nanoSeconds for the timer = decimal value x 10^9
		for (int i = 0; i < 9; i++) {
			nanoSeconds = (double) (decimalSec *= 10);
		}

		sprintf(data,
				"[metronome: %d beats/min, time signature: %d/%d, sec-per-interval: %.2f, nanoSecs: %.0f]\n",
				settings.bpm, metronomeData[i][0], metronomeData[i][1], seconds,
				nanoSeconds);
	} else if (ocb->ocb.attr->device == DEV_LOCAL_M_HELP) {
		sprintf(data,
				"Metronome Resource Manager (ResMgr)\n"
						"\nUsage: metronome <bpm> <ts-top> <ts-bottom>\n\nAPI:\n"
						"pause[1-9]                     -pause the metronome for 1-9 seconds\n"
						"quit                           -quit the metronome\n"
						"set <bpm> <ts-top> <ts-bottom> -set the metronome to <bpm> ts-top/ts-bottom\n"
						"start                          -start the metronome from stopped state\n"
						"stop                           -stop the metronome; use 'start' to resume\n");
	}

	// Get length of message
	nb = strlen(data);

	//check if the offset of the open control block (OCB) has reached
	//the number of bytes (nb) available in the data.
	//test to see if we have already sent the whole message
	if (ocb->ocb.offset == nb)
		return 0;

	//We will return which ever is smaller the size of our data or the size of the buffer
	nb = min(nb, msg->i.nbytes);

	// Sets the number of bytes to be returned
	_IO_SET_READ_NBYTES(ctp, nb);

	//Copy data into reply buffer
	SETIOV(ctp->iov, data, nb);

	// update offset into our data used to determine start position for next read.
	ocb->ocb.offset += nb;

	// If we are going to send any bytes, update the access time for this resource.
	if (nb > 0) {
		ocb->ocb.flags |= IOFUNC_ATTR_ATIME;
	}

	return (_RESMGR_NPARTS(1));
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, io_ocb_t *ocb) {
	int nb = 0; 	//Number of bytes

	if (msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg))) {

		char *value;
		char *pause_msg;
		char *set_msg;
		int small_int = 0;
		value = (char*) (msg + 1);

		if (strstr(value, "start") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL),
			METRONOME_PULSE_CODE, small_int);
		} else if (strstr(value, "stop") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), STOP_PULSE_CODE,
					small_int);
		} else if (strstr(value, "pause") != NULL) {

			for (int i = 0; i < 2; i++) {
				pause_msg = strsep(&value, " ");
			}

			small_int = atoi(pause_msg);

			if (small_int >= 1 && small_int <= 9) {
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL),
				PAUSE_PULSE_CODE, small_int);
			} else {
				printf("\nInteger is not between 1 and 9.\n");
			}
		} else if (strstr(value, "set") != NULL) {
			for (int i = 0; i < 4; i++) {
				set_msg = strsep(&value, " ");
				if (i == 1) {
					settings.bpm = atoi(set_msg);
				} else if (i == 2) {
					settings.ts_top = atoi(set_msg);
				} else if (i == 3) {
					settings.ts_bot = atoi(set_msg);
				}
			}

			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), SET_PULSE_CODE,
					small_int);
		} else if (strstr(value, "quit") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), QUIT_PULSE_CODE, 0);
			quit = 1;
			printf("\n Metronome quitting.....\n");
		} else {
			printf("\n Invalid command... \n");
		}

		nb = msg->i.nbytes;
	} // end of if(msg)

	_IO_SET_WRITE_NBYTES(ctp, nb);

	if (msg->i.nbytes > 0) {
		ocb->ocb.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
	}

	return (_RESMGR_NPARTS(0));
}

int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra) {
	if ((server_coid = name_open("metronome", 0)) == -1) {
		perror("name_open failed");
		return EXIT_FAILURE;
	}

	return (iofunc_open_default(ctp, msg, handle, extra));
}

//print commend line usage
void print_usage(void) {
	printf("Usage: metronome <bpm> <ts-top> <ts-bottom>\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	dispatch_t *dpp;
	resmgr_io_funcs_t io_funcs;
	resmgr_connect_funcs_t connect_funcs;
	io_attr_t ioattr[NumDevices];
	dispatch_context_t *ctp;
	name_attach_t *attach;

	pthread_attr_t tattr;
	pthread_t metroThread;

	int resmgr_id = 0;
	quit = 0;

	// verify number of command-line arguments == 4
	if (argc != 4) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	// Get values from command line
	settings.bpm = atoi(argv[1]);
	settings.ts_top = atoi(argv[2]);
	settings.ts_bot = atoi(argv[3]);
	settings.metronomeState = 0;

	//check if pattern stored in the table
	if (findPattern(&settings) == -1) {
		fprintf(stderr, "\n Invalid metronome settings\n");
		exit(EXIT_FAILURE);
	}

	//We then need to put these new functions into an array which defines which
	//functions to call when doing memory resizing
	iofunc_funcs_t ioocb_funcs = { _IOFUNC_NFUNCS, ioocb_calloc, ioocb_free };
	iofunc_mount_t ioocb_mount = { 0, 0, 0, 0, &ioocb_funcs };

	//dispatch interface initialization
	dpp = dispatch_create();

	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS,
			&io_funcs);

	// Set the user defined io functions to override primitive io functions.
	connect_funcs.open = io_open;
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// Attach the resource manager to the pathname space
	for (int i = 0; i < NumDevices; i++) {
		iofunc_attr_init(&ioattr[i].attr, S_IFCHR | 0666, NULL, NULL);
		ioattr[i].device = i;
		ioattr[i].attr.mount = &ioocb_mount;
		resmgr_attach(dpp, NULL, devnames[i], _FTYPE_ANY, 0, &connect_funcs,
				&io_funcs, &ioattr[i]);
	}

	ctp = dispatch_context_alloc(dpp);

	pthread_attr_init(&tattr);
	pthread_create(NULL, &tattr, &metronome_thread, &settings);

	while (quit != 1) {
		ctp = dispatch_block(ctp);
		dispatch_handler(ctp);
	}

	//quit process
	pthread_join(metroThread, NULL);
	name_close(server_coid);
	name_detach(attach, 0);
	resmgr_detach(dpp, resmgr_id, 0);

	return EXIT_SUCCESS;
}

