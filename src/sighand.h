#ifndef SIGHAND
#define SIGHAND

void chld_handler(int);
void stop_handler(int);
void int_handler(int);
void installSigHandlers();

#endif // SIGHAND
