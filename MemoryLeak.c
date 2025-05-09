#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline char *readFile(const char *name, int *len) {
  FILE *file;
  errno_t err = fopen_s(&file, name, "r");
  if (!file) {
    printf("Failed to open file with errno code : %i", err);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  *len = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *data = malloc(*len + 1);
  *len = fread(data, 1, *len, file);
  fclose(file);
  data = realloc(data, *len + 1);
  data[*len] = '\0';

  return data;
}

inline int wordMatch(const char *file, int *pos, const int len,
                     const char *string, const int str_len) {
  if (*pos + str_len >= len) {
    *pos = len;
    return 0;
  }
  for (int i = 0; i < str_len; i++) {
    if (string[i] != file[*pos + i]) {
      return 0;
    }
  }
  *pos += str_len;
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Need only filepath as arg.");
    return 1;
  } else {
    int len;
    char *data = readFile(argv[1], &len);
    if (data == NULL) {
      return 1;
    }
    // Constants of the memory associated functions
    const char mallocName[] = "malloc";
    const char callocName[] = "calloc";
    const char freeName[] = "free";

    // Variable setup
    long numVars = 1;
    char **vars = malloc(sizeof(char *) * numVars);
    char *freed = malloc(sizeof(char) * numVars);
    int pos = 0;

    // Loop through the file
    while (pos < len) {
      int endPos = pos - 1;
      // Is it malloc or calloc
      if (wordMatch(data, &pos, len, mallocName, sizeof(mallocName) - 1) ||
          wordMatch(data, &pos, len, callocName, sizeof(callocName) - 1)) {
        // Get the end position of the variable name
        while (!isalpha(data[endPos])) {
          endPos--;
        }

        // Get the start position of the variable name
        int startPos = endPos - 1;
        while (isalpha(data[startPos])) {
          startPos--;
        }

        // Create memory for the variable
        int varLen = endPos - startPos;
        int varIdx = numVars;
        numVars++;
        vars = realloc(vars, sizeof(char *) * numVars);
        freed = realloc(freed, numVars);
        vars[varIdx] = malloc(varLen + 1);
        vars[varIdx][varLen] = '\0';

        // Copy the variable name and set it to unfreed
        memcpy(vars[varIdx], data + startPos + 1, varLen);
        freed[varIdx] = 0;
      }

      // Check if there is a call to free
      if (wordMatch(data, &pos, len, freeName, sizeof(freeName) - 1)) {
        // Get the start and end positions of the variable being freed
        int startPos = pos;
        while (!isalpha(data[startPos])) {
          startPos++;
        }
        endPos = startPos + 1;
        while (isalpha(data[endPos])) {
          endPos++;
        }

        // Get the variable name
        int varLen = endPos - startPos;
        char *freedVar = malloc(varLen + 1);
        freedVar[varLen] = '\0';
        memcpy(freedVar, data + startPos, varLen);

        // Check if it matches existing variable names
        for (int i = 1; i < numVars; i++) {
          // If variable is the same as the one freed, set it to freed
          if (freed[i]) {
            continue;
          }
          if (strcmp(freedVar, vars[i]) == 0) {
            freed[i] = 1;
            break;
          }
        }

        free(freedVar);
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