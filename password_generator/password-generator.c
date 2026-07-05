#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PASSWORD_LEN 20

int main(void) {

  // define a char set
  char allowed_characters[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFHGHIJKLMNOPQRSTUVWXYZ123456789";

 // printf("Size of allowed_characters %zu\n", sizeof(allowed_characters));

  // to generate a different password every run
  srand(time(NULL));

  char password[PASSWORD_LEN + 1]; // last character to be null byte
  for (int i = 0; i < PASSWORD_LEN; i++) {
    int pos = rand() % (sizeof(allowed_characters) - 1);
    printf("%d\n", pos);
    password[i] = allowed_characters[pos];
  }

  password[PASSWORD_LEN] = 0;

  printf("Password generated: %s\n", password);

  return 0;
}
