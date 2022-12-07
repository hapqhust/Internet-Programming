#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "account.h"

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
        if (!CheckUser(account, username))
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
            return account;
        }
    }
}

int CheckUser(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        // printf("%s - %s\n", cur->username, username);
        if (strcmp(cur->username, username) == 0)
        {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

int CheckPassword(Account *account, char *password)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->password, password) == 0)
        {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

int CheckActive(Account *account, char *username)
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
        int x;
        x = fscanf(inp, "%s %s %d", username, password, &status);
        if (x > 0)
        {
            acc = AddAccount(acc, username, password, status);
            numAccount++;
        }
        else
            break;
    } while (1);
    fclose(inp);
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