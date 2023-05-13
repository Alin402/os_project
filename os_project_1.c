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
    if (unlink(link_name) < 0 && errno != ENOENT) {
        perror("unlink");
        return;
    }

    if (symlink(file_name, link_name) < 0) {
        perror("fstat");
        return;
    }

    printf("You created the symbolic link '%s' for file '%s'\n", link_name, file_name);
}

void delete_symbolic_link(char *link_name) 
{
    char *arguments[] = {"rm", link_name, NULL};

    printf("deleted link %s\n", link_name);
    fflush(stdout);

    if (execv("/usr/bin/rm", arguments) == -1)
    {
        perror("execv");
        exit(EXIT_FAILURE);
    }
}

void get_file_name(char file_path[])
{
    int l = strlen(file_path);
    for (int i = l - 1; i >= 0; i--)
    {
        if (file_path[i] == '.')
        {
            file_path[i] = '\0';
            break;
        }
    }
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

enum types_of_files return_file_type(char *file_name)
{
    struct stat buf;
    lstat(file_name, &buf);

    if (S_ISLNK(buf.st_mode))
    {
        return SYM_FILE;
    }
    else if (S_ISDIR(buf.st_mode))
    {
        return DIRECTORY;
    }
    else if (S_ISREG(buf.st_mode))
    {
        return REG_FILE;
    }
}

void print_menu_depending_on_file_type(char *file_name)
{
    enum types_of_files file_type = return_file_type(file_name);
    if (file_type == SYM_FILE)
    {
        print_symbolic_link_menu();
    }
    else if (file_type == DIRECTORY) 
    {
        print_directory_menu();
    }
    else if (file_type == REG_FILE)
    {
        print_regular_file_menu();
    }
}

int validate_operations_string(char operations_string[], enum types_of_files type_of_file)
{
    char *characters_allowed_regular = "-ndhmal";
    char *characters_allowed_symbolic = "-nldta";
    char *characters_allowed_directory = "-ndac";
    int n = strlen(operations_string);
    int i = 0;
    while (isspace(operations_string[i]))
    {
        i++;
    }
    if (operations_string[i] != '-')
    {
        return 0;
    }
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
        if (type_of_file == DIRECTORY && !strchr(characters_allowed_directory, operations_string[i]))
        {
            return 0;
        }
    }
    return 1;
}

int check_if_file_exists(char *file_name)
{
    return !access(file_name, F_OK);
}

int count_files_of_c_type(char *dir_path) 
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
                    char f_name[256];
                    strcpy(f_name, file_name);
                    get_file_name(f_name);
                    printf("file name: %s\n", f_name);
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
                    char link_name[256];
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
                    printf("number of c files: %d\n", count_files_of_c_type(file_name));
                    break;
            }
        }
    }
}

int calculate_score(int nr_of_errors, int nr_of_warnings)
{   
    if (nr_of_errors == 0 && nr_of_warnings == 0)
    {
        return 10;
    }
    else if (nr_of_errors >= 1)
    {
        return 1;
    }
    else if (nr_of_errors == 0 && nr_of_warnings > 10)
    {
        return 2;
    }
    else if (nr_of_errors == 0 && nr_of_warnings <= 10)
    {
        return 2 + 8 * (10 - nr_of_warnings) / 10;
    }
}

void count_errors_and_warnings(char *file_name) {
    char *arguments[] = {"bash", "compileCProgram.sh", file_name, "errors_output.txt", NULL};

    printf("Compiled file %s\n\n", file_name);
    fflush(stdout);

    if (execv("/usr/bin/bash", arguments) == -1)
    {
        perror("execv");
        exit(EXIT_FAILURE);
    }
}

void count_number_of_lines(char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    int ch = 0;
    int lines = 1;

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }

    printf("Number of lines: %d\n\n", lines);
}

void create_file(char *directory_name)
{
    char file_name[256];
    strcpy(file_name, strcat(directory_name, "_file.txt"));
    char *arguments[] = {"touch", directory_name, NULL};

    printf("\nCreated file %s\n\n", file_name);
    fflush(stdout);

    if (execv("/usr/bin/touch", arguments) == -1)
    {
        perror("execv");
        exit(EXIT_FAILURE);
    }
}

