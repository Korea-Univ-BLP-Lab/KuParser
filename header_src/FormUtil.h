#if !defined (__FORM_UTIL_H__)
#define __FORM_UTIL_H__

#include <stdio.h>

/* 서버로부터 넘겨받는 매개변수의 최대크기 */
#define MAX_ENTRIES 10000

typedef struct {
  char *Name;
  char *Value;
} FormEntry;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus extern "C" {

void getEntry_GET(FormEntry entries[MAX_ENTRIES]);
void getEntry(FormEntry parameter[MAX_ENTRIES]); 
char *getValue(FormEntry parameter[MAX_ENTRIES], char *s);

void getword(char *word, char *line, char stop);
char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);

#ifdef __cplusplus
}
#endif // __cplusplus }


#endif // __FORM_UTIL_H__