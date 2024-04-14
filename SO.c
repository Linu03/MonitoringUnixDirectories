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

void createSnapshot(const char *cale, const char *outputDir) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileInfo;
    char fullPath[1024];
    char snapshotPath[2048];
    char buffer[2048];

    if (!(dir = opendir(cale))) {
 printf("Eroare la deschiderea directorului \%s.\n", cale);
        return;
    }

    //obt nume dir
    char *lastSlash = strrchr(cale, '/');
    const char *dirName;  // Modificat aici

    if (lastSlash) {
        dirName = lastSlash + 1;
    } else {
        dirName = cale;
    }

    // creare nume
    snprintf(snapshotPath, sizeof(snapshotPath), "%s/%s_snapshot.txt", outputDir, dirName);

    int snapshotFile = open(snapshotPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (snapshotFile == -1) {
        printf("Eroare deschidere fisier de snapshot \"%s\".\n", snapshotPath);
        closedir(dir);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);

        if (lstat(fullPath, &fileInfo) == -1) {
            perror("Eroare info fisier.\n");
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
    }

    close(snapshotFile);
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc <=3 || argc > MAX_DIR + 3) {
        printf("Numar invalid de argumente.\n");
        exit(-1);
    }

    const char *outputDir = NULL;
    int dirsEndIndex = argc - 2;

    if (strcmp(argv[dirsEndIndex], "-o") != 0) {
        printf("Nu exista -o .\n");
        exit(-1);
    }

    outputDir = argv[dirsEndIndex + 1];

    for (int i = 1; i < dirsEndIndex; i++) {
        createSnapshot(argv[i], outputDir);
    }


    printf("All good all done. =)\n");
    return 0;
}
