#ifndef _Lnn_CODE_H_
#define _Lnn_CODE_H_

#include "fab_utility.h"

typedef char Lnn_KeywordID;
enum
{
	Lnn_KW_IF,
	Lnn_KW_THEN,
	Lnn_KW_ELSE,
	Lnn_KW_FOR,
	Lnn_KW_DO,
	Lnn_KW_WHILE,
	Lnn_KW_FUNCTION,
	Lnn_KW_RETURN,
	Lnn_KW_FALSE,
	Lnn_KW_TRUE,
	Lnn_KW_END,
	Lnn_NUM_KEYWORDS,
	Lnn_KW_NULL = -1, /* Invalid or non keyword */
};
extern const char* lnn_keyword_strings[Lnn_NUM_KEYWORDS];
extern const char* lnn_keywordid_names[Lnn_NUM_KEYWORDS];

/**
 * @brief Checks the keyword id of a string.
 * @param string String to check.
 * @return The ID of the keyword or Lnn_KW_NULL if not keyword.
 */
Lnn_KeywordID Lnn_GetKeyword(const char* string);



typedef char Lnn_OperatorID;
enum
{
	Lnn_OP_ASSIGN,			/* '=' */
	Lnn_OP_ASSIGNADD,		/* '+=' */
	Lnn_OP_ASSIGNSUB,		/* '-=' */
	Lnn_OP_ASSIGNMUL,		/* '*=' */
	Lnn_OP_ASSIGNDIV,			/* '/=' */

	Lnn_OP_NOT,				/* '!' Logical not */
	Lnn_OP_AND,				/* '&' Logical and */
	Lnn_OP_OR,				/* '|' Logical or */
	Lnn_OP_XOR,				/* '^' Logical xor */
	Lnn_OP_NEGATIVE,		/* '-' Unary negative */

	Lnn_OP_EQUALITY,		/* '==' */
	Lnn_OP_INEQUALITY,		/* '!=' */
	Lnn_OP_LESS,			/* '<'  */
	Lnn_OP_GREATER,			/* '>'  */
	Lnn_OP_LESSEQUAL,		/* '<=' */
	Lnn_OP_GREATEREQUAL,	/* '>=' */

	Lnn_OP_ADD,				/* '+' Arithmetic add */
	Lnn_OP_SUB,				/* '-' Arithmetic subtract */
	Lnn_OP_MUL,				/* '*' Arithmetic multiply */
	Lnn_OP_DIV,				/* '/' Arithmetic divide */

	Lnn_OP_MEMBERACCESS,	/* '.' */
	Lnn_OP_ARRAYACCESS,		/* '[]' */

	Lnn_NUM_OPERATORS,
	Lnn_OP_NULL = -1,		/* Invalid or non operator */
};
extern const char* lnn_operator_strings[Lnn_NUM_OPERATORS];
extern const char* lnn_operatorid_names[Lnn_NUM_OPERATORS];
extern const int lnn_operator_precedence[Lnn_NUM_OPERATORS];

/**
 * @brief Checks the operator id of a string.
 * @param string String to check.
 * @return The ID of the operator or Lnn_OP_NULL if not operator.
 */
Lnn_OperatorID Lnn_GetOperator(const char* string);

#define Lnn_IsAssignmentOp(op)	((op) >= Lnn_OP_ASSIGN		|| (op) <= Lnn_OP_ASSIGNDIV)
#define Lnn_IsLogicalOp(op)		((op) >= Lnn_OP_NOT			|| (op) <= Lnn_OP_XOR)
#define Lnn_IsRelationalOp(op)	((op) >= Lnn_OP_EQUALITY	|| (op) <= Lnn_OP_GREATEREQUAL)
#define Lnn_IsArithmeticOp(op)	((op) >= Lnn_OP_ADD	|| (op) <= Lnn_OP_DIV)
#define Lnn_IsUnaryOp(op)		((op) == Lnn_OP_NOT			|| (op) == Lnn_OP_NEGATIVE)



