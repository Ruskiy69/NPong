#include "Logging.h"

using namespace std;

/*
    There are 3 overloaded constructors, one for a string as 
    a filename, another as a C-string, and one for no filename
    at all.
*/

Log::Log(string& filename)
{
    this->logFile.open(filename);
    this->exit = true;
}

Log::Log(const char* filename)
{
    this->logFile.open(filename);
    this->exit = true;
}

Log::Log()
{
    this->logFile.open("Debug.log");
    this->exit = true;
}

Log::~Log()
{
    this->logFile.close();
}

void Log::log(const string& message, LOG_LEVEL level)
{
    this->log(message.c_str(), level);
}

void Log::log(const char* message, LOG_LEVEL level)
{
    // Get the current time
    time(&this->timer);
    string now = ctime(&this->timer);

    // Remove a newline (\n) if found at the end of the string.
    if(now.find('\n') == now.length() - 1)
        now = now.erase(now.length() - 2);

    // Based on the log level, record the appropriate message.
    switch(level)
    {
        case INFO:
            this->logFile << now << " [INFO]    : " << message << endl;
            break;
        case DEBUG:
#ifdef _DEBUG
            this->logFile << now << " [DEBUG]   : " << message << endl;
#endif
            break;
        case ERRORS:
            this->logFile << now << " [ERROR]   : " << message << endl;
            break;
        case FATAL:
            this->logFile << now << " [FATAL]   : " << message << endl;
            if(this->exit)
                ::exit(1);
            break;
        default:
            this->logFile << now << " [INFO]    : " << message << endl;
            break;
    }
    this->logFile.flush();
}

void Log::log(const int message, LOG_LEVEL level)
{
    // Get the current time
    time(&this->timer);
    string now = ctime(&this->timer);

    // Remove a newline (\n) if found in the string.
    now = now.substr(0, now.find("\n") == string::npos ? -1 : now.find("\n"));

    // Based on the log level, record the appropriate message.
    switch(level)
    {
        case INFO:
            this->logFile << now << " [INFO]    : " << message << endl;
            break;
        case DEBUG:
#ifdef _DEBUG
            this->logFile << now << " [DEBUG]   : " << message << endl;
#endif
            break;
        case ERRORS:
            this->logFile << now << " [ERROR]   : " << message << endl;
            break;
        case FATAL:
            this->logFile << now << " [FATAL]   : " << message << endl;
            if(this->exit)
                ::exit(1);
            break;
        default:
            this->logFile << now << " [INFO]    : " << message << endl;
            break;
    }
    this->logFile.flush();
}

void Log::noFatal()
{
    // If this is called, don't exit after a FATAL log message.
    this->exit = false;
}

void Log::Fatal()
{
    // If this is called, exit after a FATAL log message.
    // This is on by default.
    this->exit = true;
}