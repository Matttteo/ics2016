#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);


//My code
static int cmd_si(char *args){
	/* 
	   Do the single step excutions
	   Use the atoi(char*) function, include in <stdlib.h>
	*/
	int n = atoi(args);
	cpu_exec(n);
	return 0;
}
static void print_registers(){
	printf("%-10s%-#20x%-5d\n","eax",cpu.eax, cpu.eax );
	printf("%-10s%-#20x%-5d\n","ecx",cpu.ecx, cpu.ecx );
	printf("%-10s%-#20x%-5d\n","edx",cpu.edx, cpu.edx );
	printf("%-10s%-#20x%-5d\n","ebx",cpu.ebx, cpu.ebx );
	printf("%-10s%-#20x%-5d\n","esp",cpu.esp, cpu.esp );
	printf("%-10s%-#20x%-5d\n","ebp",cpu.ebp, cpu.ebp );
	printf("%-10s%-#20x%-5d\n","esi",cpu.esi, cpu.esi );
	printf("%-10s%-#20x%-5d\n\n","edi",cpu.edi, cpu.edi );
	printf("%-10s%-#20x%-5d\n","eip",cpu.eip, cpu.eip );
}
static int cmd_info(char *args){
	switch(args[0]){
		case 'r':
			//Print registers
			print_registers();
			break;
		default:
			printf("No implements\n");
			break;
	}
	return 0;
}
static int cmd_x(char *args){
	char *N = strtok(args," ");
	char *address = N + strlen(N) + 1;
	if(N == NULL){
		printf("Please Enter a number.\n");
		return -1;
	}
	if(address == NULL){
		printf("Please enter a correct address.\n");
		return -1;
	}
	int num = atoi(N);
	int add = (int) strtol(address, NULL, 16);

	int i;
	printf("%#16x:", add);
	for(i = 0; i < num; ++i){
		int m = swaddr_read(add + 4 * i, 4);
		printf("%#16x", m);
	}
	printf("\n");
	return 0;
}
static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	/* TODO: Add more commands */
	{ "si", "Single step execution", cmd_si },
	{ "info", "Print informations", cmd_info },
	{ "x", "Print memory", cmd_x},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */

		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
