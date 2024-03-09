#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MaxSize 20
#define FIFO_FILE "myfifo"

int main(int argc, char *argv[]) {
    int filefd, countWords;
    char buf[MaxSize];

    // Sprawdź istnienie potoku nazwanego, jeśli nie istnieje, utwórz go
    if (access(FIFO_FILE, F_OK) == -1) {
        if (mkfifo(FIFO_FILE, 0666) != 0) {
            perror("Błąd podczas tworzenia potoku nazwanego");
            return 1;
        }
    }

    // Otwórz potok nazwany do zapisu
    int pipefd = open(FIFO_FILE, O_WRONLY);
    if (pipefd < 0) {
        perror("Błąd podczas otwierania potoku nazwanego do zapisu");
        return 1;
    }

    // Przetwarzaj pliki przekazane jako argumenty
    for (int i = 1; i < argc; i++) {
        // Otwórz plik do odczytu
        if ((filefd = open(argv[i], O_RDONLY)) < 0) {
            perror("Błąd podczas otwierania pliku");
            return 1;
        }

        // Odczytuj dane z pliku i zapisuj do potoku
        while ((countWords = read(filefd, buf, MaxSize)) > 0) {
            write(pipefd, buf, countWords);
        }

        // Zamknij plik
        close(filefd);

        // Oczekuj przez 5 sekund (opcjonalnie)
        sleep(5);
    }

    // Zamknij potok
    close(pipefd);

    return 0;
}