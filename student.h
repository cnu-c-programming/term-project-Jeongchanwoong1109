#ifndef STUDENT_H
#define STUDENT_H

typedef struct Student {
    int id;
    char name[32];
    int score;
    struct Student* next;
} Student;

/* 연결 리스트 조작 함수 */
Student* student_create(int id, const char* name, int score);
void     student_append(Student** head, Student* node);
Student* student_find(Student* head, int id);
int      student_delete(Student** head, int id);
void     student_free_all(Student** head);
int      student_count(Student* head);

/* 정렬 */
void student_sort_by_name(Student** head);
void student_sort_by_score(Student** head);

#endif /* STUDENT_H */
