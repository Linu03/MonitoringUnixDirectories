#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void snapFunction(const char *cale, int fileDescriptor) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileInfo;
    char fullPath[1024];

    if (!(dir = opendir(cale))) {
        printf("Eroare la deschiderea directorului.\n ");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);

        if (lstat(fullPath, &fileInfo) == -1) {
            perror("Eroare la accesarea informatiei fisierului.\n");
            continue;
        }

        if (S_ISDIR(fileInfo.st_mode)) {
            snapFunction(fullPath, fileDescriptor);
        } else {
            char buf[1024];
            time_t mtime = fileInfo.st_mtime;
            int n = snprintf(buf, sizeof(buf), "%s\t%ld\t%s", fullPath, fileInfo.st_size, ctime(&mtime));
            write(fileDescriptor, buf, n);
        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Eroare la numarul de argumente in linia de comanda.\n");
        return -1;
    }

    const char *cale = argv[1];

    int snapshotFile = open("snapshot.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (snapshotFile == -1) {
        printf("Eroare la deschiderea directorului.\n");
        return -1;
    }

    snapFunction(cale, snapshotFile);

    close(snapshotFile);
    return 0;
}
