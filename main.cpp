/**
 * Main entry point
 * USC csci 580 *
*/

#include "Application1.h"
#include "Application2.h"
#include "Application3.h"

#include "test.h"

int main(int argc, const char * argv[])
{
    // For Application1 in HW1.
    // GzDisplayApplication dispApp;
    // runRenderer(&dispApp);
     
    
    // For Application 2
    // Application2 application;
    // application.runRenderer();
    
    // For Application 3
    Application3 application;
    application.runRenderer();
    
    // For tests.
    // testMatrixMultiply();
    // testMatrixMultiplyVector();
    
    return 0;
}

