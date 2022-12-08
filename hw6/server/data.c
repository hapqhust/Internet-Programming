#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

Image *CreateNewFile(char *name, char *path)
{
    Image *p = (Image *)malloc(sizeof(Image));
    strcpy(p->name, name);
    strcpy(p->path, path);
    p->next = NULL;
    return p;
}

Image *AddFile(Image *account, char *name, char *path)
{
    if (account == NULL)
    {
        Image *temp = CreateNewFile(name, path);
        return temp;
    }
    else
    {
        if (!CheckFile(account, name))
        {
            Image *cur = account;
            while (cur->next != NULL)
            {
                cur = cur->next;
            }
            Image *temp = CreateNewFile(name, path);
            cur->next = temp;
            return account;
        }
        else
        {
            return account;
        }
    }
}

int CheckFile(Image *account, char *name)
{
    Image *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

char* GetPath(Image *acc, char *name){
    Image *cur = acc;
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur->path;
        }
        cur = cur->next;
    }
    return "";
}

Image *ReadFile(Image *acc)
{
    char name[30];
    char path[30];
    FILE *inp = fopen("data.txt", "r");
    if (!inp)
    {
        printf("Error: Can't open this file! \n");
        return NULL;
    }

    do
    {
        int x;
        x = fscanf(inp, "%s %s", name, path);
        if (x > 0)
        {
            acc = AddFile(acc, name, path);
            numAccount++;
        }
        else
            break;
    } while (1);
    fclose(inp);
    return acc;
}

void UpdateFile(Image *acc)
{
    FILE *inp = fopen("data.txt", "w");
    Image *cur = acc;
    while (cur != NULL)
    {
        fprintf(inp, "%s %s\n", cur->name, cur->path);
        cur = cur->next;
    }
    fclose(inp);
}

void freeList(Image *head)
{
    Image *tmp;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}