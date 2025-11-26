


typedef enum {
  STATE_UNOCC,
  STATE_OCC,
  STATE_MAINT
} state_e; //enumerate states, then give then the type state_e


typedef enum{
  EVENT_OCC_DET,
  EVENT_UNOCC_DET,
  EVENT_MAINT,
  EVENT_ONLINE
} event_e; 