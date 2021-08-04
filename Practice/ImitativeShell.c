#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#define RAW_CMD_SIZE 256

// 操作类型
typedef enum Op_tag {
    OP_EXIT,
    OP_HELP,
    OP_CD,
    OP_EXEC,
    OP_ERROR,
    OP_EMPTY,
} Op;
// 指令结构体
typedef struct Cmd_tag {
    char **args;
    Op op;
} Cmd;
// 节点结构体
typedef struct Node_tag {
    char * ptr;
    struct Node_tag * next;
} Node;

// 将命令行字符串预处理解析为 Cmd 结构体
Cmd * preprocess_cmd(char *);
// 处理命令行字符串
void handle_raw_cmd(char *);
// 处理 Cmd 指令
void handle_cmd(Cmd *, int, int);
// 执行内建指令
void exec_buildin_cmd(Cmd *);
// 执行外部指令
void exec_outer_cmd(Cmd *);
// 释放 Cmd 结构体内存
void free_cmd(Cmd *);
// 输出提示
void prompt();

int main () {
    char raw_cmd[RAW_CMD_SIZE];
    while (1) {
        prompt();
        memset(raw_cmd, 0, sizeof(raw_cmd));
        fgets(raw_cmd, RAW_CMD_SIZE, stdin);
        fflush(stdin);
        handle_raw_cmd(raw_cmd);
    }
    return 0;
}

Cmd * preprocess_cmd(char * raw_cmd) {
    int i = 0, count = 0;
    Cmd *pCmd = (Cmd *)calloc(1, sizeof(Cmd));
    Node *head = (Node *)calloc(1, sizeof(Node));
    Node *pNode = head;
    // 将命令行字符串的 \n 修改为 \0
    char * enter_ptr = strstr(raw_cmd, "\n");
    if (enter_ptr) *enter_ptr = '\0';
    // 通过空格分割以解析参数列表
    char *token = strtok(raw_cmd, " ");
    char *first_arg;
    while (token != NULL) {
        pNode->ptr = token;
        pNode->next = (Node *)calloc(1, sizeof(Node));
        pNode = pNode->next;
        count++;
        token = strtok(NULL, " ");
    }
    // 经过解析，存在参数列表
    if (count > 0) {
        // 创建 Cmd 的参数列表 args
        pCmd->args = (char **)malloc((count + 1) * sizeof(char *));
        for (i = 0; i < count && head; i++) {
            pCmd->args[i] = head->ptr;
            pNode = head->next;
            free(head);
            head = pNode;
        }
        pCmd->args[i] = NULL;
        first_arg = pCmd->args[0];
        // 判断 Cmd 对应的操作类型
        if (strcmp(first_arg, "help") == 0) {
            pCmd->op = OP_HELP;
        } else if (strcmp(first_arg, "exit") == 0) {
            pCmd->op = OP_EXIT;
        } else if (strcmp(first_arg, "cd") == 0) {
            pCmd->op = OP_CD;
        } else {
            pCmd->op = OP_ERROR;
            if (!access(first_arg, F_OK) && !access(first_arg, X_OK)) {
                pCmd->op = OP_EXEC;
            } else {
                char check_cmd[RAW_CMD_SIZE << 1] = "which ";
                strcat(check_cmd, first_arg);
                strcat(check_cmd, " >> nul");
                int ret = system(check_cmd);
                if (!access("nul", F_OK))
                    remove("nul");
                if (!ret)
                    pCmd->op = OP_EXEC;
            }
        }
    } else {
        pCmd->op = OP_EMPTY;
    }
    // 返回指向 Cmd 的指针
    return pCmd;
}

