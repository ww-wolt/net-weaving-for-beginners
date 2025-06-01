#pragma once

#include "FS.h"
#include "SD.h"

class SDCardManager {
public:
    SDCardManager();

    bool begin();
    void listDir(const char *dirname, uint8_t levels = 1);
    void createDir(const char *path);
    void removeDir(const char *path);
    void readFile(const char *path);
    void writeFile(const char *path, const char *message);
    void appendFile(const char *path, const char *message);
    void renameFile(const char *oldPath, const char *newPath);
    void deleteFile(const char *path);
    void testFileIO(const char *path);
    void printCardInfo();

    fs::SDFS &getSD() {
        return SD;
    }
};