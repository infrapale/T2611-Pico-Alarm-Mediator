
#include "relay.h"
#include "main.h"
#include "io.h"
#include "r69.h"

typedef enum
{
    RELAY_ACTION_UNDEF = 0,
    RELAY_ACTION_NEXT,
    RELAY_ACTION_SWITCH_ON,
    RELAY_ACTION_SWITCH_OFF,
    RELAY_ACTION_SEND
}   relay_action_et;

typedef struct
{
    char        buff[R69_MSG_SIZE];
    uint8_t     indx;
    uint32_t    timeout;
    relay_action_et action;
} relay_ctrl_st;

relay_ctrl_st relay_ctrl;

const int16_t relay_groups[VA_RELAY_GROUP_NBR_OF] =
{
    [VA_RELAY_GROUP_ALL]      = RELAY_GR_BIT_ALL,
    [VA_RELAY_GROUP_ARRIVE]   = RELAY_GR_BIT_ARRIVE,
    [VA_RELAY_GROUP_TUPA]     = RELAY_GR_BIT_TUPA,
    [VA_RELAY_GROUP_MH1]      = RELAY_GR_BIT_MH1,
    [VA_RELAY_GROUP_MH2]      = RELAY_GR_BIT_MH2,
    [VA_RELAY_GROUP_K]        = RELAY_GR_BIT_K,
    [VA_RELAY_GROUP_KHH]      = RELAY_GR_BIT_KHH,
    [VA_RELAY_GROUP_ULKO]     = RELAY_GR_BIT_ULKO,
    [VA_RELAY_GROUP_WC]       = RELAY_GR_BIT_WC,
    [VA_RELAY_GROUP_ET]       = RELAY_GR_BIT_ET,
};


relay_addr_st relay_addr[VA_RELAY_NBR_OF] =
{   //                        Unit   Relay   S  T  Group Bitmap 
    [VA_RELAY_UNDEF] =      {"XXX", "XXXXX", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE},
    [VA_RELAY_TK] =         {"TK1", "RTK__", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE  | RELAY_GR_BIT_ET },
    [VA_RELAY_ET] =         {"TK1", "RET_1", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_ET },
    [VA_RELAY_WC_1] =       {"TK1", "RWC_1", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE},
    [VA_RELAY_WC_2] =       {"MH2", "RWC_2", 0, 0, RELAY_GR_BIT_ALL},
    [VA_RELAY_TUPA_1] =     {"TK1", "RTUP1", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_TUPA},
    [VA_RELAY_TUPA_2] =     {"TK1", "RTUP2", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_TUPA},
    [VA_RELAY_MH1_1] =      {"MH1", "RMH11", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_MH1},
    [VA_RELAY_MH1_2] =      {"MH1", "RMH12", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_MH1},
    [VA_RELAY_MH1_3] =      {"MH1", "RMH13", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_MH1},
    [VA_RELAY_MH1_4] =      {"MH1", "RMH14", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_MH1},
    [VA_RELAY_MH1_5] =      {"MH1", "RMH15", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE},
    [VA_RELAY_MH2_1] =      {"MH2", "RMH21", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_MH2},
    [VA_RELAY_MH2_2] =      {"MH2", "RMH22", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_MH2},
    [VA_RELAY_KEITTIO_1] =  {"MH1", "RKOK1", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_K},
    [VA_RELAY_KEITTIO_2] =  {"MH2", "RKOK2", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_K},
    [VA_RELAY_KEITTIO_3] =  {"MH2", "RKOK3", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_K},
    [VA_RELAY_KEITTIO_4] =  {"MH2", "RKOK4", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_K},
    [VA_RELAY_KEITTIO_5] =  {"MH2", "RKOK5", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE},
    [VA_RELAY_KHH_1] =      {"TK1", "RKHH1", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_KHH},
    [VA_RELAY_KHH_2] =      {"MH2", "RKHH2", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_KHH},
    [VA_RELAY_PSH] =        {"MH2", "RPSH_", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_KHH},
    [VA_RELAY_SAUNA] =      {"TK1", "RSAUN", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_KHH},
    [VA_RELAY_PARVI] =      {"TK1", "RPARV", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE },
    [VA_RELAY_PIHA] =       {"TK1", "RPIHA", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ARRIVE | RELAY_GR_BIT_ULKO},
    [VA_RELAY_TERASSI] =    {"TK1", "RTERA", 0, 0, RELAY_GR_BIT_ALL | RELAY_GR_BIT_ULKO},
    [VA_RELAY_POLKU] =      {"TK1", "RPOLK", 0, 0, RELAY_GR_BIT_ALL },
};


