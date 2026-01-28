/*#ifndef STATE_MACHINE.H
#define STATE_MACHINE.H

#include <stdint.h> //idk why I need this, it was in state_common originally


// ENUM AND STRUCTS

typedef enum {
  STATE_UNOCC,
  STATE_OCC,
  STATE_MAINT
} state_e; //enumerate states, then give then the type state_e

/*

typedef enum{
  EVENT_OCC_DET,
  EVENT_UNOCC_DET,
  EVENT_MAINT,
  EVENT_ONLINE,
  EVENT_NONE
} event_e; 





//state transition struct
struct state_transition
{
  state_e from;
  //event_e event;
  state_e to;
};


/* 
//state transition table (array of the state transition struct type we just made above)
static const struct state_transition state_transitions[] = {
//current state, event, new state
{STATE_UNOCC, EVENT_OCC_DET, STATE_OCC}, 
{STATE_UNOCC, EVENT_MAINT, STATE_MAINT},
{STATE_OCC, EVENT_UNOCC_DET, STATE_UNOCC},
{STATE_OCC, EVENT_MAINT, STATE_MAINT},
{STATE_MAINT, EVENT_ONLINE, STATE_UNOCC}, //"online" meaning "not under maintenance". Brings stall back to unoccupied because that it has just been under maintenance implies that it's unoccupied. 

{STATE_UNOCC, EVENT_NONE, STATE_UNOCC},
{STATE_OCC, EVENT_NONE, STATE_OCC},
{STATE_MAINT, EVENT_NONE, STATE_MAINT}
};
*/



// FUNCTIONS

//void state_machine_run(void); //starts state machine*/


