#include "../inc/utils.h"
#include <argp.h>
#include <elf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const char *argp_program_version = "db 1.0";

const char *argp_program_bug_address = "<vitalik16200@gmail.com>";

static char doc[] = "db -- a simple database for storing records in a file.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

static struct argp_option options[] = {
	{"verbose",  'v', 0,        0,  "Produce verbose output" },
	{"addr",     'a', 0,        0,  "Address of the function main" },
	{"output",   'o', "FILE",   0,  "Output"},
	{"file",     'f', "FILE",   0,  "FILE database. Defaut name (main)" },
	{ 0 }
};

typedef struct elf {
	Elf32_Ehdr *header;
	Elf32_Shdr *symbol;
	Elf32_Sym *symtab;
	uint32_t *code;
	char *shstrtab;
	char *strtab;
}elf_t;

struct arguments {
	int verbose, addr;
	char *file;
	char *output;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
		case 'v':
			arguments->verbose = 1;
			break;
		case 'a':
			sscanf(arg, "%d", &arguments->addr);
			break;
		case 'o':
			arguments->output = arg;
			break;
		case 'f':
			arguments->file = arg;
			break;
		 default:
			return ARGP_ERR_UNKNOWN;
			break;
	}
	return 0;
}

/* argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

enum STATE read_elf(elf_t *elf, const char *elfFile)
{
	Elf32_Ehdr *header;
	Elf32_Shdr *symbol;
	Elf32_Sym *symtab;
	uint32_t *code;
	char *shstrtab;
	char *strtab;
	FILE *file;
	size_t number;

	file = fopen(elfFile, "rb");
	if(NULL == file)
		return STATE_EOP;

	/* read header */
	header = malloc(sizeof(*header));
	fread(header, sizeof(*header), 1, file);

	/* read symbol */
	number = header->e_shstrndx + 1;
	fseek(file, header->e_shoff, SEEK_SET);
	symbol = malloc(sizeof(*symbol) * number);
	fread(symbol, sizeof(*symbol), number, file);
	
	/* read shstrtab */
	number--;
	fseek(file, symbol[number].sh_offset, SEEK_SET);
	shstrtab = malloc(symbol[number].sh_size);
	fread(shstrtab, 1, symbol[number].sh_size, file);

	/* read strtab */
	number--;
	fseek(file, symbol[number].sh_offset, SEEK_SET);
	strtab = malloc(symbol[number].sh_size);
	fread(strtab, 1, symbol[number].sh_size, file);

	/* read symtab */
	number--;
	fseek(file, symbol[number].sh_offset, SEEK_SET);
	symtab = malloc(symbol[number].sh_size);
	fread(symtab, 1, symbol[number].sh_size, file);

	/* read .text */
	fseek(file, symbol[1].sh_offset, SEEK_SET);
	code = malloc(symbol[1].sh_size);
	fread(code, sizeof(*code), symbol[1].sh_size / sizeof(*code), file);

	fclose(file);

	elf->header = header;
	elf->symbol = symbol;
	elf->symtab = symtab;
	elf->code = code;

	elf->shstrtab = shstrtab;
	elf->strtab = strtab;
	
	return STATE_OK;
}

Elf32_Sym *find_func(Elf32_Sym *symtab, const size_t value, const size_t size)
{
	typeof(symtab->st_info) info;
	size_t i;
	const size_t number = size / sizeof(*symtab);

	for (i = 0; i < number; i++) {
		info = ELF32_ST_TYPE(symtab[i].st_info);
		if (STT_FUNC == info && value == symtab[i].st_value)
			return &symtab[i];
	}

	return NULL;
}

enum STATE set_addr(elf_t *elf, const int addr_main)
{

	return STATE_OK;
}

enum STATE write_output(elf_t *elf, const char *outFile)
{
	FILE *file;
	const size_t number = elf->symbol[1].sh_size / 4;
	const size_t number_symtab = elf->symbol[elf->header->e_shstrndx-2].sh_size;
	size_t i;
	Elf32_Sym *temp;

	file = fopen(outFile, "w");

	for (i = 0; i < number; i++) {
		temp = find_func(elf->symtab, i * 4, number_symtab);
		if (NULL != temp)
			fprintf(file, "// 0x%.8lx: %s\n", i * 4, &elf->strtab[temp->st_name]);
		fprintf(file, "%.8x\n", elf->code[i]);
	}

	fclose(file);

	return STATE_OK;
}

int main(int argc, char **argv)
{
	enum STATE state;
	elf_t elf;
	//struct database *temp;
	//struct list_t *pos;
	//struct list_t *t;
	struct arguments arguments;

	/* Set arguments defaults */
	arguments.file = "main";
	arguments.output = "a.out";

	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	/*
	 *   read file
	 */
	state = read_elf(&elf, arguments.file);
	if (STATE_OK != state) {
		print_error(state, arguments.verbose);
		goto end;
	}

	set_addr(&elf, arguments.addr);
	write_output(&elf, arguments.output);

	free(elf.header);
	free(elf.symbol);
	free(elf.symtab);
	free(elf.code);

	free(elf.strtab);
	free(elf.shstrtab);

end:
	exit(state);
}

