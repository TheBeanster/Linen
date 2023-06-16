#include "lnn_code.h"

const char* lnn_keyword_strings[Lnn_NUM_KEYWORDS] =
{
	"if",
	"then",
	"else",
	"for",
	"do",
	"while",
	"function",
	"return",
	"false",
	"true",
	"end"
};

const char* lnn_keywordid_names[Lnn_NUM_KEYWORDS] =
{
	"KW_IF",
	"KW_THEN",
	"KW_ELSE",
	"KW_FOR",
	"KW_DO",
	"KW_WHILE",
	"KW_FUNCTION",
	"KW_RETURN",
	"KW_FALSE",
	"KW_TRUE",
	"KW_END"
};

Lnn_KeywordID Lnn_GetKeyword(const char* string)
{
	for (int i = 0; i < Lnn_NUM_KEYWORDS; i++)
		if (strcmp(string, lnn_keyword_strings[i]) == 0)
			return (Lnn_KeywordID)i;
	return Lnn_KW_NULL;
}



const char* lnn_operator_strings[Lnn_NUM_OPERATORS] =
{
	"=",
	"+=",
	"-=",
	"*=",
	"/=",

	"!",
	"&",
	"|",
	"^",
	"", /* Unary negative has a special case */

	"==",
	"!=",
	"<",
	">",
	"<=",
	">=",

	"+",
	"-",
	"*",
	"/",

	".",
	"", /* Array access has a special case */
};

const char* lnn_operatorid_names[Lnn_NUM_OPERATORS] =
{
	"OP_ASSIGN",
	"OP_ASSIGNADD",
	"OP_ASSIGNSUB",
	"OP_ASSIGNMUL",
	"OP_ASSIGNDIV",

	"OP_NOT",
	"OP_AND",
	"OP_OR",
	"OP_XOR",
	"OP_NEGATIVE",

	"OP_EQUALITY",
	"OP_INEQUALITY",
	"OP_LESS",
	"OP_GREATER",
	"OP_LESSEQUAL",
	"OP_GREATEREQUAL",

	"OP_ADD",
	"OP_SUB",
	"OP_MUL",
	"OP_DIV",

	"OP_MEMBERACCESS",
	"OP_ARRAYACCESS"
};

const int lnn_operator_precedence[Lnn_NUM_OPERATORS] =
{
	1,	/* ASSIGN */
	1,	/* ASSIGNADD */
	1,	/* ASSIGNSUB */
	1,	/* ASSIGNMUL */
	1,	/* ASSIGNDIV */

	9,	/* NOT */
	4,	/* AND */
	4,	/* OR */
	4,	/* XOR */
	8,	/* NEGATIVE */

	2,	/* EQUALITY */
	2,	/* INEQUALITY */
	3,	/* LESS */
	3,	/* GREATER */
	3,	/* LESSEQUAL */
	3,	/* GREATEREQUAL */

	5,	/* ADD */
	5,	/* SUB */
	6,	/* MUL */
	6,	/* DIV */

	11, /* MEMBERACCESS */
	10, /* ARRAYACCESS */
};



Lnn_OperatorID Lnn_GetOperator(const char* string)
{
	Utl_Assert(string);
	for (int i = 0; i < Lnn_NUM_OPERATORS; i++)
		if (strcmp(string, lnn_operator_strings[i]) == 0)
			return (Lnn_OperatorID)i;
	return Lnn_OP_NULL;
}



const char lnn_separator_chars[Lnn_NUM_SEPARATORS] =
{
	'(',
	')',
	'[',
	']',
	'{',
	'}',
	',',
};

const char* lnn_separatorid_names[Lnn_NUM_SEPARATORS] =
{
	"SP_LPAREN",
	"SP_RPAREN",
	"SP_LBRACKET",
	"SP_RBRACKET",
	"SP_LBRACE",
	"SP_RBRACE",
	"SP_COMMA",
};

Lnn_SeparatorID Lnn_GetSeparator(const char c)
{
	for (int i = 0; i < Lnn_NUM_SEPARATORS; i++)
		if (c == lnn_separator_chars[i])
			return (Lnn_SeparatorID)i;
	return Lnn_SP_NULL;
}



void Lnn_DestroyExpression(Lnn_ExprNode* expr)
{
}

void Lnn_DestroyCodeBlock(Lnn_CodeBlock* block)
{
}

void Lnn_DestroyStatement(Lnn_Statement* stmt)
{
}
