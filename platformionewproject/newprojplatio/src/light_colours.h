#ifndef LIGHT_COLOURS.H
#define LIGHT_COLOURS.H
void yellow_light(double Tms, double duty, int red, int green);

void green_light(double Tms, double duty, int green);

void red_light(double Tms, double duty, int red);

void state_unocc_enter(data, from, event);

//TODO data structs for the state data to pass into the enter functions

#endif
