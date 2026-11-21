#ifndef HYPE_TIME
#define HYPE_TIME

/* Private Hype functions, should be hidden from partitions */
void __time_init();
int __time_get(unsigned int * seconds, unsigned int * microseconds);

#endif