void change_permissions(char *link_name)
{
    char *arguments[] = {"chmod", "-v", "760", link_name, NULL};
    printf("Changing permissions to 'rwxrw----'\n");
    if(execv("/usr/bin/chmod", arguments) == -1) 
    {
        perror("execv");
        exit(EXIT_FAILURE);
    }
}

void loop(int nr_of_files, char *file_names[])
{
    pid_t process1;
    pid_t process2;
    int fd[2];
    int st1;
    int st2;
    char buffer[4096];
    for (int i = 1; i < nr_of_files; i++)
    {

        if (pipe(fd) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        if ((process1 = fork()) < 0) 
        {
            printf("Something went wrong while creating child process 1\n");
            exit(EXIT_FAILURE);
        }

        if (process1 == 0)
        {
            char *file_name = file_names[i];
            if (!check_if_file_exists(file_name))
            {
                printf("File %s not found!\n\n", file_name);
                exit(1);
            }

            enum types_of_files file_type = return_file_type(file_name);
            printf("\n<< %s >>\n\n", file_name);
            print_menu_depending_on_file_type(file_name);
            printf("an example format is \"-nd\"\n\n");
            char operations_string[10];
            scanf("%s", operations_string);

            int is_valid_operations_string = validate_operations_string(operations_string, file_type);
            while (!is_valid_operations_string) 
            {
                printf("\nThe string << %s >> is not a valid input\nPlease try again\n\n", operations_string);
                printf("<< %s >>\n\n", file_name);
                print_menu_depending_on_file_type(file_name);
                printf("an example format is \"-nd\"\n\n");
                scanf("%s", operations_string);
                is_valid_operations_string = validate_operations_string(operations_string, file_type);
            }

            struct stat buf;
            lstat(file_name, &buf);
            execute_operations(operations_string, file_name, file_type, buf);
            exit(0);
        }
        else if (process1 > 0)
        {
            if ((process2 = fork()) < 0) 
            {
                printf("Something went wrong while creating child process 2\n");
                exit(1);
            }

            if (process2 == 0)
            {
                close(fd[0]);

                char *file_name = file_names[i];
                enum types_of_files file_type = return_file_type(file_name);
                if (file_type == REG_FILE) 
                {
                    char *extension = strstr(file_name, ".c");
                    if (extension)
                    {
                        if (dup2(fd[1], STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }

                        if(close(fd[1]) == -1) 
                        {
                            perror("close");
                            exit(EXIT_FAILURE);
                        }
                        count_errors_and_warnings(file_name);
                    }
                    else
                    {
                        if (dup2(fd[1], STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }

                        if(close(fd[1]) == -1) 
                        {
                            perror("close");
                            exit(EXIT_FAILURE);
                        }

                        count_number_of_lines(file_name);
                    }
                }
                else if (file_type == DIRECTORY)
                {
                    if (dup2(fd[1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }

                    if(close(fd[1]) == -1) 
                    {
                        perror("close");
                        exit(EXIT_FAILURE);
                    }
                    create_file(file_name);
                }
                else if (file_type == SYM_FILE)
                {
                    if (dup2(fd[1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }

                    if(close(fd[1]) == -1) 
                    {
                        perror("close");
                        exit(EXIT_FAILURE);
                    }
                    change_permissions(file_name);
                }
                exit(1);
            }
            else if (process2 > 0)
            {
                close(fd[1]);

                int n1, n2;
        
                waitpid(process1, &st1, 0);

                while((n2 = read(fd[0], buffer, sizeof(buffer))) > 0) 
                {
                    buffer[n2] = '\0';
                    write(STDOUT_FILENO, buffer, n2);
                }

                waitpid(process2, &st2, 0);
        
                printf("\n");
                if(WIFEXITED(st1))
                {
                    printf("Process with PID %d has ended with the exit code %d\n", process1,  WEXITSTATUS(st1));
                }
                else
                {
                    printf("Process with PID %d has not ended correctly\n", process1);
                }
                if(WIFEXITED(st2))
                {
                    printf("Process with PID %d has ended with the exit code %d\n", process2, WEXITSTATUS(st2));
                }
                else
                {
                    printf("Child with PID %d has not ended correctly\n", process2);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    loop(argc, argv);
    return 0;
}