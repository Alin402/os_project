#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

off_t return_file_dimension(char *file_name, struct stat buf) {
    return buf.st_size;
}

enum types_of_files
{
    REG_FILE,
    SYM_FILE,
    DIR
};

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

enum types_of_files print_menu_depending_on_file_type(char *file_name)
{
    struct stat buf;
    lstat(file_name, &buf);
    if (S_ISLNK(buf.st_mode))
    {
        print_symbolic_link_menu();
        return SYM_FILE;
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
        if(file_type == REG_FILE) {
            switch(operation) {
                case 'n':
                    printf("file name: %s\n", file_name);
                    break;
                case 'd':
                    printf("dim/size: %ld bytes\n", return_file_dimension(file_name, buf));
                    break;
                case 'h':
                    printf("number of hard links\n");
                    break;
                case 'm':
                    printf("time of last modification\n");
                    break;
                case 'a':
                    printf("access rights\n");
                    break;
                case 'l':
                    printf("create a sym link give: link name\n");
                    break;
            }
        } else if(file_type == SYM_FILE) {
            switch(operation) {
                case 'n':
                    printf("link name: %s\n", file_name);
                    break;
                case 'd':
                    printf("size of the link\n");
                    break;
                case 't':
                    printf("size of the target\n");
                    break;
                case 'a':
                    printf("access rights\n");
                    break;
                case 'l':
                    printf("delete link\n");
                    break;
            }
        }
    }
}

void loop(int nr_of_files, char *file_names[])
{
    for (int i = 1; i < nr_of_files; i++)
    {
        char *file_name = file_names[i];
        enum types_of_files file_type = print_menu_depending_on_file_type(file_name);
        printf("choose operations for file %s\n", file_name);
        printf("an example format is \"-nd\"\n\n");
        char operations_string[10];
        scanf("%s", operations_string);
        if (validate_operations_string(operations_string, file_type))
        {
            struct stat buf;
            lstat(file_name, &buf);
            execute_operations(operations_string, file_name, file_type, buf);
        }
        else
        {
            printf("Not Valid\n");
        }
    }
}

int main(int argc, char *argv[])
{
    loop(argc, argv);
    return 0;
}