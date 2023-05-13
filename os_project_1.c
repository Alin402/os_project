#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

enum types_of_files
{
    REG_FILE,
    SYM_FILE,
    DIRECTORY
};

off_t return_file_dimension(char *file_name, struct stat buf) 
{
    return buf.st_size;
}

nlink_t return_number_of_hard_links(char *file_name, struct stat buf) 
{
    return buf.st_nlink;
}

const char * return_time_of_last_modification(char *file_name, struct stat buf) 
{
    char* time = (char *) ctime(&buf.st_mtime);
    return time;
}

void create_sym_link_to_file(char *file_name, char *link_name) 
{
    int sym_link = symlink(file_name, link_name);
    if(sym_link == 0) 
    {
        printf("Sym link created successfuly\n");
    }
}

void delete_symbolic_link(char *link_name) 
{
    unlink(link_name);
    printf("successfuly deleted link\n");
}

void print_file_access_rights(struct stat buf) 
{
    printf("access rights: \n");
    printf("User:\n");
    printf((buf.st_mode & S_IRUSR) ? "Read - yes\n" : "Read - no \n");
    printf((buf.st_mode & S_IWUSR) ? "Write - yes\n" : "Write - no\n");
    printf((buf.st_mode & S_IXUSR) ? "Exec - yes\n" : "Exec - no\n");
    printf("Group:\n");
    printf((buf.st_mode & S_IRGRP) ? "Read - yes\n" : "Read - no\n");
    printf((buf.st_mode & S_IWGRP) ? "Write - yes\n" : "Write - no\n");
    printf((buf.st_mode & S_IXGRP) ? "Exec - yes\n" : "Exec - no\n");
    printf("Other:\n");
    printf((buf.st_mode & S_IROTH) ? "Read - yes\n" : "Read - no\n");
    printf((buf.st_mode & S_IWOTH) ? "Write - yes\n" : "Write - no\n");
    printf((buf.st_mode & S_IXOTH) ? "Exec - yes\n" : "Exec - no\n");
    printf("\n\n");
}

void print_regular_file_menu()
{
    const char *msg = "-n (file name)\n"
                      "-d (dim/size)\n"
                      "-h (number of hard links)\n"
                      "-m (time of last modification)\n"
                      "-a (access rights)\n"
                      "-l (create a sym link give: link name)";
    printf("%s\n\n", msg);
}

void print_symbolic_link_menu()
{
    const char *msg = "-n (link name)\n"
                      "-l (delete link)\n"
                      "-d (size of the link)\n"
                      "-t (size of the target)\n"
                      "-a (access rights)";
    printf("%s\n\n", msg);
}

void print_directory_menu()
{
    const char *msg = "-n (directory name)\n"
                      "-d (size of the directory)\n"
                      "-a (access rights)\n"
                      "-c (total number of files with the .c extension)\n";
    printf("%s\n\n", msg);
}

enum types_of_files print_menu_depending_on_file_type(char *file_name)
{
    struct stat buf;
    lstat(file_name, &buf);
    if (S_ISLNK(buf.st_mode))
    {
        print_symbolic_link_menu();
        return SYM_FILE;
    }
    else if (S_ISDIR(buf.st_mode)) {
        print_directory_menu();
        return DIRECTORY;
    }
    else
    {
        print_regular_file_menu();
        return REG_FILE;
    }
}

int validate_operations_string(char operations_string[], enum types_of_files type_of_file)
{
    char *characters_allowed_regular = "-ndhmal";
    char *characters_allowed_symbolic = "-ndatl";
    int n = strlen(operations_string);
    // ignore white space characters until beginning of string
    int i = 0;
    while (isspace(operations_string[i]))
    {
        i++;
    }
    if (operations_string[i] != '-')
    {
        return 0;
    }
    // check if characters are not allowed in string
    for (; i < n && !isspace(operations_string[i]); i++)
    {
        if (type_of_file == SYM_FILE && !strchr(characters_allowed_symbolic, operations_string[i]))
        {
            return 0;
        }
        if (type_of_file == REG_FILE && !strchr(characters_allowed_regular, operations_string[i]))
        {
            return 0;
        }
    }
    return 1;
}

int count_c_files(char *dir_path) 
{
    int file_count = 0;
    DIR *dir;
    struct dirent *entry;

    struct stat buf;

    if (dir = opendir(dir_path))
    {
        while (entry = readdir(dir)) 
        {
            if (entry -> d_type == DT_REG)
            {
                char *extension = strstr(entry -> d_name, ".c");
                if (extension) {
                    file_count++;
                }
            }
        }
    }
    else 
    {
        perror("Directory could not be opened\n\n");
        exit(1);
    }
    closedir(dir);
    return file_count;
}

