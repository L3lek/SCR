#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MaxSize 20


int main(int argc,char *argv[]){

	int pipefd[2],filefd,countWords;
	char buf[MaxSize];

	if (pipe(pipefd) == -1) {
		fprintf(stderr,"Błąd podczas tworzenia potoku\n");
		return 1;
	}

	pid_t pid = fork();

	if(pid <0){
		fprintf(stderr,"Błąd podczas tworzenia procesu potomnego\n");
		return 1;

	}else if(pid==0){
		close(pipefd[1]);

		while((countWords=read(pipefd[0],buf,MaxSize))>0){
			printf("@@@@%.*s#### ", countWords, buf);
		}

		close(pipefd[0]);

	}else{
		close(pipefd[0]);

		if((filefd=open(argv[1],O_RDONLY)) < 0){
			fprintf(stderr,"Błąd podczas otwierania pliku\n");
			return 1;
		}

		while ((countWords=read(filefd,buf,MaxSize))>0) {
            write(pipefd[1], buf, countWords);
        }

		close(pipefd[1]);
	}
}