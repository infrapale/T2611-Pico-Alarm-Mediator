#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <Arduino.h>
#include <time.h>
#include "main.h"
#include "msg.h"
#include "action.h"
#include "dingdong.h"
#include "atask.h"

#define    EVENT_NAME_LEN   10

typedef enum
{
    ACTION_UNDEFINED = 0,
    ACTION_SHORT_BEEP, 
    ACTION_MEDIUM_BEEP, 
    ACTION_LONG_BEEP,
    ACTION_BEEP_400HZ,
    ACTION_BEEP_600HZ,
    ACTION_BEEP_800HZ,
    ACTION_BEEP_1000HZ,
    ACTION_BEEP_400_405HZ,
    ACTION_BEEP_300_301HZ,
    ACTION_BEEP_440HZ,
    ACTION_X,
    ACTION_BIG_BEN, 
    ACTION_RING, 
    ACTION_NBR_OF
} action_indx_et;

typedef enum
{
    ALARM_OFF = 0,
    ALARM_ALERT_LOW,
    ALARM_ALERT_HIGH,
    ALARM_WARNING_1,
    ALARM_WARNING_2,
    ALARM_WARNING_3,
    ALARM_PII_PAA,
    ALARM_NBR_OF
} alarm_indx_et;


typedef struct
{
    // action_indx_et  action[HOME_NBR_OF];
    uint32_t duration;
    float freq1;
    float freq2;
    float freq3;
} tone_param_st;

typedef struct
{
    alarm_indx_et   alarm_indx;
    uint16_t        duration_sec;
} alarm_param_st;


typedef struct 
{
    char            name[EVENT_NAME_LEN];
    uint16_t        counter;
    uint8_t         state;
    alarm_param_st  alarm[ALARM_NBR_OF];
    //tone_param_st   tone[HOME_NBR_OF];
    //action_indx_et  action[HOME_NBR_OF];
} event_st;

typedef struct 
{
    uint8_t nbr_of_events;
    home_state_et home_state;
} action_ctrl_st;


event_st event[] =
{
    {
        .name = "Katos",    
        .counter = 0, 
        .state = 0,
        .alarm =
        { // State              Index               Duration
            [HOME_AWAY]     = {ALARM_WARNING_1,     10},
            [HOME_INDOOR]   = {ALARM_ALERT_LOW,     2},
            [HOME_OUTDOOR]  = {ALARM_ALERT_HIGH,    5},
            [HOME_SLEEP]    = {ALARM_ALERT_LOW,     2},
            [HOME_MUTE]     = {ALARM_OFF,           0},
        }
    },
    {
        .name = "Piha_1",    
        .counter = 0, 
        .state = 0,
        .alarm =
        { // State              Index               Duration
            [HOME_AWAY]     = {ALARM_WARNING_1,     10},
            [HOME_INDOOR]   = {ALARM_ALERT_LOW,     2},
            [HOME_OUTDOOR]  = {ALARM_ALERT_HIGH,    5},
            [HOME_SLEEP]    = {ALARM_ALERT_LOW,     2},
            [HOME_MUTE]     = {ALARM_OFF,           0},
        }
    },
    {
        .name = "Terassi",    
        .counter = 0, 
        .state = 0,
        .alarm =
        { // State              Index               Duration
            [HOME_AWAY]     = {ALARM_WARNING_1,     10},
            [HOME_INDOOR]   = {ALARM_ALERT_LOW,     2},
            [HOME_OUTDOOR]  = {ALARM_ALERT_HIGH,    5},
            [HOME_SLEEP]    = {ALARM_ALERT_LOW,     2},
            [HOME_MUTE]     = {ALARM_OFF,           0},
        }
    },
    {
        .name = "PIR_3",    
        .counter = 0, 
        .state = 0,
        .alarm =
        { // State              Index               Duration
            [HOME_AWAY]     = {ALARM_WARNING_1,     10},
            [HOME_INDOOR]   = {ALARM_ALERT_LOW,     2},
            [HOME_OUTDOOR]  = {ALARM_ALERT_HIGH,    5},
            [HOME_SLEEP]    = {ALARM_ALERT_LOW,     2},
            [HOME_MUTE]     = {ALARM_OFF,           0},
        }
    },
};

tone_param_st   local_tone[ALARM_NBR_OF] =
{

    [ALARM_OFF]         = {0,       800.0,      0.0,        0.0},
    [ALARM_ALERT_LOW]   = {1000,    600.0,      602.0,      0.0},
    [ALARM_ALERT_HIGH]  = {5000,    900.0,      901.0,      10.0},
    [ALARM_WARNING_1]   = {5000,    700.0,      705.0,      2.0},
    [ALARM_WARNING_2]   = {5000,    1000.0,     1001.0,     1.0},
    [ALARM_WARNING_3]   = {5000,    1200.0,     1210.0,     0.0},
    [ALARM_PII_PAA]     = {5000,    300.0,      601.0,      2.0},
};
 


action_ctrl_st action_ctrl = {};

int action_find_event(const char *namep);


void action_initialize(void)
{
    action_ctrl.nbr_of_events = sizeof(event) / sizeof(event[0]);
}

void action_set_home_state(home_state_et home_state)
{
    action_ctrl.home_state = home_state;
}

void action_new_event(char *namep)
{
    int ev = action_find_event(namep);

    if ((ev >= 0) && (ev < action_ctrl.nbr_of_events))
    {
        event[ev].counter++;
        Serial.printf("Event %s found at index %d\n",
                      event[ev].name, ev);
        alarm_indx_et aindx = event[ev].alarm[action_ctrl.home_state].alarm_indx;
        uint32_t duration = event[ev].alarm[action_ctrl.home_state].duration_sec;
        float freq1 = local_tone[aindx].freq1;     
        float freq2 = local_tone[aindx].freq2;     
        float freq3 = local_tone[aindx].freq3;     
        Serial.printf("Tone: %d, %f %f %f\n", duration, freq1, freq2, freq3);    
        dingdong_play_tone(duration, freq1, freq2, freq3);

        // switch(ev)
        // {
        //     case 0:
        //         //dingdong_play_wav(WAV_BIG_BEN);
        //         dingdong_play_tone(2000, 800, 805);
        //         break;
        //     case 1:
        //         //dingdong_play_wav(WAV_SENSOR_BEEP);
        //         dingdong_play_tone(500, 1200, 2000);
        //         break;
        //     case 2:
        //         //dingdong_play_wav(WAV_DIAL_CALL);
        //         dingdong_play_tone(200, 100, 150);
        //         break;
        //     case 3:
        //         // dingdong_play_wav(WAV_PHONE_RINGING);
        //         dingdong_play_tone(5000, 300, 301);
        //         break;
        //     default:
        //         break;
        // }
    }
    else
    {
        Serial.printf("Unknown event %s\n", namep);
    }
}


    // WAV_BIG_BEN = 0,
    // WAV_SENSOR_BEEP,
    // WAV_DIAL_CALL,
    // WAV_OLD_CHURCH,
    // WAV_PHONE_RINGING,
    // WAV_ROTARY_PHONE_HIGH,
    // WAV_ROTARY_PHONE_LOW,

int action_find_event(const char *namep)
{
    for (uint8_t i = 0; i < action_ctrl.nbr_of_events; i++)
    {
        if (strcmp(event[i].name, namep) == 0)
        {
            return i;   // Found
        }
    }

    return -1;          // Not found
}

