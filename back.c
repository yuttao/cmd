//
// Created by yutao on 16-10-31.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include "msg_operation.h"
#include <errno.h>
#include <sys/msg.h>


void main() {
    int running = 1;
    int msg_type = 0;
    FILE *pipe;
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE], *str;
    struct msg_wrapper data;
    int filedsc, msgid;

    //创建消息队列
    msgid = msgget((key_t) MESSAGE_ID, 0666 | IPC_CREAT);
    if (-1 == msgid) {
        perror("create message queue");
        exit(errno);
    }
    //阻塞写方式打开FIFO文件
    filedsc = open(FIFO_NAME, O_WRONLY);
    if(-1==filedsc){
        perror("back open FIFO");
        exit(errno);
    }
    while (running) {
        //接受消息
        msg_del(&data);
        msgrcv(msgid, (void *) &data, BUFFER_SIZE, msg_type, 1);
        if (0 == strcmp(data.text, "exit")) {
            running = 0;
            continue;
        } else if (0 == strncmp(data.text, "cd", 2)) {
            strtok(data.text, " ");
            str = strtok(NULL, "\n ");
            printf("%s\n", str);
            if (chdir(str) == -1) {
                perror("change directory");
            }
            strcpy(data.text, "pwd");
        }
        //执行命令，读取结果，写入FIFO
        pipe = popen(data.text, "r");
        memset(buffer, 0, BUFFER_SIZE);
        while (fgets(temp, BUFFER_SIZE, pipe) != NULL) {
            strcat(buffer, temp);
        }
        pclose(pipe);
        write(filedsc, buffer, BUFFER_SIZE);

    }
    //关闭FIFO文件
    if (-1 == close(filedsc)) {
        perror("background fifo close");
        exit(errno);
    }
    //用于显示前后台退出的顺序
    printf("background exited\n");
}
