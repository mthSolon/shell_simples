#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void shell_loop();
char *read_line();
char **split_line(char *line);
int execute_command(char **args);

int main(int argc, char **argv) {
    // Start the shell loop
    shell_loop();
    return 0;
}

void shell_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = execute_command(args);

        free(line);
        free(args);
    } while (status);
}

char *read_line() {
    int buffer_size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int position = 0;
    int c;

    if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        
        // Lê o input até chegar no final da linha ou do arquivo
        if (c == EOF || c == '\n') {
            buffer[position] = '\0'; // adiciona um string terminator
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;
        
        // realoca caso precise
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);

            if (!buffer) {
                fprintf(stderr, "Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **split_line(char *line) {
    int buffer_size = BUFFER_SIZE;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;
    int position = 0;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    // separa a string em argumentos
    token = strtok(line, " ");

    while (token != NULL) {
        tokens[position] = token; // adiciona o argumento no array de argumentos
        position++;

        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " ");
    }

    tokens[position] = NULL;
    return tokens;
}

int execute_command(char **args) {
    pid_t pid;
    int status;
    
    // Se o comando é vazio, retorna sem fazer nada
    if (args[0] == NULL) {
        return 1;
    }
  
    // Fork um processo novo
    pid = fork();

    if (pid == 0) {
        // Processo filho executa o comando dado pelo usuário
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }

        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // falhou em fazer o fork
        perror("shell");
    } else {
        // processo pai espera o processo filho acabar
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
