

//state machine data
struct state_machine_data
{
  state_e state;
  struct common_state_data state_data;
};



//state transistion struct
struct state_transistion
{
  state_e from;
  event_e event;
  state_e to;
};


//state transistion table (array of the state transistion struct type we just made above)
static const struct state_transistion state_transistions[] = {

//current state, event, new state
{STATE_UNOCC, EVENT_OCC_DET, STATE_OCC}, 
{STATE_UNOCC, EVENT_MAINT, STATE_MAINT},
{STATE_OCC, EVENT_UNOCC_DET, STATE_UNOCC},
{STATE_OCC, EVENT_MAINT, STATE_MAINT},
{STATE_MAINT, EVENT_ONLINE, STATE_UNOCC}, //"online" meaning detecting occupancy. Brings stall back to unoccupied because that it has just been under maintenance implies that it's unoccupied. 

};
