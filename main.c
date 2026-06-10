#include "student.h"
#include "file_io.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_prompt(void) {
#ifdef ADMIN_MODE
    printf("admin> ");
#else
    printf("client> ");
#endif
    fflush(stdout);
}

static void interactive_loop(ShellContext* ctx) {
    char line[512];
    while (1) {
        print_prompt();
        if (!fgets(line, sizeof(line), stdin)) break; /* EOF */
        line[strcspn(line, "\r\n")] = '\0';
        ShellResult res = run_command(line, ctx);
        if (res == SHELL_EXIT) break;
    }
}

static void run_command_file(const char* filepath, ShellContext* ctx) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open command file '%s'.\n", filepath);
        interactive_loop(ctx);
        return;
    }

    char line[512];
    int  cmd_no  = 0;
    int  exited  = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') continue;

        cmd_no++;
        printf("[command file:%d] %s\n", cmd_no, line);
        fflush(stdout);

        ShellResult res = run_command(line, ctx);
        if (res == SHELL_EXIT) {
            exited = 1;
            break;
        }
        if (res != SHELL_OK) {
            fprintf(stderr, "Skipped line %d.\n", cmd_no);
        }
    }

    fclose(fp);
    if (!exited) {
        interactive_loop(ctx);
    }
}

static void print_usage(const char* prog) {
    fprintf(stderr, "Usage: %s <csv_file> [-f command_file]\n", prog);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    const char* csv_file = NULL;
    const char* cmd_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -f option requires a filename.\n");
                print_usage(argv[0]);
                return 1;
            }
            cmd_file = argv[++i];
        } else {
            if (csv_file) {
                fprintf(stderr, "Error: unexpected argument '%s'.\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
            csv_file = argv[i];
        }
    }

    if (!csv_file) {
        print_usage(argv[0]);
        return 1;
    }

#ifdef ADMIN_MODE
    printf("[Admin Program]\n");
#else
    printf("[Client Program]\n");
#endif
    Student* head = NULL;
    int loaded = load_students(csv_file, &head);
    if (loaded < 0) {
        return 1;
    }
    printf("Loaded %d students from %s.\n", loaded, csv_file);
    ShellContext ctx;
    ctx.head     = &head;
    ctx.csv_file = csv_file;
    if (cmd_file) {
        run_command_file(cmd_file, &ctx);
    } else {
        interactive_loop(&ctx);
    }
    student_free_all(&head);
    return 0;
}
