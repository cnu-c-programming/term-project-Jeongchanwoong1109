#ifndef FILE_IO_H
#define FILE_IO_H

#include "student.h"

/* CSV 파일에서 학생 정보를 읽어 연결 리스트로 반환 */
/* 성공 시 읽은 학생 수, 실패 시 -1 반환 */
int load_students(const char* filename, Student** head);

/* 연결 리스트의 학생 정보를 CSV 파일에 저장 */
/* 성공 시 저장한 학생 수, 실패 시 -1 반환 */
int save_students(const char* filename, Student* head);

#endif /* FILE_IO_H */