void relay_task(void);

// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st relay_th       = {"Clock Task     ", 1000,    0,     0,  255,    0,   1, relay_task};

void relay_initialize(void)
{
    atask_add_new(&relay_th);
}

void relay_send_one(va_relays_et rindx, char value)
{
    // <#X1N:RMH1;RKOK1;T;->\n  
    // {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
    sprintf(relay_ctrl.buff,"<#X1N:%s;%s;%c;->\n", relay_addr[rindx].unit,relay_addr[rindx].relay, value);

    Serial.printf("#%d: %s %s %c\n", rindx, relay_addr[rindx].unit,relay_addr[rindx].relay, value);
    Serial.printf(relay_ctrl.buff);
}

// relay_activate( event[ev].relay[i].indx, event[ev].relay[i].duration_sec );
void relay_activate( va_relays_et indx, uint16_t duration_sec )
{
    switch(relay_addr[indx].state)
    {
        case RELAY_STATE_IDLE:
            relay_addr[indx].state = RELAY_STATE_ACTIVATE;
            relay_addr[indx].timeout = duration_sec * 1000;
            break;
        case RELAY_STATE_ACTIVATE:
            break;
        case RELAY_STATE_ACTIV:
            // relay_addr[indx].state = RELAY_STATE_ ;
            relay_addr[indx].timeout = duration_sec * 1000;
            break;
    } 
}



void relay_task(void)
{
    switch(relay_th.state)
    {
        case 0:
            relay_ctrl.indx = VA_RELAY_UNDEF + 1;
            relay_th.state = 10;
            break;
        case 10:
            switch(relay_addr[relay_ctrl.indx].state)
            {
                case RELAY_STATE_IDLE:
                    relay_ctrl.action = RELAY_ACTION_NEXT;
                    break;
                case RELAY_STATE_ACTIVATE:
                    if(r69_ready_to_send()) {
                        relay_ctrl.action = RELAY_ACTION_SWITCH_ON;
                        relay_ctrl.timeout = millis() + 2000;
                        relay_th.state = 100;
                    }
                    break;
                case RELAY_STATE_ACTIV:
                    // relay_addr[indx].state = RELAY_STATE_ ;
                    if(millis() > relay_addr[relay_ctrl.indx].timeout){
                        if(r69_ready_to_send()) {
                            relay_ctrl.action = RELAY_ACTION_SWITCH_OFF;
                            relay_ctrl.timeout = millis() + 2000;
                            relay_th.state = 100;
                        }
                    }
                    else
                        relay_ctrl.action = RELAY_ACTION_NEXT;
                    break;
            } 
            break;
        case 100:
            if( millis() > relay_ctrl.timeout)  relay_th.state = 10;
            break;
    }

    while(relay_ctrl.action != RELAY_ACTION_UNDEF)
    {
        switch(relay_ctrl.action)
        {
            case RELAY_ACTION_UNDEF:
                break;
            case RELAY_ACTION_NEXT:
                if(++relay_ctrl.indx >= VA_RELAY_NBR_OF) relay_ctrl.indx = 0;
                relay_ctrl.action = RELAY_ACTION_UNDEF;
                break;
            case RELAY_ACTION_SWITCH_ON:
                relay_send_one((va_relays_et)relay_ctrl.indx, '1');
                relay_ctrl.action = RELAY_ACTION_NEXT;
                break;
            case RELAY_ACTION_SWITCH_OFF:
                relay_send_one((va_relays_et)relay_ctrl.indx, '0');
                relay_ctrl.action = RELAY_ACTION_NEXT;
                break;

        }
    }
}

const relay_addr_st *relay_get_addr_ptr( va_relays_et relay_id)
{
    return &relay_addr[relay_id];
}

bool relay_get_is_relay_in_group(va_relays_et rindx, uint8_t gindx )
{
    return ((relay_groups[gindx] & relay_addr[rindx].group_map) != 0);
}

