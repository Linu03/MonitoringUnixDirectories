# MonitoringUnixDirectories

This project is a C + Bash application that allows recursive snapshots of the filesystem, analyzing files in a directory to detect potentially corrupt or suspicious files, then moving them to an isolation directory.

------

## Main Features

Create snapshot for each specified directory (with information such as: size, inode, access rights, modification date, etc.).

Scan for suspicious files: any file with 000 rights, non-ASCII characters, keywords such as malware, dangerous, risk, attack, or disproportionately large files vs. lines, are considered suspicious.

Automatic isolation: suspicious files are moved to a dedicated quarantine directory (e.g. ./isolated_files/).

Parallelization: each directory is processed in parallel using fork for increased efficiency.

Snapshot comparison: changes between snapshots are detected, allowing changes in file structure to be highlighted.

---

## Project structure
main.c - the main C application that:

- checks arguments;

- processes directories;

- generates snapshots and isolates corrupt files.

- script.sh - auxiliary Bash script that analyzes the contents of files to identify dangerous ones.

- snapshot.txt - output file generated for each directory analyzed.

------

## Technologies used

- C (programming language) - for file processing, process management and file system work.

- Bash - for analyzing file contents in a flexible and scriptable way.

- Concurrent programming - use fork() function for parallelizing directory parsing.

- Linux file systems - for extracting metadata (inode, access, timestamps, etc.).

- POSIX standard - for functions such as opendir, readdir, stat, fork, execlp, waitpid.

------

## How to run the app

```gcc main.c -o snapshot_tool```

```chmod +x script.sh```

```./snapshot_tool -o output -s izolation_directory dir1 dir2 ... dirN```

- -o iesire – the directory where snapshots are saved

- -s director_izolare – the directory where the suspicious files will be moved

- dir1 dir2 ... dirN – directories to be processed


### Exemple

``` ./snapshot_tool -o snapshots -s izolari /home/user/docs /home/user/proiect```

``` Child Process 1 terminated with PID 12345 and exit code 2.```
```Child Process 2 terminated with PID 12346 and exit code 0.```
```All good all done.```

------

## Suspicious file criteria

- Has no access rights (000)

- Contains non-ASCII characters

- Contains words such as malware, dangerous, attack, risk

- Has a disproportionate number of characters/words compared to lines (e.g. <3 lines, >1000 words, >2000 characters)Suspicious file criteria:
