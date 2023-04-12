#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void print_regular_file_menu() {
    const char *msg = "-n (file name)\n"
                      "-d (dim/size)\n"
                      "-h (number of hard links)\n"
                      "-m (time of last modification)\n"
                      "-a (access rights)\n"
                      "-l (create a sym link give: link name)";
    printf("%s\n\n", msg);
}

void print_symbolic_link_menu() {
    const char *msg = "-n (link name)\n"
                      "-l (delete link)\n"
                      "-d (size of the link)\n"
                      "-t (size of the target)\n"
                      "-a (access rights)";
    printf("%s\n\n", msg);
}

void print_menu_depending_on_file_type(char *fileName) {
    struct stat buf;
    lstat(fileName, &buf);
    if (S_ISLNK(buf.st_mode)) {
        print_symbolic_link_menu();
    } else {
        print_regular_file_menu();
    }
}

int main(int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        char *fileName = argv[i];
        printf("%s\n", fileName);
        print_menu_depending_on_file_type(fileName);
    }
    return 0;
}