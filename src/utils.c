#include "../inc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_error(enum STATE state, int verbose)
{
	switch (state) {
		case STATE_EOP:
			if (verbose)
				fprintf(stderr, "Error: ");
			fprintf(stderr, "[EOP] The file cannot be opened. "
					"The file name is incorrect or the"
				        " file does not exist.\n");
			break;
		case STATE_EFA:
			if (verbose)
				fprintf(stderr, "Error: ");
			fprintf(stderr, "[EFA] The function could not "
					"allocate the requested memory"
					"block.\n");
			break;
		case STATE_EFC:
			if (verbose)
				fprintf(stderr, "Error: ");
			fprintf(stderr, "[EFC] Data entered incorrectly.\n");
			break;
		case STATE_EEM:
			if (verbose)
				fprintf(stderr, "Error: ");
			fprintf(stderr, "[ERR] Record not found in database.");
			if (verbose)
				fprintf(stderr, " The number is out of range.");
			fprintf(stderr, "\n");
			break;
		default:
			break;
	}
}

