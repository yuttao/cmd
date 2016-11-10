//
// Created by yutao on 16-10-31.
//

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "msg_operation.h"
#include <wait.h>

void main() {
    int flag = 1;
    int status = -1;
    int msgid, filedsc;
    char buffer[BUFFER_SIZE];
    char *command = NULL;
    struct msg_wrapper data;
    char *stack[100];
    int top, i;
    pid_t fpid;
    fpid = fork();
    if (fpid == 0) {
        execl("./back", "back", "", NULL);
        perror("background");
        exit(errno);
    } else if (fpid < 0) {
        perror("fork");
        exit(errno);
    }
    //创建消息队列
    msgid = msgget((key_t) MESSAGE_ID, 0666 | IPC_CREAT);
    if (-1 == msgid) {
        perror("create message queue");
        exit(errno);
    }
    //创建FIFO并打开
    if (0 != mkfifo(FIFO_NAME, 0777)) {
        perror("fifo");
        exit(errno);
    }

    filedsc = open(FIFO_NAME, O_RDONLY);
    if(-1==filedsc){
        perror("front open FIFO");
        exit(errno);
    }
    while (flag) {
        printf(">>");
        msg_del(&data);
        //读入标准输入
        fgets(buffer, BUFFER_SIZE, stdin);
        //将输入拆分为各个字符串
        top = -1;
        command = strtok(buffer, " \n");
        while (command != NULL) {
            stack[++top] = command;
            command = strtok(NULL, " \n");
        }
        if (top < 0) {
            continue;
        }
        command = stack[0];
        //转化命令
        if (0 == strcmp(command, "dir")) {
            msg_cmd(&data, "ls");
        } else if (0 == strcmp(command, "rename")) {
            msg_cmd(&data, "mv");
        } else if (0 == strcmp(command, "move")) {
            msg_cmd(&data, "mv");
        } else if (0 == strcmp(command, "del")) {
            msg_cmd(&data, "rm");
        } else if (0 == strcmp(command, "touch")) {
            msg_cmd(&data, "touch");
        } else if (0 == strcmp(command, "copy")) {
            msg_cmd(&data, "cp");
        } else if (0 == strcmp(command, "md")) {
            msg_cmd(&data, "mkdir");
        }else if (0 == strcmp(command, "cd")) {
            if (top == 0) {
                msg_cmd(&data, "pwd");
            } else {
                msg_cmd(&data, "cd");
            }
        } else if (0 == strcmp(command, "exit")) {
            flag = 0;
            msg_cmd(&data, "exit");
        } else {
            continue;
        }
        if (flag) {
            //合成指令 
            for (i = 1; i <= top; i++) {
                msg_add(&data, stack[i]);
            }
            msg_end(&data);
        }
        //发送消息
        if (-1 == msgsnd(msgid, (void *) &data, BUFFER_SIZE, 0)) {
            perror("send message");
            exit(errno);
        }
        if (flag) {
        //打开并阻塞读取FIFO数据
            read(filedsc, buffer, BUFFER_SIZE);
            printf("%s", buffer);
        }
    }
    //删除消息队列
    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        perror("delete msg");
        exit(errno);
    }
    //关闭，删除FIFO文件
    if(-1==close(filedsc)){
        perror("front close FIFO");
        exit(errno);
    }
    unlink(FIFO_NAME);
    //等待后台进程退出
    wait(&status);
    //用于显示前后台退出的顺序
    printf("foreground exited\n");
}
