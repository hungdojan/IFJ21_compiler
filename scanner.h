

enum tokenS {
    TOKEN_ASSIGN,
    TOKEN_DEFINE_TYPE,
    TOKEN_EQ,
    TOKEN_LT,
    TOKEN_LTEQ,
    TOKEN_GT,
    TOKEN_GTEQ,
    TOKEN_NOT,
    TOKEN_DIV,
    TOKEN_IDIV,
    TOKEN_MUL,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_LEFT_PARENTES,
    TOKEN_RIGHT_PARENTES,
    TOKEN_EOS,
    TOKEN_EOF,
    TOKEN_COMMA,
    TOKEN_COMMENT, // commenty bude mazat lexikalní analyzátor
    TOKEN_CONCAT,
    TOKEN_INTEGER,
    TOKEN_NUMBER,
    TOKEN_STRLEN,
    TOKEN_STRING,
    TOKEN_IDENTIFIER
    } token;


void get_token();