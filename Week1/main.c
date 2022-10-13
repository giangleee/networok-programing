#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 30
#define TRUE 1
#define FALSE 0
#define IS_ACTIVE 1

// CREATE LINK LIST DATA
struct accountDetail
{
    char username[MAX_CHAR];
    char password[MAX_CHAR];
    int status;
    int is_auth;
};
typedef struct accountDetail ACCOUNT;

// CREATE NODE
struct node
{
    ACCOUNT data;
    struct node *pNext;
};
typedef struct node NODE;

// CREATE LIST
struct list
{
    NODE *pHead;
    NODE *pTail;
};
typedef struct list LINKLIST;

// FUNCTION
// CODE
void createList(LINKLIST *l);

// create node with data
NODE *createNODE(ACCOUNT x);

// insert node to tail
void InsertToTail(LINKLIST *l, NODE *p);

// input data
void InputData(LINKLIST *l);

// display link list
void Display(LINKLIST l);

// search acroding username
int SearchUsername(LINKLIST l, char str[]);

// search acroding password
int SearchPassword(LINKLIST l, char author[]);

// delete node link list
void DeleteHead(LINKLIST *l);

// register function
void Register(LINKLIST *l);

// sign in function
void SignIn(LINKLIST *l);

// lcok username
void LockUsername(LINKLIST l, char str[]);

// get status
void getStatus(LINKLIST l, char str[]);
// search
void Search(LINKLIST *l);
// sign out funciton
void SignOut(LINKLIST *l);

// update function
void UpdateFile(LINKLIST l);

// set auth
void setAuth(LINKLIST l, char str[], int is_auth);

// get auth
int getAuth(LINKLIST l, char str[]);

int main()
{
    LINKLIST l;
    createList(&l);
    InputData(&l);
    int menu;

    do
    {
        printf("USER MANAGEMENT PROGRAM\n");
        printf("-----------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("Your choice (1-4, other to quit):");
        scanf("%d", &menu);

        switch (menu)
        {
        case 1:
            Register(&l);
            break;
        case 2:
            SignIn(&l);
            break;
        case 3:
            Search(&l);
            break;
        case 4:
            SignOut(&l);
            break;
        default:
            exit(0);
        }
    } while (1 == 1);
    return 0;
}

// CODE
void createList(LINKLIST *l)
{
    l->pHead = NULL;
    l->pTail = NULL;
}

// create node with data
NODE *createNODE(ACCOUNT x)
{
    NODE *p = (NODE *)malloc(sizeof(NODE));
    if (p == NULL)
    {
        printf("Khong du bo nho de cap phat!");
        return NULL;
    }

    p->data = x;
    p->pNext = NULL;
    return p;
}

// insert node to tail
void InsertToTail(LINKLIST *l, NODE *p)
{
    // Danh sách đang rỗng
    if (l->pHead == NULL)
    {
        l->pHead = l->pTail = p;
    }
    else
    {
        (l->pTail)->pNext = p;
        p->pNext = NULL;
        l->pTail = p;
    }
}

// input data
void InputData(LINKLIST *l)
{
    char user[MAX_CHAR];
    char pass[MAX_CHAR];
    int status;
    FILE *inp = fopen("account.txt", "r");

    if (!inp)
    {
        printf("Error: Can't open this file! \n");
        return;
    }
    else
    {
        do
        {
            if (fscanf(inp, "%s %s %d", user, pass, &status) > 0)
            {
                ACCOUNT *temp = (ACCOUNT *)malloc(sizeof(ACCOUNT));
                strcpy(temp->username, user);
                strcpy(temp->password, pass);
                temp->status = status;
                temp->is_auth = FALSE;
                InsertToTail(l, createNODE(*temp));
                free(temp);
            }
            else
                break;
        } while (1 == 1);
    }
    fclose(inp);
}

// display link list
void Display(LINKLIST l)
{
    printf("Display link list:\n");
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        printf("%s %s %d\n", k->data.username, k->data.password, k->data.status);
    }
}

// search acroding username
int SearchUsername(LINKLIST l, char str[])
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, str);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.username);
        if (strcmp(temp1, temp2) == 0)
        {
            return TRUE;
            break;
        }
    }
    return FALSE;
}

// search acroding password
int SearchPassword(LINKLIST l, char author[])
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, author);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.password);
        if (strcmp(temp1, temp2) == 0)
        {
            return TRUE;
            break;
        }
    }
    return FALSE;
}

