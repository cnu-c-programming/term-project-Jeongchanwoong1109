#include "command.h"
#include "file_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* 문자열 앞뒤 공백 제거 */
static void trim(char* s) {
    if (!s) return;
    int start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start) memmove(s, s + start, strlen(s) - start + 1);
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

ShellResult handle_list(char* args, ShellContext* ctx) {
    (void)args;
    Student* head = *(ctx->head);
    if (!head) {
        printf("No students found.\n");
        return SHELL_OK;
    }
    printf("%-6s %-20s %s\n", "ID", "Name", "Score");
    while (head) {
        printf("%-6d %-20s %d\n", head->id, head->name, head->score);
        head = head->next;
    }
    return SHELL_OK;
}

ShellResult handle_find(char* args, ShellContext* ctx) {
    if (!args || strlen(args) == 0) {
        fprintf(stderr, "Error: missing argument. Usage: find <id>\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char* end;
    long id = strtol(args, &end, 10);
    if (*end != '\0' || id <= 0) {
        fprintf(stderr, "Error: invalid ID '%s'.\n", args);
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    Student* s = student_find(*(ctx->head), (int)id);
    if (!s) {
        fprintf(stderr, "Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("ID: %d\nName: %s\nScore: %d\n", s->id, s->name, s->score);
    return SHELL_OK;
}

ShellResult handle_stats(char* args, ShellContext* ctx) {
    (void)args;
    Student* head = *(ctx->head);
    if (!head) {
        printf("No student data available.\n");
        return SHELL_OK;
    }
    int   count = 0;
    int   max   = head->score;
    int   min   = head->score;
    long  sum   = 0;
    Student* cur = head;
    while (cur) {
        count++;
        sum += cur->score;
        if (cur->score > max) max = cur->score;
        if (cur->score < min) min = cur->score;
        cur = cur->next;
    }
    printf("Count: %d\nAverage: %.1f\nMax: %d\nMin: %d\n",
           count, (double)sum / count, max, min);
    return SHELL_OK;
}

ShellResult handle_reload(char* args, ShellContext* ctx) {
    (void)args;
    student_free_all(ctx->head);
    int n = load_students(ctx->csv_file, ctx->head);
    if (n < 0) {
        fprintf(stderr, "Error: failed to reload from %s.\n", ctx->csv_file);
        return SHELL_ERR_FILE_OPEN;
    }
    printf("Reloaded %d students from %s.\n", n, ctx->csv_file);
    return SHELL_OK;
}

ShellResult handle_clear(char* args, ShellContext* ctx) {
    (void)args; (void)ctx;
    printf("\033[2J\033[H");
    fflush(stdout);
    return SHELL_OK;
}

ShellResult handle_exit(char* args, ShellContext* ctx) {
    (void)args; (void)ctx;
    printf("Goodbye.\n");
    return SHELL_EXIT;
}

#ifdef ADMIN_MODE

ShellResult handle_save(char* args, ShellContext* ctx) {
    (void)args;
    int n = save_students(ctx->csv_file, *(ctx->head));
    if (n < 0) {
        fprintf(stderr, "Error: failed to save to %s.\n", ctx->csv_file);
        return SHELL_ERR_FILE_WRITE;
    }
    printf("Saved %d students to %s.\n", n, ctx->csv_file);
    return SHELL_OK;
}

ShellResult handle_add(char* args, ShellContext* ctx) {
    if (!args || strlen(args) == 0) {
        fprintf(stderr, "Error: missing arguments. Usage: add <id> <name> <score>\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    char buf[256];
    strncpy(buf, args, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tok = strtok(buf, " \t");
    if (!tok) {
        fprintf(stderr, "Error: missing id.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char* end;
    long id = strtol(tok, &end, 10);
    if (*end != '\0' || id <= 0) {
        fprintf(stderr, "Error: invalid ID '%s'.\n", tok);
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    tok = strtok(NULL, " \t");
    if (!tok) {
        fprintf(stderr, "Error: missing name.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char name[32];
    strncpy(name, tok, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    if (strlen(name) == 0) {
        fprintf(stderr, "Error: name cannot be empty.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    /* 이름에 쉼표 불가 */
    if (strchr(name, ',')) {
        fprintf(stderr, "Error: name cannot contain comma.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    tok = strtok(NULL, " \t");
    if (!tok) {
        fprintf(stderr, "Error: missing score.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    long score = strtol(tok, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Error: invalid score '%s'.\n", tok);
        return SHELL_ERR_INVALID_SCORE;
    }
    if (score < 0 || score > 100) {
        fprintf(stderr, "Error: score must be 0-100.\n");
        return SHELL_ERR_INVALID_SCORE;
    }

    /* 중복 ID 확인 */
    if (student_find(*(ctx->head), (int)id)) {
        fprintf(stderr, "Error: duplicate ID %ld.\n", id);
        return SHELL_ERR_DUPLICATE_STUDENT;
    }

    Student* s = student_create((int)id, name, (int)score);
    if (!s) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    student_append(ctx->head, s);
    printf("Student added.\n");
    return SHELL_OK;
}

ShellResult handle_delete(char* args, ShellContext* ctx) {
    if (!args || strlen(args) == 0) {
        fprintf(stderr, "Error: missing argument. Usage: delete <id>\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char* end;
    long id = strtol(args, &end, 10);
    if (*end != '\0' || id <= 0) {
        fprintf(stderr, "Error: invalid ID '%s'.\n", args);
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (!student_delete(ctx->head, (int)id)) {
        fprintf(stderr, "Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("Student deleted.\n");
    return SHELL_OK;
}

ShellResult handle_update(char* args, ShellContext* ctx) {
    if (!args || strlen(args) == 0) {
        fprintf(stderr, "Error: missing arguments. Usage: update <id> <score>\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    char buf[128];
    strncpy(buf, args, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tok = strtok(buf, " \t");
    if (!tok) {
        fprintf(stderr, "Error: missing id.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char* end;
    long id = strtol(tok, &end, 10);
    if (*end != '\0' || id <= 0) {
        fprintf(stderr, "Error: invalid ID '%s'.\n", tok);
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    tok = strtok(NULL, " \t");
    if (!tok) {
        fprintf(stderr, "Error: missing score.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    long score = strtol(tok, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Error: invalid score '%s'.\n", tok);
        return SHELL_ERR_INVALID_SCORE;
    }
    if (score < 0 || score > 100) {
        fprintf(stderr, "Error: score must be 0-100.\n");
        return SHELL_ERR_INVALID_SCORE;
    }

    Student* s = student_find(*(ctx->head), (int)id);
    if (!s) {
        fprintf(stderr, "Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    s->score = (int)score;
    printf("Student updated.\n");
    return SHELL_OK;
}

ShellResult handle_sort(char* args, ShellContext* ctx) {
    if (!args || strlen(args) == 0) {
        fprintf(stderr, "Error: missing argument. Usage: sort <name|score>\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    trim(args);
    if (strcmp(args, "name") == 0) {
        student_sort_by_name(ctx->head);
        printf("Sorted: sorted by name.\n");
    } else if (strcmp(args, "score") == 0) {
        student_sort_by_score(ctx->head);
        printf("Sorted by score.\n");
    } else {
        fprintf(stderr, "Error: unknown sort key '%s'. Use 'name' or 'score'.\n", args);
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    return SHELL_OK;
}

#endif /* ADMIN_MODE */

#ifdef ADMIN_MODE
static const Command g_commands[] = {
    {"save",   handle_save,   "save",                   "Save students to CSV"},
    {"reload", handle_reload, "reload",                 "Reload students from CSV"},
    {"add",    handle_add,    "add <id> <name> <score>","Add a student"},
    {"delete", handle_delete, "delete <id>",            "Delete a student"},
    {"update", handle_update, "update <id> <score>",    "Update student score"},
    {"find",   handle_find,   "find <id>",              "Find student by ID"},
    {"list",   handle_list,   "list",                   "List all students"},
    {"stats",  handle_stats,  "stats",                  "Show statistics"},
    {"sort",   handle_sort,   "sort <name|score>",      "Sort students"},
    {"help",   handle_help,   "help",                   "Show help"},
    {"clear",  handle_clear,  "clear",                  "Clear screen"},
    {"exit",   handle_exit,   "exit",                   "Exit program"},
};
#endif

#ifdef CLIENT_MODE
static const Command g_commands[] = {
    {"reload", handle_reload, "reload",    "Reload students from CSV"},
    {"find",   handle_find,   "find <id>", "Find student by ID"},
    {"list",   handle_list,   "list",      "List all students"},
    {"stats",  handle_stats,  "stats",     "Show statistics"},
    {"help",   handle_help,   "help",      "Show help"},
    {"clear",  handle_clear,  "clear",     "Clear screen"},
    {"exit",   handle_exit,   "exit",      "Exit program"},
};
#endif

static const int g_command_count =
    (int)(sizeof(g_commands) / sizeof(g_commands[0]));

int command_table_size(void) { return g_command_count; }
const Command* command_table(void) { return g_commands; }

ShellResult handle_help(char* args, ShellContext* ctx) {
    (void)args; (void)ctx;
    printf("Commands:\n");
    for (int i = 0; i < g_command_count; i++) {
        printf("  %-30s %s\n", g_commands[i].usage, g_commands[i].description);
    }
    return SHELL_OK;
}

ShellResult run_command(const char* line, ShellContext* ctx) {
    if (!line) return SHELL_OK;

    /* 줄 복사 및 앞뒤 공백 제거 */
    char buf[512];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    trim(buf);

    if (strlen(buf) == 0) return SHELL_OK;

    /* 명령어 이름 분리 */
    char* sp = strchr(buf, ' ');
    char* cmd_name;
    char* args;
    if (sp) {
        *sp      = '\0';
        cmd_name = buf;
        args     = sp + 1;
        trim(args);
    } else {
        cmd_name = buf;
        args     = NULL;
    }

    /* 테이블 탐색 */
    for (int i = 0; i < g_command_count; i++) {
        if (strcmp(g_commands[i].name, cmd_name) == 0) {
            return g_commands[i].handler(args, ctx);
        }
    }

    /* 알 수 없는 명령어 */
    printf("Unknown command or permission denied.\n");
    return SHELL_ERR_UNKNOWN_COMMAND;
}
