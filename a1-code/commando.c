#include "commando.h"

void helpMessage();

int main(int argc, char** argv) {
  setvbuf(stdout, NULL, _IONBF, 0); // Turn off output buffering
  char commands[MAX_LINE];
  char *tokens[MAX_CMDS];
  int num;

  cmdcol_t temp = {};
  cmdcol_t *col = &temp;

  int echo_set = 0;	//echo flag
  if(argc > 1) {
    if (strcmp(argv[1], "--echo") == 0)
      echo_set=1;	//if the command has echo as option, set the flag
  }
  if(getenv("COMMANDO_ECHO"))
    echo_set = 1;	//if environment variable COMMANDO_ECHO exists, set the flag

  while(1) {
	printf("@> ");
    char* input = fgets(commands, MAX_LINE, stdin);	//capture command line input

    if(input == NULL) {
      printf("\nEnd of input\n");
      break;
    }
    parse_into_tokens(commands, tokens, &num);	// Parse the contents of input_command so that tokens[i] will point to
																						// the ith space-separated string in it. Set num to the number of
																						// tokens that are found.

    if (echo_set)
    {
      int i = 0;
      while(tokens[i] != NULL) {
        printf("%s ", tokens[i]);
        i++;
      }
      printf("\n");
    }

  if(tokens[0] == NULL)
      ;	//if no input, do nothing

	//built-ins
    else if(strncmp(tokens[0], "help", MAX_LINE) == 0)
      helpMessage();
    else if (strncmp(tokens[0], "exit", MAX_LINE) == 0)
      break;
    else if (strncmp(tokens[0], "list", MAX_LINE) == 0)
      cmdcol_print(col);
    else if (strncmp(tokens[0], "pause", MAX_LINE) == 0)
      pause_for(atol(tokens[1]), atoi(tokens[2]));
    else if (strncmp(tokens[0], "output-for", MAX_LINE) == 0) {
      printf("@<<< Output for %s[#%d] (%d bytes):\n", col->cmd[atoi(tokens[1])]->name, col->cmd[atoi(tokens[1])]->pid, col->cmd[atoi(tokens[1])]->output_size);
      printf("%s\n", "----------------------------------------");
      cmd_print_output(col->cmd[atoi(tokens[1])]);
      printf("%s\n", "----------------------------------------");
    }
    else if (strncmp(tokens[0], "output-all", MAX_LINE) == 0) {
      for(int i = 0; i < col->size; i++) {
        printf("@<<< Output for %s[#%d] (%d bytes):\n", col->cmd[i]->name, col->cmd[i]->pid, col->cmd[i]->output_size);
        printf("%s\n", "----------------------------------------");
        cmd_print_output(col->cmd[i]);
        printf("%s\n", "----------------------------------------");
      }
    }
    else if (strncmp(tokens[0], "wait-for", MAX_LINE) == 0) {
      int job_num = atoi(tokens[1]);
      cmd_update_state(col->cmd[job_num], DOBLOCK);
    }
    else if (strncmp(tokens[0], "wait-all", MAX_LINE) == 0) {
      cmdcol_update_state(col, DOBLOCK);
    }
	//add new command in collection
    else {
      cmd_t *cmd = cmd_new(tokens);
      cmdcol_add(col, cmd);
      cmd_start(cmd);
    }
    cmdcol_update_state(col,NOBLOCK);	//check on status of all commands
  }
  cmdcol_freeall(col);

  return 0;
}

void helpMessage() {
  printf("COMMANDO COMMANDS \n");
  printf("help               : show this message\n");
  printf("exit               : exit the program\n");
  printf("list               : list all jobs that have been started giving information on each\n");
  printf("pause nanos secs   : pause for the given number of nanseconds and seconds\n");
  printf("output-for int     : print the output for given job number\n");
  printf("output-all         : print output for all jobs\n");
  printf("wait-for int       : wait until the given job number finishes\n");
  printf("wait-all           : wait for all jobs to finish\n");
  printf("command arg1 ...   : non-built-in is run as a job\n");
}
