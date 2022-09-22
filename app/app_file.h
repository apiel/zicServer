#ifndef APP_FILE_H_
#define APP_FILE_H_

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h> // mkdir
#include <sys/types.h> // mkdir

struct dirent* myReaddir(DIR* x)
{
    struct dirent* directory;
    directory = readdir(x);
    if (directory != NULL && (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)) {
        return myReaddir(x);
    }
    return directory;
}

int8_t myStrcmp(const char *__s1, const char *__s2) {
    int res = strcmp(__s1, __s2);
    if (res == 0) {
        return 0;
    }
    return res > 0 ? 1 : -1;
}

void nextFile(char* filename, const char* folder, const char* current, int8_t direction)
{
    DIR* x = opendir(folder);
    if (x != NULL) {
        struct dirent* directory;
        if (direction == 0) { // TODO replace this by find first.
            if (((directory = myReaddir(x)) != NULL)) {
                strncpy(filename, directory->d_name, 256);
                return;
            }
        } else {
            char cur[256];
            strncpy(cur, current, 256);
            while ((directory = myReaddir(x)) != NULL) {
                if (myStrcmp(directory->d_name, cur) == direction
                    && (strcmp(filename, cur) == 0 || myStrcmp(directory->d_name, filename) == direction * -1)) {
                    strncpy(filename, directory->d_name, 256);
                }
            }
            if (!filename) {
                strncpy(filename, cur, 256);
            }
        }
        closedir(x);
    } else {
        strncpy(filename, "Empty folder", 256);
    }
}

void firstFile(char* filename, const char* folder)
{
    bool initialized = false;
    DIR* x = opendir(folder);
    if (x != NULL) {
        struct dirent* directory;
        while ((directory = myReaddir(x)) != NULL) {
            printf("%s <> %s = %d\n", directory->d_name, filename, myStrcmp(directory->d_name, filename));
            if (!initialized || myStrcmp(directory->d_name, filename) == -1) {
                strncpy(filename, directory->d_name, 256);
                initialized = true;
            }
        }
        closedir(x);
    }
    if (!initialized) {
        strncpy(filename, "Empty folder", 256);
    }
}

enum {
    FILE_NONE,
    FILE_SUCCESS,
};

#define MAX_FILENAME 100

template <typename... Args>
uint8_t loadFileContent(char* content, uint16_t len, const char* fmt, Args... args)
{
    char filename[MAX_FILENAME];
    snprintf(filename, MAX_FILENAME, fmt, args...);

    Zic_File file(filename, "r");
    if (file.isOpen()) {
        file.read(content, len);
        file.close();
        return FILE_SUCCESS;
    }

    return FILE_NONE;
}

uint8_t loadFileContent(char* content, uint16_t len, const char* fmt)
{
    return loadFileContent(content, len, fmt, NULL);
}

template <typename... Args>
uint8_t saveFileContent(const char* mode, char* content, uint16_t len, const char* fmt, Args... args)
{
    char filename[MAX_FILENAME];

    snprintf(filename, MAX_FILENAME, fmt, args...);
    strrchr(filename, '/')[0] = '\0';
    mkdir(filename, 0777);

    snprintf(filename, MAX_FILENAME, fmt, args...);
    Zic_File file(filename, mode);
    if (file.isOpen()) {
        file.write(content, len);
        file.close();
        return FILE_SUCCESS;
    }
    return FILE_NONE;
}

template <typename... Args>
uint8_t saveFileContent(char* content, uint16_t len, const char* fmt, Args... args)
{
    return saveFileContent("w", content, len, fmt, args...);
}

uint8_t saveFileContent(char* content, uint16_t len, const char* fmt)
{
    return saveFileContent(content, len, fmt, NULL);
}

uint8_t saveFileContent(const char* mode, char* content, uint16_t len, const char* fmt)
{
    return saveFileContent(mode, content, len, fmt, NULL);
}

#endif