#include "Errors.h"

void handleError(const char* errorMessage, const bool quit)
{
#ifdef _WIN32
    MessageBoxA(NULL, errorMessage, "Error!", MB_ICONEXCLAMATION | MB_OK | MB_ICONERROR);
#else
    if(quit)
        fprintf(stderr, "[-] FATAL! %s", errorMessage);
        fprintf(stderr, "Press ENTER to exit...");
    else
        fprintf(stderr, "[-] A non fatal error occured: %s", errorMessage);
        fprintf(stderr, "Press ENTER to continue...");
    gets();
#endif // _WIN32

    if(quit)
        exit(1);
}