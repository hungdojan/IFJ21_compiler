typedef struct
{
    char *string;
    int allocSize;
} Istring;


int string_Init(Istring *s);
int string_Add_Char(Istring *s, char c);
int string_Free(Istring *s);
