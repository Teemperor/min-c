#ifndef DIRECTORYCOPIER_H
#define DIRECTORYCOPIER_H

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

class DirectoryCopier {
public:
    struct File {
       std::string dirPath;
       // full relative file path. Same as dirPath for directories
       std::string filePath;
       // file name
       std::string fileName;
       mode_t mode;
       bool operator==(const File& other) const {
           return filePath == other.filePath;
       }
    };
private:
    std::string dir_;


    std::vector<File> dirList_;
    std::vector<File> fileList_;

    void traverseDir(const std::string& path);

public:
    DirectoryCopier(const std::string& dir);

    void createShallowCopy(const std::string& target);

    void createDeepCopy(const std::string& target, const File& file);

    const std::vector<File>& getFileList() const {
        return fileList_;
    }
};

#endif // DIRECTORYCOPIER_H
