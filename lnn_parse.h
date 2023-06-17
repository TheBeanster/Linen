#ifndef _Lnn_PARSE_H_
#define _Lnn_PARSE_H_

#include "fab_utility.h"
#include "lnn_code.h"
#include "lnn_state.h"

typedef char Lnn_TokenType;
enum
{
	Lnn_TT_KEYWORD,
	Lnn_TT_OPERATOR,
	Lnn_TT_SEPARATOR,
	Lnn_TT_NUMBERLITERAL,
	Lnn_TT_STRINGLITERAL,
	Lnn_TT_IDENTIFIER,
	Lnn_TT_NULL = -1,		/* Invalid token */
};

typedef struct Lnn_Token
{
	Utl_ListLinks links;

	Lnn_TokenType	type;
	Lnn_KeywordID	keywordid;
	Lnn_OperatorID	operatorid;
	Lnn_SeparatorID separatorid;

	char* string;
	unsigned short linenum;
	short lastonline; /* If this token is the last on a line */
} Lnn_Token;

void Lnn_PrintToken(const Lnn_Token* token);
void Lnn_DestroyToken(Lnn_Token* token);



/* Maximum number of characters sourcecode can be */
#define Lnn_MAX_SOURCECODE_LENGTH 200

/**
 * @brief Reads through a string character by character and divides it into separate tokens.
 * @param state State to parse in.
 * @param tokens Pointer to an empty list to put the tokens into.
 * @param sourcecode Pointer to a string with Lnn source code.
 * @return The number of errors found.
 */
int Lnn_ParseSourceCodeTokens(Lnn_State* state,
							  Utl_List* tokens,
							  const char* sourcecode);

Lnn_CodeBlock* Lnn_ParseSourceCode(Lnn_State* state,
								   const char* sourcecode);

void Lnn_PrintCodeTree(const Lnn_CodeBlock* code);

void Lnn_PrintSourceCode(const char* sourcecode);

#endif