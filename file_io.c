#include "file_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* 문자열 앞뒤 공백 제거 */
static void trim(char* s) {
    if (!s) return;
    /* 앞 공백 */
    int start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start) memmove(s, s + start, strlen(s) - start + 1);
    /* 뒤 공백 */
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

static int parse_csv_line(const char* line, int* id, char* name, int* score) {
    char buf[256];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tok = strtok(buf, ",");
    if (!tok) return 0;
    trim(tok);
    char* end;
    long lid = strtol(tok, &end, 10);
    if (*end != '\0' || lid <= 0) return 0;
    *id = (int)lid;

    tok = strtok(NULL, ",");
    if (!tok) return 0;
    trim(tok);
    if (strlen(tok) == 0 || strlen(tok) > 31) return 0;
    strncpy(name, tok, 31);
    name[31] = '\0';

    tok = strtok(NULL, ",");
    if (!tok) return 0;
    trim(tok);
    long lscore = strtol(tok, &end, 10);
    if (*end != '\0' || lscore < 0 || lscore > 100) return 0;
    *score = (int)lscore;

    return 1;
}

/* CSV 로드 */
int load_students(const char* filename, Student** head) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        /* 파일이 없으면 빈 리스트로 시작*/
        return 0;
    }

    char line[256];
    int  count = 0;
    int  lineno = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineno++;
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;
        if (lineno == 1) {
            if (strncmp(line, "id,name,score", 13) != 0) {
                fprintf(stderr, "Error: invalid CSV header.\n");
                fclose(fp);
                return -1;
            }
            continue;
        }

        int  id, score;
        char name[32];
        if (!parse_csv_line(line, &id, name, &score)) {
            fprintf(stderr, "Warning: skipping malformed CSV line %d.\n", lineno);
            continue;
        }

        if (student_find(*head, id)) {
            fprintf(stderr, "Warning: duplicate ID %d in CSV, skipping.\n", id);
            continue;
        }

        Student* s = student_create(id, name, score);
        if (!s) {
            fprintf(stderr, "Error: memory allocation failed.\n");
            fclose(fp);
            return -1;
        }
        student_append(head, s);
        count++;
    }

    fclose(fp);
    return count;
}

/* CSV 저장 */
int save_students(const char* filename, Student* head) {
    FILE* fp = fopen(filename, "w");
    if (!fp) return -1;

    fprintf(fp, "id,name,score\n");
    int count = 0;
    Student* cur = head;
    while (cur) {
        fprintf(fp, "%d,%s,%d\n", cur->id, cur->name, cur->score);
        count++;
        cur = cur->next;
    }

    fclose(fp);
    return count;
}
