#include "lnn_parse.h"



void Lnn_PrintToken(const Lnn_Token* token)
{
	if (!token) return;
	switch (token->type)
	{
	case Lnn_TT_KEYWORD:		printf("%s", lnn_keywordid_names[token->keywordid]); break;
	case Lnn_TT_OPERATOR:		printf("%s", lnn_operatorid_names[token->operatorid]); break;
	case Lnn_TT_SEPARATOR:		printf("%s", lnn_separatorid_names[token->separatorid]); break;
	case Lnn_TT_NUMBERLITERAL:	printf("%s", token->string); break;
	case Lnn_TT_STRINGLITERAL:	printf("\"%s\"", token->string); break;
	case Lnn_TT_IDENTIFIER:		printf("%s", token->string); break;
	default:
		printf("invalid");
		break;
	}
	if (token->lastonline) putchar('\\');
}

void Lnn_DestroyToken(Lnn_Token* token)
{
	Utl_Assert(token);
	if (token->string)
		Utl_Free(token->string);
	Utl_Free(token);
}



typedef enum
{
	CT_NULL,		/* Default type or any char not recognised */
	CT_ALPHA,		/* Letter or underscore */
	CT_NUMBER,
	CT_POINT,
	CT_OPERATOR,
	CT_SEPARATOR,
	CT_SPACER,		/* Space or tab */
	CT_QUOTE,		/* Quotation marks for strings */
	CT_COMMENT,		/* Comments start with # and end with an endline */
	CT_ENDLINE,
} chartype;

#define Lnn_IsAlpha(c) (isalpha(c) || c == '_')
#define Lnn_IsOperatorChar(c) (strchr("+-/*=<>!&|^", c))
#define Lnn_IsQuote(c) (c == '\"' || c == '\'')

static chartype check_chartype(const char c)
{
	if (Lnn_IsAlpha(c))				return CT_ALPHA;
	if (isdigit(c))					return CT_NUMBER;
	if (c == '.')					return CT_POINT;
	if (strchr("+-/*=<>!&|^", c))	return CT_OPERATOR;
	if (strchr("()[]{},;", c))		return CT_SEPARATOR;
	if (isblank(c))					return CT_SPACER;
	if (c == '\n' || c == ';')		return CT_ENDLINE; /* Semicolon acts the same as endline */
	if (Lnn_IsQuote(c))				return CT_QUOTE;
	if (c == '#')					return CT_COMMENT;
	if (c == '\n')					return CT_ENDLINE;
	return CT_NULL;
}



static Lnn_Token* create_token(void)
{
	Lnn_Token* token = Utl_Malloc(sizeof(Lnn_Token));
	token->links.prev = token->links.next = NULL;
	token->type = Lnn_TT_NULL;
	token->keywordid = Lnn_KW_NULL;
	token->operatorid = Lnn_OP_NULL;
	token->separatorid = Lnn_SP_NULL;
	token->string = NULL;
	token->lastonline = Utl_FALSE;
	return token;
}



#define get_char				\
	char c = sourcecode[i];		\
	if (Utl_CharIsInvalid(c))	\
	{							\
		/* Push error */		\
		return ~(i + 1);		\
	}



static int read_alpha_token(Utl_List* tokens,
							const char* sourcecode,
							const int start,
							const int linenum)
{
	int end = 0;
	for (int i = start + 1; i < Lnn_MAX_SOURCECODE_LENGTH; i++)
	{
		get_char;
		if (!Lnn_IsAlpha(c) && !isdigit(c))
		{
			end = i;
			break;
		}
	}
	Lnn_Token* token = create_token();
	char* cutstring = Utl_CopyCutString(sourcecode, start, end - start);
	token->keywordid = Lnn_GetKeyword(cutstring);
	if (token->keywordid == Lnn_KW_NULL)
	{
		token->type = Lnn_TT_IDENTIFIER;
		token->string = cutstring;
	} else
	{
		token->type = Lnn_TT_KEYWORD;
		token->string = NULL;
		Utl_Free(cutstring);
	}
	token->linenum = (unsigned short)linenum;
	Utl_PushBackList(tokens, (Utl_ListLinks*)token);
	return end;
}



static int read_number_token(Lnn_State* state,
							 Utl_List* tokens,
							 const char* sourcecode,
							 int start,
							 int linenum)
{
	Utl_Bool pointfound = Utl_FALSE; /* For checking if there are two decimal points in one number */
	int end = 0;
	for (int i = start + 1; i < Lnn_MAX_SOURCECODE_LENGTH; i++)
	{
		get_char;
		if (c == '.')
		{
			if (pointfound)
			{
				//Lnn_PUSHCONSTSYNTAXERROR("Two decimal points in one number");
				return ~(i + 1);
			}
			pointfound = Utl_TRUE;
		} else if (!isdigit(c))
		{
			if (Lnn_IsAlpha(c))
			{
				//Lnn_PUSHSYNTAXERROR("Letter character '%c' directly after number", sourcecode[i]);
				return ~(i + 1);
			}
			end = i;
			break;
		}
	}
	Lnn_Token* token = create_token();
	token->string = Utl_CopyCutString(sourcecode, start, end - start);
	token->type = Lnn_TT_NUMBERLITERAL;
	token->linenum = (unsigned short)linenum;
	Utl_PushBackList(tokens, (Utl_ListLinks*)token);
	return end;
}



