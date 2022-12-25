#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Account
{
    char username[30];
    char password[30];
    int status;
    int isSignIn;
    struct Account *next;

} Account;

#define SUCCESSFUL 1
#define ACCOUNT_IS_BLOCKED 2
#define ACCOUNT_IS_BLOCKED_OR_INACITIVE 3
#define PASSWORD_IS_NOT_TRUE 4

int numAccount;

Account *CreateNewAccount(char *username, char *password, int status);
Account *AddAccount(Account *account, char *username, char *password, int status);
int CheckUser(Account *account, char *username);
int CheckPassword(Account *account, char *password);
int CheckActive(Account *account, char *username);
void CheckSignIn(Account *account, char *username);
Account *ReadAccount(Account *account);
void UpdateFile(Account *account);
void freeList(Account *account);