#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Image
{
    char username[30];
    char password[30];
    int status;
    int isSignIn;
    struct Image *next;

} Image;

#define SUCCESSFUL 1
#define ACCOUNT_IS_BLOCKED 2
#define ACCOUNT_IS_BLOCKED_OR_INACITIVE 3
#define PASSWORD_IS_NOT_TRUE 4

int numAccount;

Image *CreateNewAccount(char *username, char *password, int status);
Image *AddAccount(Image *account, char *username, char *password, int status);
int CheckUser(Image *account, char *username);
int CheckPassword(Image *account, char *password);
int CheckActive(Image *account, char *username);
void CheckSignIn(Image *account, char *username);
Image *ReadAccount(Image *account);
void UpdateFile(Image *account);
void freeList(Image *account);