void handle_raw_cmd(char * raw_cmd) {
    Cmd * pCmd = NULL;
    char * next_raw_cmd = NULL;
    char * redirect_right_1_ptr = NULL;
    char * redirect_right_2_ptr = NULL;
    char * pipe_ptr = NULL;
    // 判断是否有重定向
    if (redirect_right_1_ptr = strstr(raw_cmd, ">")) {
        int redirect_type = 0;
        // 判断重定向模式
        if (redirect_right_2_ptr = strstr(raw_cmd, ">>")) redirect_type = 1;
        char * redirect_right_ptr = redirect_type ? redirect_right_2_ptr : redirect_right_1_ptr;
        Cmd * next_pCmd = NULL;
        // 分割命令
        *(redirect_right_ptr) = '\0';
        if (redirect_type) *(++redirect_right_ptr) = '\0';
        next_raw_cmd = redirect_right_ptr + 1;
        // 判断重定向是否给定文件名
        if (!(*next_raw_cmd)) {
            printf("Error: redirection needs a file.\n");
        } else {
            // 将两个命令转换为 Cmd 结构体
            pCmd = preprocess_cmd(raw_cmd);
            next_pCmd = preprocess_cmd(next_raw_cmd);
            // 进一步判断重定向是否给定文件名
            if (next_pCmd->args) {
                // 通过重定向类型确定文件打开类型
                int fd = open(next_pCmd->args[0], O_CREAT | O_WRONLY | (redirect_type ? O_APPEND : O_TRUNC), 0644);
                if (fd >= 0) {
                    // 处理命令
                    handle_cmd(pCmd, -1, fd);
                } else {
                    printf("Error: can not open file '%s'.\n", next_pCmd->args[0]);
                }
            } else {
                printf("Error: redirection needs a file.\n");
            }
        }
    } else {
        // 处理命令
        pCmd = preprocess_cmd(raw_cmd);
        handle_cmd(pCmd, -1, -1);
    }
}

void handle_cmd(Cmd * pCmd, int ifd, int ofd) {
    int ist, ost;
    // 复制存储更新输入输出文件描述符
    if (ifd >= 0) {
        ist = dup(0);
        dup2(ifd, 0);
    }
    if (ofd >= 0) {
        ost = dup(1);
        dup2(ofd, 1);
    }
    // 根据不同操作类型执行命令
    if (pCmd->op == OP_HELP || pCmd->op == OP_EXIT || pCmd->op == OP_CD) {
        exec_buildin_cmd(pCmd);
    } else if (pCmd->op == OP_EXEC) {
        exec_outer_cmd(pCmd);
    } else if (pCmd->op == OP_EMPTY) {
        free_cmd(pCmd);
    } else if (pCmd->op == OP_ERROR) {
        printf("'%s': can not find this file or execute this command.\n", pCmd->args[0]);
        free_cmd(pCmd);
    }
    // 还原输入输出文件描述符
    if (ifd >= 0) {
        dup2(ist, 0);
    }
    if (ofd >= 0) {
        dup2(ost, 1);
    }
}

void exec_buildin_cmd(Cmd * pCmd) {
    if (pCmd->op == OP_HELP) {
        printf(
            "\n"
            "help                               to see help for using program.\n"
            "exit                               to exit program.\n"
            "cd [path]                          to change the current working directory.\n"
            "[cmd] [argv]                       to execute some kinds of commands.\n"
            "[cmd] [argv] > [file]              to redirect the output of the left command to a file.\n"
            "[cmd] [argv] >> [file]             to redirect the output of the left command to a file (append mode).\n"
            "\n"
        );
    } else if (pCmd->op == OP_EXIT) {
        free_cmd(pCmd);
        exit(0);
    } else if (pCmd->op == OP_CD) {
        if (pCmd->args[1]) {
            if (chdir(pCmd->args[1])) perror("cd");
        } else {
            struct passwd * pw_ptr = getpwuid(getuid());
            char path[RAW_CMD_SIZE] = { 0 };
            sprintf(path, "/home/%s", pw_ptr->pw_name);
            chdir(path);
        }
    }
}

void exec_outer_cmd(Cmd * pCmd) {
    // 创建子进程
    pid_t pid = fork();
    if (pid < 0) {
        // 创建失败，输出错误信息
        perror("fork");
        free_cmd(pCmd);
    } else if (pid == 0) {
        // 执行命令
        exit(execvp(pCmd->args[0], pCmd->args));
    } else {
        // 等待子进程结束后释放 Cmd 结构体内存
        wait(NULL);
        free_cmd(pCmd);
    }
}

void free_cmd(Cmd * pCmd) {
    if (pCmd) {
        if (pCmd->args)
            free(pCmd->args);
        free(pCmd);
    }
}

void prompt() {
    struct passwd * pw_ptr = getpwuid(getuid());
    char user_type = getuid() == 0 ? '#' : '$';
    char cwd[RAW_CMD_SIZE] = { 0 };
    getcwd(cwd, sizeof(cwd));
    printf("\001\e[1;32m\002%s@ubuntu\001\e[0m\002:\001\e[1;31m\002%s\001\e[0m\002%c ", pw_ptr->pw_name, cwd, user_type);
}
