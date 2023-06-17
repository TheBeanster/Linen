#include "fab_utility.h"
#include "lnn_state.h"
#include "lnn_parse.h"



static char* read_code_from_file(const char* const filename)
{
	if (!filename) return;
	FILE* file = fopen(filename, "r");
	if (!file)
	{
		return NULL;
	}

	int length = 0;
	while (!feof(file))
	{
		fgetc(file);
		length++;
	}

	char* filebuffer = malloc(length + 1);
	rewind(file);
	int pos = 0;
	while (!feof(file))
	{
		char c = fgetc(file);
		filebuffer[pos] = c;
		pos++;
	}

	filebuffer[pos - 1] = '\0';
	fclose(file);
	return filebuffer;
}



int main(void)
{
	Lnn_State* state = Utl_AllocType(Lnn_State);

	const char* sourcecode = read_code_from_file("testcode.lnn");
	(void)Lnn_ParseSourceCode(state, sourcecode);
	Utl_Free(sourcecode);

	Utl_Free(state);

	return 0;
}
