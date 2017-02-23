#include "DirectoryCopier.h"
#include <dirent.h>
#include <cassert>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <algorithm>

static mode_t getMode(const std::string& path) {
    struct stat buf;
    stat(path.c_str(), &buf);
    return buf.st_mode;
}

/* https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950 */
static int mkdir_p(const char *path, mode_t mode)
{
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, mode) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (mkdir(_path, mode) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}

void DirectoryCopier::traverseDir(const std::string &path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string fileName = dir->d_name;
            std::string fullPath = (path + "/" + fileName);
            std::cout << fullPath << std::endl;
            if (dir->d_type == DT_DIR) {
                if (fileName != "." && fileName != "..") {
                    File file;
                    file.fileName = fileName;
                    file.dirPath = fullPath;
                    file.filePath = fullPath;
                    file.mode = getMode(fullPath);

                    dirList_.push_back(file);

                    traverseDir(fullPath);
                }
            } else if (dir->d_type == DT_REG) {
                File file;
                file.fileName = fileName;
                file.dirPath = path;
                file.filePath = fullPath;
                file.mode = getMode(fullPath);
                fileList_.push_back(file);
            } else {
                std::cerr << "Can't handle " << fullPath << std::endl;
            }
        }

        closedir(d);
    } else {
        std::cerr << "[DirectoryCopier]: Couldn't open directory " << path
                  << "(" << strerror(errno) << ")" << std::endl;
    }
}

DirectoryCopier::DirectoryCopier(const std::string &directory) : dir_(directory) {
    traverseDir(directory);
}


void DirectoryCopier::createShallowCopy(const std::string &target) {
    for (File& d : dirList_) {
        std::string p = target + "/" + d.dirPath;
        mkdir_p(p.c_str(), d.mode);
    }
    for (File& f : fileList_) {
        std::string p = target + "/" + f.filePath;
        link(f.filePath.c_str(), p.c_str());
    }
}

void DirectoryCopier::createDeepCopy(const std::string &target, const File& file) {
    std::cout << "[DirectoryCopier] deep copy of " + file.filePath << std::endl;

    assert(std::find(fileList_.begin(), fileList_.end(), file) != fileList_.end());

    std::string filePath = file.filePath;
    std::string path = target + "/" + filePath;
    unlink(path.c_str());

    int in_fd = open(filePath.c_str(), O_RDONLY);
    assert(in_fd >= 0);
    int out_fd = open(path.c_str(), O_CREAT|O_WRONLY, file.mode);
    assert(out_fd >= 0);
    char buf[8192];

    while (1) {
        ssize_t bytesRead = read(in_fd, &buf[0], sizeof(buf));
        if (bytesRead == 0) break;
        if (bytesRead < 0) {
            std::cerr << "Failed to copy file " << filePath << ": " <<
                         strerror(errno) << std::endl;
            break;
        }
        auto bytesWritten = write(out_fd, &buf[0], bytesRead);
        assert(bytesWritten == bytesRead);
    }

    close(out_fd);
    close(in_fd);
}
