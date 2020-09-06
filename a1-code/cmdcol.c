#include "commando.h"

void cmdcol_print(cmdcol_t *col) {
  printf("JOB  #PID      STAT   STR_STAT OUTB COMMAND\n");
  for(int i = 0; i < col->size; i++) {
    printf("%-4d ", i);
    printf("#%-8d ", col->cmd[i]->pid);
    printf("%4d ", col->cmd[i]->status);
    printf("%10s ", col->cmd[i]->str_status);
    printf("%4d ", col->cmd[i]->output_size);

    for(int j = 0; col->cmd[i]->argv[j] != NULL; j++) {
      printf("%s ", col->cmd[i]->argv[j]);
    }
    printf("\n");
  }
}

void cmdcol_add(cmdcol_t *col, cmd_t *cmd) {
  if(col->size == MAX_CMDS) {
      perror("Exceeded MAX_CMDS\n");
      exit(1);
  }
  else {
    col->cmd[col->size] = cmd;
    col->size++;
  }

}

void cmdcol_update_state(cmdcol_t *col, int block) {
  for(int i = 0; i < col->size; i++) {
    cmd_update_state(col->cmd[i], block);
  }

}

void cmdcol_freeall(cmdcol_t *col) {
	for (int i =0; i < col->size; i++){
    cmd_free(col->cmd[i]);
	}
}
