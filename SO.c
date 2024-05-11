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

void createSnapshotRecursive(const char *cale, int snapshotFile) {
    struct stat fileInfo;
    if (lstat(cale, &fileInfo) == -1) {
        perror("Eroare info fisier \n");
        return;
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

        createSnapshotRecursive(fullPath, snapshotFile);
    }

    closedir(dir);
}

void createSnapshot(const char *cale, const char *outputDir, int index) {
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

    createSnapshotRecursive(cale, snapshotFile);
    close(snapshotFile);

    //printf("Snapshot for Directory %d created successfully.\n", index);
}

int main(int argc, char *argv[]) {
    pid_t aux;
    int status;

    const char *outputDir = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
            outputDir = argv[i + 1];
            break;
        }
    }

    if (outputDir == NULL) {
        printf("Eroare dir iesire\n");
        exit(EXIT_FAILURE);
    }

    pid_t pids[MAX_DIR];
    int pid_contor = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            i = i + 2;
        }

        if (i >= argc) {
            break;
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("Eroare la fork\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            createSnapshot(argv[i], outputDir, i - 1);
            exit(EXIT_SUCCESS);
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

    printf("All good all done.\n");
    return 0;
}