#ifndef FSM_H
#define FSM_H

#include "prompt.h"

typedef int (*fsm_fn)(void);

int state_start(void);
int state_auth(void);
int state_open(void);
int state_join(void);
int state_interupt(void);
int fsm(void);

#endif
