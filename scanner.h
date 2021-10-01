

enum State {
    STATE_NEW,
    STATE_RETURN_OPERATOR,

    STATE_ASSIGN,
    STATE_DEFINE_TYPE,
    STATE_EQ,
    STATE_LT,
    STATE_LTEQ,
    STATE_GT,
    STATE_GTEQ,
    STATE_NOT,
    STATE_DIV,
    STATE_IDIV,
    STATE_MUL,
    STATE_ADD,
    STATE_SUB,
    STATE_LEFT_PARENTES,
    STATE_RIGHT_PARENTES,
    STATE_EOS,
    STATE_EOF,
    STATE_COMMA,
    STATE_COMMENT, // commenty bude mazat lexikalní analyzátor
    STATE_CONCAT,
    STATE_INTEGER,
    STATE_NUMBER,
    STATE_STRLEN,
    STATE_STRING,
    STATE_IDENTIFIER
    } state;


void get_token();