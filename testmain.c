#include <stdio.h>

#include "fab_utility.h"
#include "lnn_code.h"
#include "lnn_parse.h"

int main(void)
{
	Lnn_State* state = Utl_AllocType(Lnn_State);
	(void)Lnn_ParseSourceCode(state, "if a == 0 then end");

	return 0;
}
