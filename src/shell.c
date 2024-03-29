#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "list.h"
#define MAXARG 40
#define LIMIT 10
#define QUIT 0
#define EXIT 1
#define HISTRY 2
int cnt;
char *pri_ins[16] = {"quit", "exit", "history",};
/*list */
static inline void initlist(void);
static inline void add_list(int, char **);
/* shell */
static inline void print_history(void);
static inline void handle_retval(int);
static inline int get_ins(char **, int);
static inline void handle_execfromhistory(char **, int *);
static inline int handler(char **, int);
struct history_commands {
	struct list_head run;
	int position;
	char **argv;
	int argc;
} hiscom;

enum RetValue{EXEC, HISTORY, EXECFROMHISROTY};
static inline int get_ins(char **argv, int argc)
{
	if (argc == 1) {
		int index = 0;
		while(pri_ins[index++]) {
			if(strcmp(pri_ins[index - 1], argv[0]) == 0)
				return index - 1;
		}
	}
	return -1;
}
static inline void handle_execfromhistory(char **argv, int *argc)
{
	if(*argc != 1) return ;	
	int position;
	if(strcmp(argv[0], "!!") == 0) {
		position = cnt - 1;
		if (cnt - 1 <= 0) {
			printf("No commands in history.\n");
			return ;
		}
	}else {
		char *p =argv[0];
		position = 0;
		p++;
		while (*p) {
			if(*p > '9' || *p < '0')
				return ;
			position = position * 10 + *p - '0';
			p++;
		}
	}

	//printf("position = %d\n", position);
	struct list_head *node;
	int count = 0;
	list_for_each(node, &hiscom.run) {
		struct history_commands *p = (struct history_commands *)node;
		count += 1;
		if (count > 10) {
			printf("You can only search for nearly ten instructions!\n");
			return ;
		}
		if (p->position == position) {
			*argc = p->argc;
			for (int i = 0; p->argv[i]; i++) {
				argv[i] = p->argv[i];
			}
			return ;
		}
	}
	printf("No such command in history.\n");

}	
static inline int handler(char **argv, int argc)
{	/*
	printf("will execute: ");
	
	for (int i = 0; i < index; i++) {
		printf("%s ", argv[i]);
	}
	*/

	/*handle instruction: history */
	int ins = get_ins(argv, argc);
	switch (ins) {
		case QUIT:
		case EXIT:
			printf("lshell is closing. Goodbye!\n");
			exit(0);
		case HISTRY:
			return HISTORY;	
		default:
			break;
	}

	/* handler "!!" or "!1" ... */
	if (argv[0][0] == '!')
		handle_execfromhistory(argv, &argc);
	/*
	for(int i = 0; argv[i]; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
		printf("argc = %d\n",argc);
	}
	*/

	/* handler '&' */
	int flag = 0;
	if (strcmp(argv[argc - 1], "&") == 0) {
		flag = 1;
		free(argv[argc - 1]);
		argv[argc - 1] = NULL;
		argc--;
	}


	if (fork() == 0) {
		if(execvp(argv[0], argv) < 0)
			printf("error message: %s failed\n", argv[0]);
		exit(0);
	} else {	
		add_list(cnt, argv);
		if (flag == 1) {
			// do nothing
		} else {
		      wait(NULL);			
		}
	}
	return EXEC;
}
/* init the list */
static inline void initlist(void)
{
	INIT_LIST_HEAD(&hiscom.run);
}
/* add a node to list */
static inline void add_list(int postion, char **argv)
{
	struct history_commands *p = (struct history_commands *) malloc(sizeof (struct history_commands));
	p->position = postion;
	p->argv = (char **)malloc(sizeof(char *) * MAXARG);
	int i;
    for (i = 0; i < MAXARG && argv[i] != NULL; ++i) {
        p->argv[i] = strdup(argv[i]);
    }
	p->argc = i;
	list_add(&p->run, &hiscom.run);
}
/* print history instructions */
/* lazy free the memory */
static inline void print_history(void)
{
	struct list_head *node;
	struct list_head *safe_node;
	struct list_head *tail;
	int count = 0;
	list_for_each_safe(node,safe_node, &hiscom.run) {
		struct history_commands *unode = (struct history_commands*)node;
		if (count < 10) {
			printf("%d ", unode->position);
			for (int i = 0; unode->argv[i]; i++) {
				printf("%s ", unode->argv[i]);
			}
			printf("\n");
			tail = node;
		}
		if (count >= 10) {
			tail->next = &hiscom.run;
			for (int i = 0; i < unode->argc; i++) {
				free(unode->argv[i]);
			}
			free(unode->argv);
			free(unode);
		}
		count += 1;
	}
}
/* handle instructinos like history or !!*/
static inline void handle_retval(int retval)
{
	switch(retval) {
		case HISTORY:
			print_history();
		default:
			break;

	}
}
int main() {
	printf("Welcome to my shell\n");
	initlist();
	char *argv[MAXARG];
	for (; ;) {
		cnt++;
		printf("lsh> ");
		fflush(stdout);
		memset(argv, 0, 40);
		char c;
		int index = 0;
		char buf[64];
		memset(buf, 0, 64);
		fgets(buf, 64, stdin);
		if (buf[0] == '\n' || buf[0] == '\0') continue;
		buf[strlen(buf) - 1] = '\0';
		//printf("%s", buf);
		char *token = strtok(buf, " ");
		while(token && index < MAXARG) {
			argv[index++] = strdup(token);
			token = strtok(NULL, " ");
		}
		if (index == 1 && strcmp(argv[0], "history") == 0)
			cnt--;

		handle_retval(handler(argv,index));
		/*
		for (int i = 0; i < index; i++) {
			printf("%s ", argv[i]);
		}*/
		//should_run = 0;
	}
	return 0;
}
