#include "commando.h"

cmd_t *cmd_new(char *argv[]) {
	cmd_t *cmd = malloc(sizeof(cmd_t));
	int i = 0;
	while(argv[i] != NULL) {
			cmd->argv[i] = strdup(argv[i]);
			i++;
	}
	cmd->argv[i] = NULL;
	strcpy(cmd->name, argv[0]);
	cmd->finished = 0;
	snprintf(cmd->str_status, 5, "%s", "INIT");
	cmd->pid = -1;
	cmd->out_pipe[PREAD] = -1;
	cmd->out_pipe[PWRITE] = -1;
	cmd->status = -1;
	cmd->output = NULL;
	cmd->output_size = -1;
	return cmd;
}

void cmd_free(cmd_t *cmd) {
	int i = 0;
	while(cmd->argv[i] != NULL) {
		free(cmd->argv[i]);
		i++;
	}

	if(cmd->output != NULL)
	{
		free(cmd->output);
	}
	free(cmd);
}

void cmd_start(cmd_t *cmd)
{
	int pipe_result = pipe(cmd->out_pipe);
	if(pipe_result != 0) {
		perror("Failed to create pipe");
		exit(1);
	}
	snprintf(cmd->str_status, 4, "%s", "RUN");

	pid_t child = fork();
	if(child < 0) {
		perror("Failed to fork");
		exit(1);
	}
	cmd->pid = child;

	//child
	if(cmd->pid == 0) {
 		dup2(cmd->out_pipe[PWRITE], STDOUT_FILENO);
		close(cmd->out_pipe[PREAD]);
		execvp(cmd->name, cmd->argv);
	}
	//parent
	close(cmd->out_pipe[PWRITE]);
}

void cmd_update_state(cmd_t *cmd, int block)
{
	if (cmd->finished != 1)
	{
		int status;
		int pid = waitpid(cmd->pid, &status, block);

		if(pid == -1)
		{
			printf("Error");
			exit(1);
		}
		else if((pid == cmd->pid) && WIFEXITED(status))
		{
			cmd->finished = 1;
			int retval = WEXITSTATUS(status);
			cmd->status = retval;
			snprintf(cmd->str_status, STATUS_LEN, "%s%d%s", "EXIT(",retval,")");
			cmd_fetch_output(cmd);
			printf("@!!! %s[#%d]: %s\n",cmd->name, cmd->pid, cmd->str_status);
		}

	}
}

char *read_all(int fd, int *nread)
{
	int size = BUFSIZE;
	char *buf = malloc(size);
	//need to continulously call read in a loop until all bytes are read
	int bytes_read = 0;
	int pos = 0;

	while(1) {
		bytes_read = read(fd, buf+pos, size-pos);

		//break if bytes read is 0 (meaning that there's no more bytes left to ready in file)
		if(bytes_read == 0)
			break;
		pos += bytes_read;

		if (pos >= size) {
			buf = realloc(buf, 2*size);
			size = 2 * size;
		}

	}
	//sets integer pointed to by nread to number of bytes read
	*nread = pos;

	buf[pos] = '\0';
	return buf;
}

void cmd_fetch_output(cmd_t *cmd) {
	if(cmd->finished == 0) {
		printf("%s[#%d] not finished yet\n", cmd->name, cmd->pid);
		return;
	}
	else {
		cmd->output = read_all(cmd->out_pipe[PREAD], &cmd->output_size);
		close(cmd->out_pipe[PREAD]);
	}

}

void cmd_print_output(cmd_t *cmd) {
	if(cmd->output != NULL)
		write(STDOUT_FILENO, cmd->output, cmd->output_size);
	else
		printf("%s[#%d] : output not ready\n", cmd->name, cmd->pid);

}
