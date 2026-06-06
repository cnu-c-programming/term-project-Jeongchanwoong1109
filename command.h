#ifndef COMMAND_H
#define COMMAND_H

#include "student.h"

typedef enum {
    SHELL_OK = 0,
    SHELL_EXIT,
    SHELL_ERR_UNKNOWN_COMMAND,
    SHELL_ERR_INVALID_ARGUMENT,
    SHELL_ERR_MISSING_ARGUMENT,
    SHELL_ERR_FILE_OPEN,
    SHELL_ERR_FILE_WRITE,
    SHELL_ERR_STUDENT_NOT_FOUND,
    SHELL_ERR_DUPLICATE_STUDENT,
    SHELL_ERR_INVALID_SCORE
} ShellResult;

typedef struct {
    Student** head;       /* 연결 리스트 헤드 포인터 */
    const char* csv_file; /* CSV 파일 경로 */
} ShellContext;

typedef ShellResult (*CommandHandler)(char* args, ShellContext* ctx);

typedef struct {
    const char*    name;
    CommandHandler handler;
    const char*    usage;
    const char*    description;
} Command;

ShellResult handle_list(char* args, ShellContext* ctx);
ShellResult handle_find(char* args, ShellContext* ctx);
ShellResult handle_stats(char* args, ShellContext* ctx);
ShellResult handle_help(char* args, ShellContext* ctx);
ShellResult handle_clear(char* args, ShellContext* ctx);
ShellResult handle_exit(char* args, ShellContext* ctx);
ShellResult handle_reload(char* args, ShellContext* ctx);

#ifdef ADMIN_MODE
ShellResult handle_add(char* args, ShellContext* ctx);
ShellResult handle_delete(char* args, ShellContext* ctx);
ShellResult handle_update(char* args, ShellContext* ctx);
ShellResult handle_save(char* args, ShellContext* ctx);
ShellResult handle_sort(char* args, ShellContext* ctx);
#endif

/* 입력 줄을 파싱해 해당 핸들러를 호출 */
ShellResult run_command(const char* line, ShellContext* ctx);

/* 명령어 테이블 크기 반환 */
int command_table_size(void);

/* 명령어 테이블 포인터 반환 */
const Command* command_table(void);

#endif /* COMMAND_H */
