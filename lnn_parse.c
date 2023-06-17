#include "lnn_parse.h"



static Lnn_CodeBlock* parse_codeblock(Lnn_State* state,
									  const Lnn_Token* begin,
									  const Lnn_Token** end);



static Lnn_ExprNode* parse_expression(Lnn_State* state,
									  const Lnn_Token* begin,
									  const Lnn_Token** end,
									  const Utl_Bool readendline)
{
	Utl_Assert(state);
	Utl_Assert(begin);
	Utl_Assert(end);

	Lnn_ExprNode* node = Utl_AllocType(Lnn_ExprNode);
	*end = begin->links.next;
	return node;
}



static Lnn_Statement* parse_if_statement(Lnn_State* state,
										 const Lnn_Token* begin,
										 const Lnn_Token** end)
{
	Utl_Assert(state);
	Utl_Assert(begin);
	Utl_Assert(end);

	Lnn_ExprNode* condition = NULL;
	Lnn_CodeBlock* block_ontrue = NULL;
	Lnn_CodeBlock* block_onfalse = NULL;

	const Lnn_Token* i = (const Lnn_Token*)begin->links.next;
	if (!i)
	{
		printf("ERROR! If statement doesn't have an end\n"); return NULL;
	}
	condition = parse_expression(state, i, &i, Utl_FALSE);
	if (!condition)
	{
		printf("ERROR! Couldn't parse if statement condition\n"); return NULL;
	}
	if (i == NULL || i->keywordid != Lnn_KW_THEN)
	{
		printf("ERROR! If statement is missing the 'then' keyword\n"); goto on_fail;
	}

	i = (Lnn_Token*)i->links.next;
	block_ontrue = parse_codeblock(state, i, &i);
	if (!block_ontrue) goto on_fail;
	if (i == NULL || !(i->keywordid == Lnn_KW_ELSE || i->keywordid == Lnn_KW_END))
	{
		printf("ERROR! If statement doesn't have any code block\n"); goto on_fail;
	}

	if (i->keywordid == Lnn_KW_ELSE) /* If there is an else statement */
	{
		/* Parse the on false block */
		i = (Lnn_Token*)i->links.next;
		block_onfalse = parse_codeblock(state, i, &i);
		if (!block_onfalse) goto on_fail;
		if (i == NULL || i->keywordid != Lnn_KW_END)
		{
			printf("ERROR! If statement doesn't have an end\n"); goto on_fail;
		}
	}

	Lnn_Statement* stmt = Utl_AllocType(Lnn_Statement);
	stmt->type = Lnn_ST_IF;
	stmt->u.stmt_if.block_ontrue = block_ontrue;
	stmt->u.stmt_if.block_onfalse = block_onfalse;
	stmt->u.stmt_if.condition = condition;
	*end = (const Lnn_Token*)i->links.next;
	return stmt;

on_fail:
	Lnn_DestroyExpression(condition);
	Lnn_DestroyCodeBlock(block_ontrue);
	Lnn_DestroyCodeBlock(block_onfalse);
	return NULL;
}



/**
 * @brief Parses a statement and puts the token that comes after it in the end param.
 * It doesn't matter how the statement ends, as long as it is valid, the new statement will return.
 * It is up to the callee to handle what the end token is found to be. If you expect it to be something then check the end token.
 * @param state Program state for error logs.
 * @param begin The token where the statement begins.
 * @param end Pointer to the token found to end this statement.
 * If you expect there to be something specific after the statement, then check this.
 * @return Pointer to the new parsed statement.
 */
static Lnn_Statement* parse_statement(Lnn_State* state,
									  const Lnn_Token* begin,
									  const Lnn_Token** end)
{
	Utl_Assert(state);
	Utl_Assert(begin);
	Utl_Assert(end);

	switch (begin->keywordid)
	{
	case Lnn_KW_IF: return parse_if_statement(state, begin, end);
		

	case Lnn_KW_END:
	case Lnn_KW_ELSE:
		*end = begin; /* Statements can't start with these keywords */
		return NULL;

	default:
		/* Statement doesn't start with a keyword */

		break;
	}
}



/**
 * @brief Parses a codeblock consisting of multiple statement ending on *any* invalid token.
 * @param state Program state for error logs.
 * @param begin The first token of the codeblock.
 * @param end The token found to end the codeblock. Will be NULL if the sourcecode ended!
 * @return Pointer to the parsed code block ,or NULL if it failed to parse.
 */
static Lnn_CodeBlock* parse_codeblock(Lnn_State* state,
									  const Lnn_Token* begin,
									  const Lnn_Token** end)
{
	Utl_Assert(state);
	Utl_Assert(begin);
	Utl_Assert(end);

	Lnn_CodeBlock* block = Utl_AllocType(Lnn_CodeBlock);
	for (const Lnn_Token* i = begin; i;)
	{
		Lnn_Token* nexttoken = NULL;
		Lnn_Statement* stmt = parse_statement(state, i, &nexttoken);
		if (!stmt)
		{
			*end = nexttoken;
			return block;
		}
		Utl_PushBackList(&block->statements, stmt);
		i = nexttoken;
	}
	/* Reached end of file */
	*end = NULL;
	return block;
}



Lnn_CodeBlock* Lnn_ParseSourceCode(Lnn_State* state, const char* sourcecode)
{
	Utl_Assert(state && sourcecode);
	
	Utl_List tokens = { 0 };
	Lnn_ParseSourceCodeTokens(state, &tokens, sourcecode);

	printf("Tokens:\n");
	for (Lnn_Token* i = (Lnn_Token*)tokens.begin; i; i = (Lnn_Token*)i->links.next)
	{
		Lnn_PrintToken(i);
		if (!i->lastonline)
			putchar(' ');
	}
	putchar('\n');
	/* The source code is now separated into tokens */

	/* If it couldn't lex the tokens then it probably shouldn't also be parsed */
	if (tokens.count <= 0) return NULL;

	Lnn_Token* endtoken = NULL;
	Lnn_CodeBlock* block = parse_codeblock(state, (Lnn_Token*)tokens.begin, &endtoken);
	if (!block)
	{
		printf("ERROR! Coudln't parse top level codeblock!\n");
		goto on_fail;
	}
	
	if (endtoken != NULL)
	{
		//Lnn_PUSHTOKENERROR(endtoken, "Sourcecode parsing ended early");
		printf("ERROR! Invalid source code end on line %i with token ", endtoken->linenum);
		Lnn_PrintToken(endtoken);
		putchar('\n');
		Lnn_DestroyCodeBlock(block);
		block = NULL;
	} else
		Lnn_PrintCodeTree(block);

	Utl_ClearList(&tokens, &Lnn_DestroyToken);

	//printf("\n\n   MESSAGES\n");
	//Lnn_PrintAllStateMessages(state);

	return block;

on_fail:
	Utl_ClearList(&tokens, &Lnn_DestroyToken);
	return NULL;
}
