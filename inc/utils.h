
enum STATE {
	STATE_OK = 0,
	STATE_EOP = -1,		// error open file
	STATE_EFA = -2,		// Failed to allocate
	STATE_EFC = -3,		// not find Chatacter
	STATE_EEM = -4,		// not found
	STATE_ERR = 5};

void print_error(enum STATE state, int verbose);

