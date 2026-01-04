#include "state_machine.h"
#include "state_common.h"
#include <cassert>

// Based on video tutorial https://www.youtube.com/watch?v=NTEHRjiAY2I&t=1220s 


//state transition struct
struct state_transition
{
  state_e from;
  event_e event;
  state_e to;
};


//state transition table (array of the state transition struct type we just made above)
static const struct state_transition state_transitions[] = {
//current state, event, new state
{STATE_UNOCC, EVENT_OCC_DET, STATE_OCC}, 
{STATE_UNOCC, EVENT_MAINT, STATE_MAINT},
{STATE_OCC, EVENT_UNOCC_DET, STATE_UNOCC},
{STATE_OCC, EVENT_MAINT, STATE_MAINT},
{STATE_MAINT, EVENT_ONLINE, STATE_UNOCC}, //"online" meaning "not under maintenance". Brings stall back to unoccupied because that it has just been under maintenance implies that it's unoccupied. 
};

//state machine data //is this supposed to be in state_common.h?
struct state_machine_data
{
  state_e state;
  struct common_state_data state_data;
  event_e event;
};





//funtion that enters new state
static void state_enter(struct state_machine_data *data, state_e from, event_e event, state_e to) {
    //we will have functions for entering each state
    switch(to){
        case STATE_UNOCC:
            state_unocc_enter(&(data->unocc), from, event);
            break;
        case STATE_OCC:
            state_occ_enter(&(data->occ), from, event);
            break;
        case STATE_MAINT:
            state_maint_enter(&(data->maint), from, event);
            break;
    }
}


//function that takes an event and goes through the table to find the right transition
static inline void process_event(struct state_machine_data *data, event_e next_event) {
    for (uint16_t i = 0; i<sizeof(state_transitions); i++) { //ARRAY SIZE was a function he made or it's not in cpp TODO: find a function that works
        if(data -> state == state_transitions[i].from && next_event == state_transitions[i].event) {
            state_enter(data, state_transitions[i].from, next_event, state_transitions[i].to);
            return;
        }
    } 
    assert(0);
}

static inline void process_input(struct state_machine_data *data) {
    //input
    //if statements for input - turn into events to be created
    //internal events
    return EVENT_NONE; 
}



static inline void state_machine_init(struct state_machine_data *data) {
    data -> STATE_UNOCC;  //this needs work and review (26:24)
    data -> state_machine_data = data;
    //internal event (I need the none internal event)
}

void state_machine_run(void) {
    //allocate and initialize data
    struct state_machine_data data;

    state_machine_init(&data); 

    while(1) {
        const event_e event = process_input(&data);
        process_event(&data, event); //changed from "next_event" - I think he just named the var wrong here
    }



}

