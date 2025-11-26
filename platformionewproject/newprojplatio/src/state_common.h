#ifndef STATE_COMMON.H
#define STATE_COMMON.H
#include <stdint.h> //idk why I need this


typedef enum {
  STATE_UNOCC,
  STATE_OCC,
  STATE_MAINT
} state_e; //enumerate states, then give then the type state_e


typedef enum{
  EVENT_OCC_DET,
  EVENT_UNOCC_DET,
  EVENT_MAINT,
  EVENT_ONLINE,
  EVENT_NONE
} event_e; 

struct state_machine_data; //will be defined in state_machine.cpp

struct common_state_data 
{
  struct state_machine_data *state_machine_data;
  //I wonder if we need a struct just to hold one pointer, in the video there was a lot more (20:18)
  //this might be where the json goes
};

//all of the states can access this function
void state_machine_post_internal_event(struct state_machine_data *data, event_e event);

#endif