//
// Created by yutao on 16-10-31.
//

#ifndef CMD_MESSAGE_H
#define CMD_MESSAGE_H
#endif //CMD_MESSAGE_H


#define BUFFER_SIZE 4096
#define MESSAGE_ID 973837
#define FIFO_NAME "/tmp/mine10_fifo"

struct msg_wrapper{
    long int type;
    char text[BUFFER_SIZE];
};

int msg_del(struct msg_wrapper *msg);
int msg_add(struct msg_wrapper *msg, char *string);
int msg_cmd(struct msg_wrapper *msg, char *string);
int msg_end(struct msg_wrapper *msg);
