#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
 * Runs the main program (passed as argv[1])
 * with preset input (passed as argv[2]) and
 * compares the output with keys (passed as argv[3])
 * and the exit status with argv[4].
 */

enum
{
    ARGC_NEEDED = 5
};

enum Errors
{
    NO_FILES_ERROR = 0x02,
    CHILD_ERROR = 0x03
};

int
cmp_files(FILE *f1, FILE *f2)
{
    int c1, c2;
    do {
        c1 = fgetc(f1);
        c2 = fgetc(f2);
        if (c1 != c2) {
            return 0;
        }
    } while (c1 != EOF && c2 != EOF);
    if (c1 == EOF && c2 == EOF) {
        return 1;
    }
    return 0;
}

void
exit_remove(void)
{
    remove("tmp_test_answers");
}

int
main(int argc, char **argv)
{
    if (argc < ARGC_NEEDED) {
        fprintf(stderr, "Not enough arguments!\n");
        _exit(NO_FILES_ERROR);
    }
    pid_t tester;
    if ((tester = fork()) < 0) {
        fprintf(stderr, "Failed to fork: %s\n", strerror(errno));
        _exit(errno);
    } else if (tester == 0) {
        int fd_out = open("tmp_test_answers", O_RDWR | O_CREAT | O_TRUNC, 0666);
        int fd_in = open(argv[2], O_RDONLY);
        if (fd_in < 0 || fd_out < 0) {
            fprintf(stderr, "Failed to open %s file: %s\n", (fd_in < 0) ? "input" :
                                                            "output", strerror(errno));
            _exit(errno);
        }
        dup2(fd_in, fileno(stdin));
        dup2(fd_out, fileno(stdout));
        dup2(fd_out, fileno(stderr));
        close(fd_in);
        close(fd_out);
        execl(argv[1], argv[1], (char *) NULL);
        fprintf(stderr, "Failed to execl: %s\n", strerror(errno));
        _exit(errno);
    }
    int status;
    waitpid(tester, &status, 0);
    if (!WIFEXITED(status)) {
        fprintf(stderr, "Failed\n");
        _exit(CHILD_ERROR);
    }
    FILE *test_answers = fopen("tmp_test_answers", "r");
    atexit(exit_remove);
    FILE *test_keys = fopen(argv[3], "r");
    if (test_keys == NULL || test_answers == NULL) {
        fprintf(stderr, "Failed to open: %s\n", strerror(errno));
        _exit(errno);
    }
    int cmp_ans = cmp_files(test_answers, test_keys);
    fclose(test_keys);
    fclose(test_answers);
    if (cmp_ans && WEXITSTATUS(status) == strtol(argv[4], NULL, 10)) {
        printf("OK!\n");
        return 0;
    } else {
        printf("Failed!\n");
        return 1;
    }
}
