//#include "state_machine.h"
//#include "light_colours.h" //we don't need this right now

#include "Arduino.h"

// Based on video tutorial https://www.youtube.com/watch?v=NTEHRjiAY2I&t=1220s 
//https://github.com/artfulbytes/nsumo_video/tree/main/src/app

//light PWM stuff based on random nerd tutorials: https://randomnerdtutorials.com/esp32-pwm-arduino-ide/#analogwrite 

int red; //pins
int green;

//duty cycle 
uint8_t duty; //0 to 255
uint8_t off = 0;

//funtion that enters new state
static void state_enter(struct state_transition transition) {
    //we will have functions for entering each state
    //Analog writing from here directly would be weird?
    switch(transition.to){
        case STATE_UNOCC:
            analogWrite(green, duty);
            analogWrite(red, off);
            break;
        case STATE_OCC:
            analogWrite(red, duty);
            analogWrite(green, off);
            break;
        case STATE_MAINT:
            analogWrite(red, duty);
            analogWrite(green, duty);
            break;
    }
}


//function that takes an event and goes through the table to find the right transition
void process_event(state_e from, event_e next_event) {
    for (uint16_t i = 0; i<sizeof(state_transitions); i++) { //ARRAY SIZE was a function he made or it's not in cpp TODO: find a function that works
        if(from == state_transitions[i].from && next_event == state_transitions[i].event) {
            state_enter(state_transitions[i]);
            return;
        }
    } 
    assert(0);
}

event_e process_input() { //input TBD
    //input
    //if statements for input - turn into events to be created
    event_e event;

    //RETURNS: an event
    return event;
}

//FIXME: how do pointers work???
void state_machine_init(struct state_transition *inital_state) { //inital transistion is unocc and event none
    inital_state -> from = STATE_UNOCC;
    inital_state -> event = EVENT_NONE;
    inital_state -> to = STATE_UNOCC;
}


void state_machine_run(void) {
    //allocate and initialize data
    struct state_transition *inital_state;
    state_machine_init(inital_state); 

    while(1) {
       
        
        process_event(//state transition ); 
    }

    //input to process input will be a json packet via ethernet 
    //process input needs to return a state transition struct
    //process event needs to take that struct and do the state change

}

