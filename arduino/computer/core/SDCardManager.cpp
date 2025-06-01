#include "SDCardManager.h"
#include "PINS.h"

SDCardManager::SDCardManager() {}

bool SDCardManager::begin() {
    if (!SD.begin(PIN_SD_SS)) {
        Serial.println("SD Card Mount Failed");
        return false;
    }

    if (SD.cardType() == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }

    return true;
}

void SDCardManager::listDir(const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);
    File root = SD.open(dirname);
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) listDir(file.path(), levels - 1);
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void SDCardManager::createDir(const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    Serial.println(SD.mkdir(path) ? "Dir created" : "mkdir failed");
}

void SDCardManager::removeDir(const char *path) {
    Serial.printf("Removing Dir: %s\n", path);
    Serial.println(SD.rmdir(path) ? "Dir removed" : "rmdir failed");
}

void SDCardManager::readFile(const char *path) {
    Serial.printf("Reading file: %s\n", path);
    File file = SD.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    Serial.print("Read from file: ");
    while (file.available()) Serial.write(file.read());
    file.close();
}

void SDCardManager::writeFile(const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    Serial.println(file.print(message) ? "File written" : "Write failed");
    file.close();
}

void SDCardManager::appendFile(const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    Serial.println(file.print(message) ? "Message appended" : "Append failed");
    file.close();
}

void SDCardManager::renameFile(const char *path1, const char *path2) {
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    Serial.println(SD.rename(path1, path2) ? "File renamed" : "Rename failed");
}

void SDCardManager::deleteFile(const char *path) {
    Serial.printf("Deleting file: %s\n", path);
    Serial.println(SD.remove(path) ? "File deleted" : "Delete failed");
}

void SDCardManager::testFileIO(const char *path) {
    File file = SD.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;

    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len > 512 ? 512 : len;
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %lu ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
        return;
    }

    file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    start = millis();
    for (size_t i = 0; i < 2048; i++) file.write(buf, 512);
    end = millis() - start;
    Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
    file.close();
}

void SDCardManager::printCardInfo() {
    uint8_t cardType = SD.cardType();
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) Serial.println("MMC");
    else if (cardType == CARD_SD) Serial.println("SDSC");
    else if (cardType == CARD_SDHC) Serial.println("SDHC");
    else Serial.println("UNKNOWN");

    Serial.printf("SD Card Size: %lluMB\n", SD.cardSize() / (1024 * 1024));
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}
