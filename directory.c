#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

/* Filter for scandir function, discards a hidden files */
int name_filter(const struct dirent * dir)
{
    if (dir->d_name[0] == '.') {
        return 0;
    }
    return 1;
}

/* Create new string wich contain concatenation of dir_path and fname */
static char *make_path(const char *dir_path, const char *fname)
{
    char *path = (char*) malloc(sizeof(char) *
            (strlen(dir_path) + strlen(fname) + 1));
    if (!path) {
        return path;
    }
    strcpy(path, dir_path);
    if(path[strlen(path) - 1] != '/') {
        strcat(path, "/");
    }
    strcat(path, fname);
    return path;
}

/* Fills p_str of file permissions */
static void make_permissions(struct stat *fstat, char *p_str)
{
    strcpy(p_str, "----------");
    int i = 0;
    mode_t mode = fstat->st_mode;

    if(S_ISDIR(mode))p_str[i] = 'd'; i++;

    if(S_IRUSR&mode)p_str[i] = 'r'; i++;
    if(S_IWUSR&mode)p_str[i] = 'w'; i++;
    if(S_IXUSR&mode)p_str[i] = 'x'; i++;

    if(S_IRGRP&mode)p_str[i] = 'r'; i++;
    if(S_IWGRP&mode)p_str[i] = 'w'; i++;
    if(S_IXGRP&mode)p_str[i] = 'x'; i++;

    if(S_IROTH&mode)p_str[i] = 'r'; i++;
    if(S_IWOTH&mode)p_str[i] = 'w'; i++;
    if(S_IXOTH&mode)p_str[i]   = 'x';
}

/* Create groups string from group structures */
static char *make_group(struct stat *fstat)
{
    struct passwd *pw = getpwuid(fstat->st_uid);
    if (pw == NULL) return NULL;

    struct group *gr = getgrgid(fstat->st_gid);
    if(gr == NULL) return NULL;

    int size = strlen(pw->pw_name) + strlen(gr->gr_name) + 2;
    char *result = (char*) malloc(sizeof(char) * size);
    if (!result) return NULL;

    sprintf(result, "%s %s", pw->pw_name, gr->gr_name);

    return result;
}

/* Fill str, (month day hours:minutes) */
static char *make_time(struct timespec *cal_time, char *str, const size_t size)
{
  struct tm *time;

  time = localtime(&cal_time->tv_sec);
  strftime(str, size, "%h %e %H:%M", time);

  return str;
}

/* Reading information about the catalog and formation of the output */
int print_dir(const char *path, int list_opt)
{
    /* Read dir info to dirent structure */
    int n;
    struct dirent **file_list;
    if ((n = scandir(path, &file_list, &name_filter, alphasort)) < 0) {
        perror("Scandir ERROR\n");
        return (EXIT_FAILURE);
    }

    /* Formation of the detail or simple output */
    if (list_opt) {
        struct stat fstat;
        for (int i = 0; i < n; i++) {
            char *fpath = make_path(path, file_list[i]->d_name);
            /* Read detailed info about current ellement of the folder */
            if (lstat(fpath, &fstat)) {
                perror("lstat ERROR\n");
                return (EXIT_FAILURE);
            }

            /* Formating strings for detailed output */
            char p_str[11];
            make_permissions(&fstat, p_str);
            char *group = make_group(&fstat);
            if (!group) {
                perror("format output ERROR\n");
                return (EXIT_FAILURE);
            }
            char time[30];
            make_time(&fstat.st_mtim, time, sizeof(time));

            printf("%s. %lu %s %8lu %s %s\n", p_str, fstat.st_nlink,
                    group, fstat.st_size, time,
                    file_list[i]->d_name);

            free(file_list[i]);
            free(fpath);
            free(group);
        }
    } else {
        /* Simple output */
        for (int i = 0; i < n; i++) {
            printf("%s  ", file_list[i]->d_name);
            free(file_list[i]);
        }
        printf("\n");
    }

    return (EXIT_SUCCESS);
}
