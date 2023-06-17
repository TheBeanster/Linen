#include "lnn_code.h"
#include "lnn_parse.h"

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

const char* lnn_statementtype_names[Lnn_NUM_STATEMENTTYPES] =
{
	"ST_EXPRESSION",
	"ST_IF",
	"ST_FOR",
	"ST_WHILE",
	"ST_DOWHILE",
	"ST_RETURN",
	"ST_SCOPE"
};

void Lnn_DestroyStatement(Lnn_Statement* stmt)
{
}





static void print_indent(const int indent)
{
	for (int i = 0; i < indent; i++)
		printf("  ");
}
#define indented_printf print_indent(indent); printf

static void print_code_block(const Lnn_CodeBlock* block, const int indent);

static void print_expression(const Lnn_ExprNode* expr, const int indent)
{
	indented_printf("Expr node lol {}\n");
}

static void print_if_statement(const Lnn_Statement* stmt, const int indent)
{
	indented_printf("Condition {\n");
	print_expression(stmt->u.stmt_if.condition, indent + 1);
	indented_printf("}\n");
	indented_printf("Code block on true {\n");
	print_code_block(stmt->u.stmt_if.block_ontrue, indent + 1);
	indented_printf("}\n");
	indented_printf("Code block on false {\n");
	print_code_block(stmt->u.stmt_if.block_onfalse, indent + 1);
	indented_printf("}\n");
}

static void print_statement(const Lnn_Statement* stmt, const int indent)
{
	if (!stmt)
		{ indented_printf("Statement is null\n"); return; }
	if (stmt->type < 0 || stmt->type >= Lnn_NUM_STATEMENTTYPES)
		{ indented_printf("Statement type %i is invalid\n", stmt->type); return; }
	
	indented_printf("%s {\n", lnn_statementtype_names[stmt->type]);
	switch (stmt->type)
	{
	case Lnn_ST_IF: print_if_statement(stmt, indent + 1); break;
	default:
		break;
	}
	indented_printf("}\n");
}

static void print_code_block(const Lnn_CodeBlock* block, const int indent)
{
	if (!block)
		{ indented_printf("null\n"); return; }
	if (block->statements.count == 0)
		{ indented_printf("empty\n"); return; }
	if (block->statements.count < 0 || block->statements.count > 500)
		{ indented_printf("Block has invalid number of statements at %i\n", block->statements.count); return; }
	Lnn_Statement* stmt_iter = block->statements.begin;
	for (int i = 0; i < block->statements.count; i++)
	{
		print_statement(stmt_iter, indent);
	}
}

void Lnn_PrintCodeTree(const Lnn_CodeBlock* block)
{
	printf("Printing code tree\n");
	if (!block)
	{
		printf("Block is null\n");
		return;
	}

	printf("{\n");
	print_code_block(block, 1);
	printf("}\n");
}
