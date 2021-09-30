
//#define STRING_BASE_SIZE = 10;

typedef struct
{
    char *string;
    int allocSize;
} Istring;


void string_Init(Istring *s);
void string_Add_Char(Istring *s, char c);
void string_Free(Istring *s);
