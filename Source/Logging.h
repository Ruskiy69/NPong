/*
    A very simple logging class to record various messages to
    a text file. The different levels are:
        - INFO
        - DEBUG
        - ERRORS
        - FATAL

    DEBUG records won't be recorded in a Release Build.
    FATAL records will exit the program unless noFatal()
          is called.

    Example usage:
        int main()
        {
            Log logfile("InfoLog.log");
            logfile.log("Testing log message...", INFO);
            logfile.log("Is it working...?", DEBUG);
            logfile.log("An error occured!", ERROR);
            logfile.noFatal();
            logfile.log("An unrecoverable error occured!", FATAL);
            logfile.Fatal();
            logfile.log("An unrecoverable error occured!", FATAL);
            return 0;
        }
*/        

#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <string>
#include <ctime>

/* An easy to use macro for logging,
 * but you must have a Log* defined.
 */
#define LOG(str, lvl) log->log(str, lvl);

using namespace std;

const enum LOG_LEVEL 
{
    INFO = 69,
    DEBUG,
    ERRORS,
    FATAL
};

class Log
{
public:
    Log(string& filename);
    Log(const char* filename);
    Log();
    ~Log();

    void log(const string& str, LOG_LEVEL level);
    void log(const char* str, LOG_LEVEL level);
    void log(const int str, LOG_LEVEL level);

    void noFatal();
    void Fatal();

protected:
    ofstream    logFile;
    string      message;
    time_t      timer;
    bool        exit;
};


#endif // LOGGING_H