void execute_operations(char *operations_string, char *file_name, enum types_of_files file_type, struct stat buf)
{
    printf("\n");
    int i = 0;
    int n = strlen(operations_string);
    while (operations_string[i] != '-')
    {
        i++;
    }
    i++;
    for (; i < n && !isspace(i); i++)
    {
        char operation = operations_string[i];
        if(file_type == REG_FILE) 
        {
            switch(operation) 
            {
                case 'n':
                    printf("file name: %s\n", file_name);
                    break;
                case 'd':
                    printf("dim/size: %ld bytes\n", return_file_dimension(file_name, buf));
                    break;
                case 'h':
                    printf("number of hard links: %ld\n", return_number_of_hard_links(file_name, buf));
                    break;
                case 'm':
                    printf("time of last modification: %s\n", return_time_of_last_modification(file_name, buf));
                    break;
                case 'a':
                    print_file_access_rights(buf);
                    break;
                case 'l':
                    printf("Give name of new symbolic link:\n");
                    char *link_name;
                    scanf("%s", link_name);
                    create_sym_link_to_file(file_name, link_name);
                    break;
            }
        } 
        else if(file_type == SYM_FILE) 
        {
            switch(operation) 
            {
                case 'n':
                    printf("link name: %s\n", file_name);
                    break;
                case 'd':
                    printf("size of the link: %ld bytes\n", return_file_dimension(file_name, buf));
                    break;
                case 't':
                    struct stat fbuf;
                    stat(file_name, &fbuf);
                    printf("size of the target: %ld bytes\n", return_file_dimension(file_name, fbuf));
                    break;
                case 'a':
                    print_file_access_rights(buf);
                    break;
                case 'l':
                    delete_symbolic_link(file_name);
                    break;
            }
        } 
        else if (file_type == DIRECTORY) 
        {
            switch(operation) 
            {
                case 'n':
                    printf("directory name: %s\n", file_name);
                    break;
                case 'd':
                    printf("size of the directory: %ld bytes\n", return_file_dimension(file_name, buf));
                    break;
                case 'a':
                    print_file_access_rights(buf);
                    break;
                case 'c':
                    printf("number of c files: %d\n", count_c_files(file_name));
                    break;
            }
        }
    }
}

int check_if_c_file(char *file_name) 
{
    int file_length = strlen(file_name);
    printf("%d\n", file_name[file_length - 2] == '.' && file_name[file_length - 1] == 'c');
    return file_name[file_length - 2] == '.' && file_name[file_length - 1] == 'c';
}

void execute_c_file(char *file_name) 
{
    char* arguments[] = {"bash", "compile_c.sh", file_name, "f1.txt", NULL};      
    printf("Executing c file %s\n\n", file_name);
    if(execv("/usr/bin/bash", arguments) == -1) 
    {
        perror("execv");
        exit(EXIT_FAILURE);
    }
    exit(0);
}

void loop(int nr_of_files, char *file_names[])
{
    pid_t pid1;
    for (int i = 1; i < nr_of_files; i++)
    {
        if ((pid1 = fork()) < 0) 
        {
            printf("Something went wrong while creating child process 1\n");
            exit(1);
        }

        if (pid1 == 0)
        {
            printf("Intra aici");
            char *file_name = file_names[i];
            printf("<< %s >>\n\n", file_name);
            enum types_of_files file_type = print_menu_depending_on_file_type(file_name);
            printf("an example format is \"-nd\"\n\n");
            char operations_string[10];
            scanf("%s", operations_string);

            int is_valid_operations_string = validate_operations_string(operations_string, file_type);
            while (!is_valid_operations_string) 
            {
                printf("\nThe string << %s >> is not a valid input\nPlease try again\n\n", operations_string);
                printf("<< %s >>\n\n", file_name);
                enum types_of_files file_type = print_menu_depending_on_file_type(file_name);
                printf("an example format is \"-nd\"\n\n");
                char operations_string[10];
                scanf("%s", operations_string);
                is_valid_operations_string = validate_operations_string(operations_string, file_type);
            }

            struct stat buf;
            lstat(file_name, &buf);
            execute_operations(operations_string, file_name, file_type, buf);
            exit(0);
        }
        else
        {
            wait(NULL);
        }
    }
}

int main(int argc, char *argv[])
{
    loop(argc, argv);
    return 0;
}