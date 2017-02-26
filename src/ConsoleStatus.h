#ifndef CONSOLESTATUS_H
#define CONSOLESTATUS_H

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <chrono>

class ConsoleStatus
{
    const size_t maxLength = 80;
    std::stringstream message_;

    std::chrono::milliseconds printInterval{50};
    bool readyToPrint_ = true;

    size_t iterations_ = 0;

    long long reducedBytes = 0;

    int printAndReturnSize(const std::string& str) {
        std::cout << str;
        return str.size();
    }
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();

    bool checkTimeToPrint();

    int remainingSpace() {
        if (message_.str().size() > maxLength)
            return 0;
        return static_cast<int>(maxLength - message_.str().size());
    }

    void resetMessage() {
        message_.str(std::string());
        message_.clear();
        message_ << "[I:" << iterations_ << "] ";
/*
        message_ << "[-";
        if (reducedBytes < 1000u) {
            message_ << reducedBytes << 'B';
        } else if (reducedBytes < 1000000u) {
            message_ << reducedBytes / 1000 << "KB";
        } else if (reducedBytes < 1000000000u) {
            message_ << reducedBytes / 1000 << "MB";
        }
        message_ << "] ";
*/
    }

public:
    ConsoleStatus();

    void addReducedBytes(long long bytes) {
        reducedBytes += bytes;
    }

    void addIteration() {
      iterations_++;
    }

    void addProgressBar(double percentage) {
        int space = remainingSpace() - 6;

        int fillUntil = static_cast<int>(space * percentage);
        int mid = space / 2 - 4;

        int intPercentage = static_cast<int>(percentage * 100);

        if (intPercentage < 100)
            mid++;
        if (intPercentage < 10)
            mid++;
        std::string pmsg = std::to_string(intPercentage) + "%";

        std::stringstream buf;
        if (space < 10)
            return;
        buf << "[";
        for (int i = 0; i < space; i++) {
            if (i == mid) {
                buf << pmsg;
                i += pmsg.size() - 1;
                continue;
            }
            if (i < fillUntil) {
                buf << "#";
            } else {
                buf << "_";
            }
        }
        buf << "]";
        appendMessage(buf.str());
    }

    void appendMessage(const std::string& str) {
        if (readyToPrint_) {
            message_ << str;
        }
    }

    void appendMessage(const char* str) {
        if (readyToPrint_) {
            message_ << str;
        }
    }

    void appendMessage(long i) {
        if (readyToPrint_) {
            message_ << i;
        }
    }

    void appendMessage(double i) {
        if (readyToPrint_) {
            message_ << i;
        }
    }

    void display() {
        if (readyToPrint_) {
            readyToPrint_ = false;
        } else {
            if (checkTimeToPrint()) {
                readyToPrint_ = true;
            }
            return;
        }

        std::cout << '\r';

        std::string messageString = message_.str();

        if (messageString.size() > maxLength) {
            messageString = messageString.substr(0, maxLength);
        }
        std::cout << messageString;

        size_t printedSize = messageString.size();
        while (printedSize <= maxLength) {
            std::cout << ' ';
            printedSize++;
        }
        std::cout.flush();
        resetMessage();
    }

};

#endif // CONSOLESTATUS_H
