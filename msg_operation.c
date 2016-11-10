//
// Created by yutao on 16-10-31.
//

#include "msg_operation.h"
#include "string.h"
int msg_del(struct msg_wrapper *msg){
    memset(msg->text,0,BUFFER_SIZE);
    msg->type=1;
    return 1;
}
int msg_add(struct msg_wrapper *msg, char *string) {
    strcat(msg->text," ");
    strcat(msg->text, string);
    return 1;
}
int msg_cmd(struct msg_wrapper *msg, char *string){
    strcat(msg->text,string);
    return 1;
}
int msg_end(struct msg_wrapper *msg){
    strcat(msg->text,"\0");
}