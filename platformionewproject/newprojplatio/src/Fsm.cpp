



//state_common header
//define the common_state_data struct
//I wonder if we need a struct just to hold one pointer, in the video there was a lot more

struct common_state_data 
{
  struct state_machine_data *state_machine_data;
};



//state data: just the light colors file that I already have

//state machine run - implemented by the setup and loop arduino framework - I think?