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

void createSnapshotRecursive(const char *cale, int snapshotFile){
  DIR *dir;
  struct dirent *entry;
  struct stat fileInfo;
  char fullPath[1024];
  char buffer[2048];
  
  if (!(dir = opendir(cale))){
    printf("Eroare la deschiderea directorului %sb\n", cale);
    return;
  }
  
  while ((entry = readdir(dir)) != NULL){
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
    }
    
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);
    
    if (lstat(fullPath, &fileInfo) == -1){
      perror("Eroare info fisier \n");
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
      createSnapshotRecursive(fullPath, snapshotFile);
    }
  }
  
  closedir(dir);
}

void createSnapshot(const char *cale, const char *dir_iesire) {
  DIR *dir;
  struct dirent *entry;
  struct stat fileInfo;
  char fullPath[1024];
  char snapshotPath[2048];
  char buffer[2048];

  if (!(dir = opendir(cale))) {
    printf("Eroare la deschiderea directorului %s.\n", cale);
    return;
  }
  
  char *lastSlash = strrchr(cale, '/');
  const char *dirName;
  
  if (lastSlash) {
    dirName = lastSlash + 1;
  } else {
        dirName = cale;
  }
  
  snprintf(snapshotPath, sizeof(snapshotPath), "%s/%s_snapshot.txt", dir_iesire, dirName);
  
  int snapshotFile = open(snapshotPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  
  if (snapshotFile == -1) {
    printf("Eroare deschidere fisier snapshot %s\n", snapshotPath);
    closedir(dir);
    return;
    }
  
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cale, entry->d_name);
    
    if (lstat(fullPath, &fileInfo) == -1) {
      perror("Eroare info fisier\n");
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
      createSnapshotRecursive(fullPath, snapshotFile);
    }
  }
  
  close(snapshotFile);
  closedir(dir);
}

int main(int argc , char *argv[] ){

  pid_t aux;
  int status;
  
  const char *dir_iesire = NULL;
  
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0 && i < argc-1){
      dir_iesire = argv[i + 1];
      break;
    }
  }
    
  if (dir_iesire == NULL){
    printf("Eroare la -o iesire \n");
    exit(-1);
  }
  
  
  pid_t pids[MAX_DIR];
  int contor_pid = 0;
 
    
  for (int i = 1; i < argc ; i++){ 
    if (strcmp(argv[i], "-o") == 0) {
      i = i + 2; 
      
    }
    
    pid_t pid = fork();

    if (pid == -1){
      perror("Eroare la fork\n");
      exit(-1);
    }
    
    if (pid == 0){  
      createSnapshot(argv[i], dir_iesire);
      exit(0);  
    }
    else{
    
      pids[contor_pid++] = pid;
    }
  }

  
  
  for (int i = 0; i < contor_pid; i++){
      aux = wait(&status);
      if (aux == -1 ){
	perror("wait");
	exit(EXIT_FAILURE);
      }
    }

  
    printf("All good all done. =)\n");
    return 0;
}