typedef char Lnn_SeparatorID;
enum
{
	Lnn_SP_LPAREN,		/* '(' */
	Lnn_SP_RPAREN,		/* ')' */
	Lnn_SP_LBRACKET,	/* '[' */
	Lnn_SP_RBRACKET,	/* ']' */
	Lnn_SP_LBRACE,		/* '{' */
	Lnn_SP_RBRACE,		/* '}' */
	Lnn_SP_COMMA,		/* ',' */
	Lnn_NUM_SEPARATORS,
	Lnn_SP_NULL = -1,	/* Invalid or non separator */
};
extern const char lnn_separator_chars[Lnn_NUM_SEPARATORS];
extern const char* lnn_separatorid_names[Lnn_NUM_SEPARATORS];

/**
 * @brief Checks the separator id of a char.
 * @param c Character to check.
 * @return The ID of the separator or Lnn_SP_NULL if not separator char.
 */
Lnn_SeparatorID Lnn_GetSeparator(const char c);



typedef enum
{
	Lnn_ET_OPERATOR,
	Lnn_ET_NUMBERLITERAL,
	Lnn_ET_STRINGLITERAL,
	Lnn_ET_BOOLLITERAL,
	Lnn_ET_OBJECT,
	Lnn_ET_VARIABLE,
	Lnn_ET_CLOSURE,
	Lnn_ET_FUNCTIONCALL,
	Lnn_NUM_EXPRNODETYPES
} Lnn_ExprNodeType;
extern const char* lnn_exprnodetype_names[Lnn_NUM_EXPRNODETYPES];

typedef struct
{
	Lnn_ExprNodeType type;
	struct Lnn_ExprNode* parent;
	union
	{
		struct
		{
			Lnn_OperatorID id;
			struct Lnn_ExprNode* left;
			struct Lnn_ExprNode* right;
		} op;
		Utl_Float number;
		Utl_Bool boolean;
		struct
		{
			char* chars;
			int len;
		} str;
		//Lnn_Function* closure;
		char* variable;
		struct
		{
			char* identifier;
			int numargs;
			struct Lnn_ExprNode** args; /* Array of arguments */
		} functioncall;
	} u;
} Lnn_ExprNode;

/**
 * @brief Destroys an expression and all its child nodes recursively.
 * @param expr Expression to destroy.
 */
void Lnn_DestroyExpression(Lnn_ExprNode* expr);



/**
 * @brief Code blocks are containers for a list statements.
 * These are the statements that are in the same scope depth and are executed in order.
 * Scopes contain code blocks and are executed recursively from other code blocks.
 */
typedef struct Lnn_CodeBlock
{
	Utl_List statements; /* List of Lnn_Statement */
} Lnn_CodeBlock;

/**
 * @brief Destroys a code block and all its child nodes recursively.
 * @param block Block to destroy.
 */
void Lnn_DestroyCodeBlock(Lnn_CodeBlock* block);



typedef enum
{
	Lnn_ST_EXPRESSION,
	Lnn_ST_IF,
	Lnn_ST_FOR,
	Lnn_ST_WHILE,
	Lnn_ST_DOWHILE,
	Lnn_ST_RETURN,
	Lnn_ST_SCOPE,
	Lnn_NUM_STATEMENTTYPES
} Lnn_StatementType;
extern const char* lnn_statementtype_names[Lnn_NUM_STATEMENTTYPES];

typedef struct Lnn_Statement
{
	Utl_ListLinks links;
	Lnn_StatementType type;
	union
	{
		struct
		{
			Lnn_ExprNode* expression;
		} stmt_expr;
		struct
		{
			Lnn_ExprNode* expression;
		} stmt_return;
		struct
		{
			Lnn_ExprNode* condition;
			Lnn_CodeBlock* block_ontrue;
			Lnn_CodeBlock* block_onfalse;
		} stmt_if;
		struct
		{
			Lnn_ExprNode* init;
			Lnn_ExprNode* condition;
			Lnn_ExprNode* loop;
			Lnn_CodeBlock* block;
		} stmt_for;
		struct
		{
			Lnn_ExprNode* condition;
			Lnn_CodeBlock* block;
		} stmt_while;
		struct
		{
			Lnn_ExprNode* condition;
			Lnn_CodeBlock* block;
		} stmt_dowhile;
		struct
		{
			Lnn_CodeBlock* block;
		} stmt_scope;
	} u;
} Lnn_Statement;

/**
 * @brief Destroys a statement and all its child nodes recursively.
 * @param stmt Statement to destroy.
 */
void Lnn_DestroyStatement(Lnn_Statement* stmt);



void Lnn_PrintCodeTree(const Lnn_CodeBlock* block);

#endif