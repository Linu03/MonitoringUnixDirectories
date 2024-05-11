#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_DIR 10
#define SIZE 1024

struct stat stat_buffer;

int contor_fisiere_corupte = 0;

int comparare_snapshot(char sn1[], int fd1, char sn2[], int fd2) {
    fstat(fd1, &stat_buffer);
    int size1 = stat_buffer.st_size;
    fstat(fd2, &stat_buffer);
    int size2 = stat_buffer.st_size;

    if (size1 != size2) {
        return 1;
    } else {
        char buf1[SIZE] = "\0";
        char buf2[SIZE] = "\0";
        int count1;
        int count2;
        lseek(fd1, 0, SEEK_SET);
        lseek(fd2, 0, SEEK_SET);
        while ((count1 = read(fd1, buf1, SIZE)) > 0 && (count2 = read(fd2, buf2, SIZE)) > 0) {
            if (strlen(buf1) != strlen(buf2)) {
                return 1;
            }
            for (int i = 0; i < strlen(buf1); i++) {
                if (buf1[i] != buf2[i]) {
                    return 1;
                }
            }
        }
    }

    return 0; // identice
}

void createSnapshotRecursive(const char *cale, int snapshotFile,char* izolare) {
  pid_t w;
  int w_status;
  int pid;
    struct stat fileInfo;
    if (lstat(cale, &fileInfo) == -1) {
        perror("Eroare info fisier \n");
        return;
    }

    if (S_ISREG(fileInfo.st_mode)) {
        if ((fileInfo.st_mode & S_IRWXU) == 0 && (fileInfo.st_mode & S_IRWXG) == 0 && (fileInfo.st_mode & S_IRWXO) == 0) {
	  //printf("Fișierul %s are drepturile setate la 000.\n", cale);
	  int pipefd[2];
	  if (pipe(pipefd) < 0 ){
	    perror("pipe");
	    exit(-1);
	  }
	  pid = fork();
	  if (pid == 0){
	    close(pipefd[0]);
	    dup2(pipefd[1],1);
	    execlp("./script.sh", "script.sh", cale, NULL);
	    perror("eroare");
	    exit(-1);
	  }else{
	    close(pipefd[1]);
	    char aux[150]= "";
	    read(pipefd[0],aux,sizeof(aux));
	    //printf("%s",aux);

	    // inlocuim enter cu \0
	    aux[strcspn(aux, "\n")]= '\0';

	    if(strcmp(aux, "SAFE") != 0){
	      contor_fisiere_corupte ++;
	      char noua_cale[400] = "";   char numele_fisierului[100]= "";
	      char* p = NULL;
	      if ((p=strchr(cale, '/')) != 0){
		strcpy(numele_fisierului, p+1);
	      }
	      snprintf(noua_cale, sizeof(noua_cale), "%s/%s", izolare, numele_fisierului);
	      rename(cale, noua_cale);
	    }
	    
	  }
	  w = wait(&w_status);
	  if(w == -1){
	    perror("wait");
	    exit(-1);
	  }
        }
    }
    
    char buffer[2048];
    int bytes_written = snprintf(buffer, sizeof(buffer), "Cale: %s\n", cale);
    write(snapshotFile, buffer, bytes_written);

    bytes_written = snprintf(buffer, sizeof(buffer), "Dimensiune: %ld\n", fileInfo.st_size);
    write(snapshotFile, buffer, bytes_written);

    bytes_written = snprintf(buffer, sizeof(buffer), "Ultima modificare: %s", ctime(&fileInfo.st_mtime));
    write(snapshotFile, buffer, bytes_written);

    bytes_written = snprintf(buffer, sizeof(buffer), "Inode: %ld\n", fileInfo.st_ino);
    bytes_written = snprintf(buffer, sizeof(buffer), "Drepturi: ");
    write(snapshotFile, buffer, bytes_written);

    write(snapshotFile, S_ISDIR(fileInfo.st_mode) ? "d" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IRUSR) ? "r" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IWUSR) ? "w" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IXUSR) ? "x" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IRGRP) ? "r" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IWGRP) ? "w" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IXGRP) ? "x" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IROTH) ? "r" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IWOTH) ? "w" : "-", 1);
    write(snapshotFile, (fileInfo.st_mode & S_IXOTH) ? "x" : "-", 1);
    write(snapshotFile, "\n\n", 2);

    if (!S_ISDIR(fileInfo.st_mode)) {
        return;
    }

    DIR *dir;
    struct dirent *entry;
    char fullPath[1024];

    if (!(dir = opendir(cale))) {
        printf("Eroare la deschiderea directorului %s \n", cale);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);

        createSnapshotRecursive(fullPath, snapshotFile, izolare);
    }

    closedir(dir);
}

void createSnapshot(const char *cale, const char *outputDir, int index, char* izolare) {
    struct stat dirInfo;
    if (lstat(cale, &dirInfo) == -1) {
        perror("Eroare info director \n");
        return;
    }

    char snapshotPath[2048];
    snprintf(snapshotPath, sizeof(snapshotPath), "%s/%ld_snapshot.txt", outputDir, dirInfo.st_ino);

    int snapshotFile = open(snapshotPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (snapshotFile == -1) {
        printf("Eroare deschidere fisier snapshot %s\n", snapshotPath);
        return;
    }

    int existingSnapshotFile = open(snapshotPath, O_RDONLY);
    if (existingSnapshotFile != -1) {
        int comparare = comparare_snapshot(snapshotPath, existingSnapshotFile, snapshotPath, snapshotFile);
        close(existingSnapshotFile);
        if (comparare != 0) {
            lseek(snapshotFile, 0, SEEK_SET);
        }
    }

    createSnapshotRecursive(cale, snapshotFile, izolare);
    close(snapshotFile);

    //printf("Snapshot for Directory %d created successfully.\n", index);
}

int main(int argc, char *argv[]) {
    pid_t aux;
    int status;

    char *outputDir = NULL;
    char *izolare = NULL;
    // Verificăm dacă opțiunea -o iesire este specificată corect
    if (argc < 4 || strcmp(argv[1], "-o") != 0 || strcmp(argv[2], "iesire") != 0) {
        printf("Utilizare: %s -o iesire dir1 dir2 .. dirn\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    outputDir = malloc(strlen(argv[2]) + 1);
    if (outputDir == NULL) {
      perror("Eroare la alocarea memoriei pentru outputDir\n");
      exit(EXIT_FAILURE);
    }
    strcpy(outputDir, argv[2]);
    
    izolare = malloc(strlen(argv[4]) + 1);
    if (izolare == NULL) {
      perror("Eroare la alocarea memoriei pentru izolare\n");
      exit(EXIT_FAILURE);
    }
    strcpy(izolare, argv[4]);
    
    pid_t pids[MAX_DIR];
    int pid_contor = 0;

    // Parcurgem argumentele de la 3 în sus pentru a crea snapshot-uri pentru fiecare director
    for (int i = 5; i < argc; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Eroare la fork\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
	  contor_fisiere_corupte = 0;
	  createSnapshot(argv[i], outputDir, i-1, izolare);
	  exit(contor_fisiere_corupte);
        } else {
            pids[pid_contor++] = pid;
        }
    }

    for (int i = 0; i < pid_contor; i++) {
        aux = wait(&status);
        if (aux == -1) {
            perror("Eroare asteptare\n");
            exit(EXIT_FAILURE);
        }
        printf("Child Process %d terminated with PID %d and exit code %d.\n", i+1 , aux, WEXITSTATUS(status));
    }

    free(outputDir);
    free(izolare);

    

    printf("All good all done.\n");
    return 0;
}