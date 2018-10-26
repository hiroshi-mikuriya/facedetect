#include "stdafx.h"

#ifdef TEST_MODE

#if defined _WIN32
#pragma comment( lib, "kernel32.lib" )
#pragma comment( lib, "user32.lib" )
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "comdlg32.lib" )
#pragma comment( lib, "shell32.lib" )
#endif


#include <gtest/gtest.h>
int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif // TEST_MODE
