#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MaxSize 20

int main(int argc, char *argv[]) {
    int pipefd[2], filefd;
    char buf[MaxSize];

    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <ścieżka_do_pliku_obrazu>\n", argv[0]);
        return 1;
    }

    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Błąd podczas tworzenia potoku\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Błąd podczas tworzenia procesu potomnego\n");
        return 1;
    } else if (pid == 0) {
        close(pipefd[1]);
        // Przekierowanie zawartości potoku na standardowe wejście
        close(0);
        dup(pipefd[0]);
        close(pipefd[0]);

        // Uruchom program do wyświetlania obrazków
        execlp("display", "display", "-", NULL);
        perror("Błąd exec");
        return 1;
    } else {
        close(pipefd[0]);

        if ((filefd = open(argv[1], O_RDONLY)) < 0) {
            fprintf(stderr, "Błąd podczas otwierania pliku\n");
            return 1;
        }

        while (1) {
            ssize_t bytesRead = read(filefd, buf, MaxSize);
            if (bytesRead <= 0) {
                // Koniec pliku, zamknij potok
                close(pipefd[1]);
                break;
            }

            // Zapisz dane do potoku
            write(pipefd[1], buf, bytesRead);
        }

        close(filefd);
    }

    return 0;
}