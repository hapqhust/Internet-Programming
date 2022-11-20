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

int numAccount;

Account *CreateNewAccount(char *username, char *password, int status);
Account *AddAccount(Account *account, char *username, char *password, int status);
int CheckUser(Account *account, char *username);
int CheckPassword(Account *account, char *password);
int CheckBlocked(Account *account, char *username);
void CheckSignIn(Account *account, char *username);
Account *ReadAccount(Account *account);
Account *Register(Account *account);
void UpdateFile(Account *account);
void SignIn(Account *account);
void Search(Account *account);
void SignOut(Account *account);
void freeList(Account *account);

int main()
{
    int menu;
    Account *acc = NULL;
    acc = ReadAccount(acc);
    while (1 == 1)
    {
        printf("\nUSER MANAGEMENT PROGRAM\n");
        printf("-----------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("Your choice (1-4, other to quit):\n");
        scanf("%d", &menu);
        switch (menu)
        {
        case 1:
            acc = Register(acc);
            break;
        case 2:
            SignIn(acc);
            break;
        case 3:
            Search(acc);
            break;
        case 4:
            SignOut(acc);
            break;
        default:
            freeList(acc);
            exit(0);
        }
    }
}

Account *CreateNewAccount(char *username, char *password, int status)
{
    Account *p = (Account *)malloc(sizeof(Account));
    strcpy(p->username, username);
    strcpy(p->password, password);
    p->status = status;
    p->isSignIn = 0;
    p->next = NULL;
    return p;
}

Account *AddAccount(Account *account, char *username, char *password, int status)
{
    if (account == NULL)
    {
        Account *temp = CreateNewAccount(username, password, status);
        return temp;
    }
    else
    {
        if (CheckUser(account, username))
        {
            Account *cur = account;
            while (cur->next != NULL)
            {
                cur = cur->next;
            }
            Account *temp = CreateNewAccount(username, password, status);
            cur->next = temp;
            return account;
        }
        else
        {
            return NULL;
        }
    }
}

int CheckUser(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            return 0;
        }
        cur = cur->next;
    }
    return 1;
}

int CheckPassword(Account *account, char *password)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->password, password) == 0)
        {
            return 0;
        }
        cur = cur->next;
    }
    return 1;
}

int CheckBlocked(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            return cur->status;
        }
        cur = cur->next;
    }
}

void CheckSignIn(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            if (cur->isSignIn == 0)
            {
                printf("This account is not signed in! \n");
            }
            else
            {
                printf("Sign out is sucessful! \n");
                cur->isSignIn = 0;
            }
            return;
        }
        cur = cur->next;
    }
}

Account *ReadAccount(Account *acc)
{
    char username[30];
    char password[30];
    int status;
    numAccount = 0;
    FILE *inp = fopen("account.txt", "r");
    if (!inp)
    {
        printf("Error: Can't open this file! \n");
        return NULL;
    }

    do
    {
        if (fscanf(inp, "%s %s %d", username, password, &status) > 0)
        {
            acc = AddAccount(acc, username, password, status);
            numAccount++;
        }
        else
            break;
    } while (1 == 1);
    fclose(inp);
    return acc;
}

Account *Register(Account *acc)
{
    printf("----Welcome to Register function.----\n");

    char username[30];
    char password[30];
    printf("Input your Username: ");
    scanf("%s", username);
    if (CheckUser(acc, username) != 0)
    {
        printf("Input your Password: ");
        scanf("%s", password);
        acc = AddAccount(acc, username, password, 1);
        numAccount++;
        printf("Successful registration. \n");
        UpdateFile(acc);
    }
    else
    {
        printf("This account existed! \n");
    }
    return acc;
}

void UpdateFile(Account *acc)
{
    FILE *inp = fopen("account.txt", "w");
    Account *cur = acc;
    while (cur != NULL)
    {
        fprintf(inp, "%s %s %d\n", cur->username, cur->password, cur->status);
        cur = cur->next;
    }
    fclose(inp);
}

void SignIn(Account *acc)
{
    printf("----Welcome to SignIn function.----\n");
    char username[30];
    char password[30];

    int n = 3;
    printf("Input your Username: ");
    scanf("%s", username);
    if (CheckUser(acc, username) != 0)
    {
        printf("Cannot find account \n");
        return;
    }
    else
    {
        if (CheckBlocked(acc, username) == 0)
        {
            printf("Account is blocked, you can't access this account !\n");
            return;
        }
        else
        {
            while (n > 0)
            {
                printf("Input your Password: ");
                scanf("%s", password);
                if (CheckPassword(acc, password) != 0)
                {
                    printf("Password is incorrect!\n");
                    n--;
                }
                else
                {
                    printf("Hello %s, sign in is successful!\n", username);
                    Account *cur = acc;
                    while (cur != NULL)
                    {
                        if (strcmp(cur->username, username) == 0)
                        {
                            cur->isSignIn = 1;
                        }
                        cur = cur->next;
                    }
                    return;
                }
            }
            printf("This account is blocked!\n");
            Account *cur = acc;
            while (cur != NULL)
            {
                if (strcmp(cur->username, username) == 0)
                {
                    cur->status = 0;
                }
                cur = cur->next;
            }
            UpdateFile(acc);
            return;
        }
    }
}

void Search(Account *acc)
{
    printf("----Welcome to Search function.----\n");
    char username[30];

    printf("Input your Username: ");
    scanf("%s", username);
    if (CheckUser(acc, username) != 0)
    {
        printf("Account does not exist!\n");
    }
    else
    {
        if (CheckBlocked(acc, username) == 0)
        {
            printf("Account is blocked!\n");
        }
        else
        {
            printf("Account is active!\n");
        }
    }
    return;
}

void SignOut(Account *acc)
{
    printf("----Welcome to SignOut function.----\n");
    char username[30];

    printf("Input your Username: ");
    scanf("%s", username);
    if (CheckUser(acc, username) != 0)
    {
        printf("Account does not exist!\n");
    }
    else
    {
        CheckSignIn(acc, username);
    }
}

void freeList(Account *head)
{
    Account *tmp;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}