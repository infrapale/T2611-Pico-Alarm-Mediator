#ifndef __MSG_H__
#define __MSG_H__

#define MSG_MAX_FIELDS          20
#define MSG_MAX_FIELD_LEN       16
#define MSG_MAX_RAW_MSG_LEN     (60)


typedef struct 
{
    char        raw[MSG_MAX_RAW_MSG_LEN];
    char        fields[MSG_MAX_FIELDS][MSG_MAX_FIELD_LEN];
    uint16_t    len;
    uint8_t     field_count;
} msg_st;

void msg_initialize(void);

uint8_t msg_process(char *msg_inp, bool verbose);

bool msg_is_valid_char(char c);

uint32_t msg_robust_atoi(const char *s, uint8_t *err_cntr, int min, int max );

uint8_t msg_split(char *msg_inp,  char separator ); 

void msg_sub_print(void);

void msg_time_action(void);


#endif