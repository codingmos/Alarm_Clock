#ifndef FSM_H_
#define FSM_H_

/*INCLUDES *******************************************************************/
#include "ses_common.h"

typedef struct fsm_s Fsm; //< typedef for alarm clock state machine
typedef struct event_s Event; //< event type for alarm clock fsm
/** return values */
enum {
RET_HANDLED, //< event was handled
RET_IGNORED, //< event was ignored; not used in this implementation
RET_TRANSITION //< event was handled and a state transition occurred
};
typedef uint8_t fsmReturnStatus; //< typedef to be used with above enum

/** typedef for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event*);

struct fsm_s {
State state; //< current state, pointer to event handler
bool isAlarmEnabled; //< flag for the alarm status
struct time_t timeSet; //< multi-purpose var for system time and alarm time
};

typedef struct event_s {
uint8_t signal; //< identifies the type of event
} Event;


#endif /* FSM_H_ */
