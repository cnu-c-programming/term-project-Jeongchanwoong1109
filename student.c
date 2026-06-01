#include "student.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 노드 생성 */
Student* student_create(int id, const char* name, int score) {
    Student* s = (Student*)malloc(sizeof(Student));
    if (!s) return NULL;
    s->id    = id;
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    s->score = score;
    s->next  = NULL;
    return s;
}

/* 리스트 끝에 추가 */
void student_append(Student** head, Student* node) {
    if (!node) return;
    if (!*head) {
        *head = node;
        return;
    }
    Student* cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = node;
}

/* ID로 검색 */
Student* student_find(Student* head, int id) {
    while (head) {
        if (head->id == id) return head;
        head = head->next;
    }
    return NULL;
}

/* ID로 삭제 */
int student_delete(Student** head, int id) {
    if (!*head) return 0;

    /* 첫 번째 노드가 대상인 경우 */
    if ((*head)->id == id) {
        Student* tmp = *head;
        *head = (*head)->next;
        free(tmp);
        return 1;
    }

    Student* prev = *head;
    Student* cur  = (*head)->next;
    while (cur) {
        if (cur->id == id) {
            prev->next = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    return 0;
}

/* 전체 해제 */
void student_free_all(Student** head) {
    Student* cur = *head;
    while (cur) {
        Student* next = cur->next;
        free(cur);
        cur = next;
    }
    *head = NULL;
}

/* 개수 반환 */
int student_count(Student* head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

/* 이름 오름차순 */
void student_sort_by_name(Student** head) {
    if (!*head || !(*head)->next) return;
    int swapped;
    do {
        swapped = 0;
        Student* cur = *head;
        while (cur->next) {
            if (strcmp(cur->name, cur->next->name) > 0) {
                int  tmp_id    = cur->id;
                char tmp_name[32];
                int  tmp_score = cur->score;
                strncpy(tmp_name, cur->name, 32);

                cur->id    = cur->next->id;
                strncpy(cur->name, cur->next->name, 32);
                cur->score = cur->next->score;

                cur->next->id    = tmp_id;
                strncpy(cur->next->name, tmp_name, 32);
                cur->next->score = tmp_score;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);
}

/* 점수 오름차순 */
void student_sort_by_score(Student** head) {
    if (!*head || !(*head)->next) return;
    int swapped;
    do {
        swapped = 0;
        Student* cur = *head;
        while (cur->next) {
            if (cur->score > cur->next->score) {
                int  tmp_id    = cur->id;
                char tmp_name[32];
                int  tmp_score = cur->score;
                strncpy(tmp_name, cur->name, 32);

                cur->id    = cur->next->id;
                strncpy(cur->name, cur->next->name, 32);
                cur->score = cur->next->score;

                cur->next->id    = tmp_id;
                strncpy(cur->next->name, tmp_name, 32);
                cur->next->score = tmp_score;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);
}