static int read_operator_token(Lnn_State* state,
							   Utl_List* tokens,
							   const char* sourcecode,
							   const int start,
							   const int linenum)
{
	int end = 0;
	for (int i = start + 1; i < Lnn_MAX_SOURCECODE_LENGTH; i++)
	{
		get_char;
		if (!Lnn_IsOperatorChar(c))
		{
			end = i;
			break;
		}
	}
	char* cutstring = Utl_CopyCutString(sourcecode, start, end - start);
	Lnn_OperatorID op = Lnn_GetOperator(cutstring);
	if (op == Lnn_OP_NULL)
	{
		//Lnn_PUSHSYNTAXERROR("Invalid operator '%s'", cutstring);
		Utl_Free(cutstring);
		return ~end;
	}
	Utl_Free(cutstring);
	Lnn_Token* token = create_token();
	token->operatorid = op;
	token->type = Lnn_TT_OPERATOR;
	token->linenum = (unsigned short)linenum;
	Utl_PushBackList(tokens, (Utl_ListLinks*)token);
	return end;
}



static int read_separator_token(Lnn_State* state,
								Utl_List* tokens,
								const char* sourcecode,
								const int start,
								const int linenum)
{
	Lnn_OperatorID sp = Lnn_GetSeparator(sourcecode[start]);
	if (sp == Lnn_SP_NULL)
	{
		//Lnn_PUSHSYNTAXERROR("Invalid separator '%c'", sourcecode[start]);
		return ~(start + 1);
	}
	Lnn_Token* token = create_token();
	token->separatorid = sp;
	token->type = Lnn_TT_SEPARATOR;
	token->linenum = (unsigned short)linenum;
	Utl_PushBackList(tokens, (Utl_ListLinks*)token);
	return start + 1;
}



static int read_string_token(Lnn_State* state,
							 Utl_List* tokens,
							 const char* sourcecode,
							 const int start,
							 const int linenum)
{
	int end;
	for (int i = start + 1;; i++)
	{
		get_char;
		if (Lnn_IsQuote(c))
		{
			end = i + 1; /* Plus 1 to include quote mark */
			break;
		} else if (c == '\n' || c == '\0')
		{
			//Lnn_PUSHCONSTSYNTAXERROR("String doesn't have closing quote mark");
			return ~(i + 1);
		}
	}
	Lnn_Token* token = create_token();
	token->string = Utl_CopyCutString(sourcecode, start + 1, end - start - 2);
	token->type = Lnn_TT_STRINGLITERAL;
	token->linenum = (unsigned short)linenum;
	Utl_PushBackList(tokens, (Utl_ListLinks*)token);
	return end;
}



static int read_comment(const char* sourcecode,
						const int start)
{
	for (int i = start + 1; i < Lnn_MAX_SOURCECODE_LENGTH; i++)
	{
		get_char;
		if (c == '\n')
			return i + 1; /* Plus 1 to skip \n */
		else if (c == '\0')
			return i;
	}
}





int Lnn_ParseSourceCodeTokens(Lnn_State* state,
							  Utl_List* tokens,
							  const char* sourcecode)
{
	Utl_Assert(state);
	Utl_Assert(tokens);
	Utl_Assert(sourcecode);

	int	linenum = 1;
	int numerrors = 0;
	int i = 0;
	while (1)
	{
		if (i >= Lnn_MAX_SOURCECODE_LENGTH) /* Don't exceed max length */
		{
			printf("ERROR! Sourcecode exceeds max length of " Utl_Stringify(Lnn_MAX_SOURCECODE_LENGTH) " characters.\n");
			break;
		}

		char c = sourcecode[i];
		if (c == '\0') break;
		if (c < 0)
		{
			printf("ERROR! Source code contains invalid character on line %i. Linen only supports ASCII.\n", linenum);
			i++; continue;
		}

		switch (check_chartype(c))
		{
		case CT_ALPHA:		i = read_alpha_token(tokens, sourcecode, i, linenum); break;
		case CT_NUMBER:		i = read_number_token(state, tokens, sourcecode, i, linenum); break;
		case CT_POINT:		i++; continue; /* No need to check if token is invalid */
		case CT_OPERATOR:	i = read_operator_token(state, tokens, sourcecode, i, linenum); break;
		case CT_SEPARATOR:	i = read_separator_token(state, tokens, sourcecode, i, linenum); break;
		case CT_SPACER:		i++; continue; /* No need to check if token is invalid */
		case CT_QUOTE:		i = read_string_token(state, tokens, sourcecode, i, linenum); break;
		case CT_COMMENT:	i = read_comment(sourcecode, i); continue;
		case CT_ENDLINE:
			linenum++;
			if (sourcecode[i + 1] != '\\' && tokens->end) /* Backslash negates endline */
				((Lnn_Token*)tokens->end)->lastonline = Utl_FALSE;
			i++;
			continue;

		case CT_NULL:
		default: /* Invalid character */
		{
			//Lnn_PUSHSYNTAXERROR("Invalid character '%c'", c);
		}
		}

		if (i <= 0) /* Token invalid */
		{
			i = ~i;
			numerrors++;
		}
	}
	return numerrors;
}
