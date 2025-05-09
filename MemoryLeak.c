#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Need only filepath as arg.");
    return 1;
  } else {
    FILE *file;
    errno_t err = fopen_s(&file, argv[1], "r");
    if (!file) {
      printf("Failed to open file with errno code : %i", err);
      return 1;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = malloc(len + 1);
    len = fread(data, 1, len, file);
    fclose(file);
    data = realloc(data, len + 1);
    data[len] = '\0';
    long numVars = 1;
    char **vars = malloc(sizeof(char *) * numVars);
    short *freed = malloc(sizeof(int) * numVars);
    long pos = 0;
    while (pos < len) {
      int ogPos;
      if (data[pos] == 'm') {
        ogPos = pos - 1;
        pos++;
        if (data[pos] == 'a') {
          pos++;
          if (data[pos] == 'l') {
            pos++;
            if (data[pos] == 'l') {
              pos++;
              if (data[pos] == 'o') {
                pos++;
                if (data[pos] == 'c') {
                  pos++;
                  while (!isalpha(data[ogPos])) {
                    ogPos--;
                  }
                  int startPos = ogPos - 1;
                  while (isalpha(data[startPos])) {
                    startPos--;
                  }
                  int varLen = ogPos - startPos;
                  numVars++;
                  vars = realloc(vars, sizeof(char *) * numVars);
                  freed = realloc(freed, sizeof(int) * numVars);
                  freed[numVars - 1] = 0;
                  vars[numVars - 1] = malloc(sizeof(char) * (varLen + 1));
                  vars[numVars - 1][varLen] = '\0';
                  startPos++;
                  int vc = 0;
                  while (vc != varLen) {
                    vars[numVars - 1][vc] = data[startPos];
                    vc++;
                    startPos++;
                  }
                }
              }
            }
          }
        }
      }
      if (data[pos] == 'f') {
        pos++;
        if (data[pos] == 'r') {
          pos++;
          if (data[pos] == 'e') {
            pos++;
            if (data[pos] == 'e') {
              pos += 2;
              ogPos = pos;
              while (isalpha(data[pos])) {
                pos++;
              }
              int varLen = pos - ogPos;
              char *freedVar = malloc(sizeof(char) * (varLen + 1));
              freedVar[varLen] = '\0';
              int varPos = 0;
              while (ogPos != pos) {
                freedVar[varPos] = data[ogPos];
                ogPos++;
                varPos++;
              }

              for (int i = 1; i < numVars; i++) {
                if (strcmp(freedVar, vars[i]) == 0) {
                  freed[i] = 1;
                }
              }

              free(freedVar);
            }
          }
        }
      }
      pos++;
    }
    free(data);

    for (long var = 1; var < numVars; var++) {
      if (!freed[var]) {
        printf("Variable not freed: %s\n", vars[var]);
      }
      free(vars[var]);
    }
    free(vars);
    free(freed);

    return 0;
  }
}