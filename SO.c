#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_DIR 10

void createSnapshotRecursive(const char *cale, const char *outputDir, int snapshotFile) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileInfo;
    char fullPath[1024];
    char buffer[2048];

    if (!(dir = opendir(cale))) {
        printf("Eroare la deschiderea directorului %s.\n", cale);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);

        if (lstat(fullPath, &fileInfo) == -1) {
            perror("Eroare la accesarea informatiilor fisierului.\n");
            continue;
        }

        int bytes_written = snprintf(buffer, sizeof(buffer), "Cale: %s\n", fullPath);
        write(snapshotFile, buffer, bytes_written);

        bytes_written = snprintf(buffer, sizeof(buffer), "Dimensiune: %ld\n", fileInfo.st_size);
        write(snapshotFile, buffer, bytes_written);

        bytes_written = snprintf(buffer, sizeof(buffer), "Ultima modificare: %s", ctime(&fileInfo.st_mtime));
        write(snapshotFile, buffer, bytes_written);

        bytes_written = snprintf(buffer, sizeof(buffer), "Inode: %ld\n\n", fileInfo.st_ino);
        write(snapshotFile, buffer, bytes_written);

        if (S_ISDIR(fileInfo.st_mode)) {
            createSnapshotRecursive(fullPath, outputDir, snapshotFile);
        }
    }

    closedir(dir);
}


void createSnapshot(const char *cale, const char *outputDir) {
    char snapshotPath[2048];
    snprintf(snapshotPath, sizeof(snapshotPath), "%s/%s_snapshot.txt", outputDir, cale);

    int snapshotFile = open(snapshotPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (snapshotFile == -1) {
        printf("Eroare la deschiderea fisierului de snapshot %s.\n", snapshotPath);
        return;
    }

    createSnapshotRecursive(cale, outputDir, snapshotFile);

    close(snapshotFile);
}


int main(int argc, char *argv[]) {
    if (argc <= 3 || argc > MAX_DIR + 4) {
        printf("Numar invalid de argumente.\n");
        exit(-1);
    }

    const char *outputDir = NULL;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            outputDir = argv[i + 1];
        }
    }

    if (!outputDir) {
        printf("Nu exista -o sau directorul de iesire.\n");
        exit(-1);
    }

    for (int i = 1; i < argc - 2; i++) {
        if (strcmp(argv[i], "-o") == 0) {
	  i += 2; 
        }
        createSnapshot(argv[i], outputDir);
    }

    printf("All good all done. =)\n");
    return 0;
}
