#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Image
{
    char name[30];
    char path[30];
    struct Image *next;

} Image;

int numAccount;

Image *CreateNewFile(char *name, char *path);
Image *AddFile(Image *account, char *name, char *path);
int CheckFile(Image *account, char *name);
char* GetPath(Image *acc, char *name);
Image *ReadFile(Image *acc);
void UpdateFile(Image *acc);
void freeList(Image *account);