// delete node link list
void DeleteHead(LINKLIST *l)
{
    if (l->pHead == NULL)
    {
        return;
    }

    NODE *p = l->pHead;
    l->pHead = l->pHead->pNext;
    free(p);
}

// register function
void Register(LINKLIST *l)
{
    ACCOUNT *temp = (ACCOUNT *)malloc(sizeof(ACCOUNT));

    int n;
    printf("----Welcome to Register function.----\n");
    printf("Input your Username: ");
    scanf("%s", temp->username);
    temp->status = 1;
    if (SearchUsername(*l, temp->username) == TRUE)
    {
        printf("Account existed\n");
    }
    else
    {
        printf("Input your Password: ");
        scanf("%s", temp->password);
        InsertToTail(l, createNODE(*temp));
        UpdateFile(*l);
        printf("Successful registration.\n");
    }

    free(temp);
}

// sign in function
void SignIn(LINKLIST *l)
{
    char user[MAX_CHAR], pass[MAX_CHAR];
    int numSignIn = 0;
    int isSignIn = FALSE;

    printf("----Welcome to Sign in function.----\n");
    printf("Input your Username: ");
    scanf("%s", user);
    printf("Input your Password: ");
    scanf("%s", pass);

    if ((SearchUsername(*l, user) == TRUE && SearchPassword(*l, pass) == TRUE))
    {
        printf("Hello hust \n");
        isSignIn = TRUE;
        setAuth(*l, user, TRUE);
    }

    while (numSignIn < 2 && getAuth(*l, user) == FALSE)
    {
        if ((SearchUsername(*l, user) == FALSE || SearchPassword(*l, pass) == FALSE))
        {
            printf("Cannot find account\n");
            printf("Please re-enter data\n");
            numSignIn++;
            printf("Input your Username: ");
            scanf("%s", user);
            printf("Input your Password: ");
            scanf("%s", pass);
        }
        else if ((SearchUsername(*l, user) == TRUE && SearchPassword(*l, pass) == TRUE))
        {
            printf("Hello hust.\n");
            setAuth(*l, user, TRUE);
            break;
        }
    }

    if (isSignIn == FALSE && numSignIn >= 2)
    {
        printf("Password is incorrect. Account is blocked\n");
        LockUsername(*l, user);
    }
}

// lcok username
void LockUsername(LINKLIST l, char str[])
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, str);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.username);
        if (strcmp(temp1, temp2) == 0)
        {
            k->data.status = 0;
        }
    }
}

// get status
void getStatus(LINKLIST l, char str[])
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, str);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.username);
        if (strcmp(temp1, temp2) == 0)
        {
            if (k->data.status == IS_ACTIVE)
            {
                printf("Account is active\n");
            }
            else
            {
                printf("Account is blocked\n");
            }
        }
    }
}

// search
void Search(LINKLIST *l)
{
    char user[MAX_CHAR];

    printf("----Welcome to Search function.----\n");
    printf("Input your Username: ");
    scanf("%s", user);

    if (getAuth(*l, user) == FALSE)
    {
        printf("Account is not sign in\n");
    }
    else if (SearchUsername(*l, user) == TRUE)
    {
        getStatus(*l, user);
    }
}

// sign out funciton
void SignOut(LINKLIST *l)
{
    char user[MAX_CHAR];

    printf("----Welcome to Sign out function.----\n");
    printf("Input your Username: ");
    scanf("%s", user);

    if (SearchUsername(*l, user) == TRUE)
    {
        setAuth(*l, user, FALSE);
        printf("Goodbye hust\n");
    }
    else
    {
        printf("Cannot find account\n");
    }
}

// update function
void UpdateFile(LINKLIST l)
{
    FILE *inp = fopen("account.txt", "w");

    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        fprintf(inp, "%s %s %d\n", k->data.username, k->data.password, k->data.status);
    }

    fclose(inp);
}

// set auth
void setAuth(LINKLIST l, char str[], int is_auth)
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, str);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.username);
        if (strcmp(temp1, temp2) == 0)
        {
            k->data.is_auth = is_auth;
        }
    }
}

// get auth
int getAuth(LINKLIST l, char str[])
{
    char temp1[MAX_CHAR], temp2[MAX_CHAR];
    strcpy(temp1, str);
    for (NODE *k = l.pHead; k != NULL; k = k->pNext)
    {
        strcpy(temp2, k->data.username);
        if (strcmp(temp1, temp2) == 0)
        {
            return k->data.is_auth;
        }
    }
}
