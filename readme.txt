To compile and run this program run open solution in VS 2019 and select Build Solution.

There are no external dependencies outside of VS 2019.  The project is currently set to compile against ISO C++ v14 
so anyone who wants avoid using VS could use any IDE that supports ISO C++ v14.  You just need to include 
CJumpCloudJsonStats.cpp, CJumpCloudJsonStats.h, TestInput.h, and JumpcloudJsonStats.cpp in your project.

To run just the test executable you can run x64\Debug\JumpCloudJsonStats.exe on x64 release of Windows 8 or 
Windows 10 release.

Test results are outputted to command line.  SUCCESS means test case succeeded.  ERROR means test case failed.

The class with functionality is CJumpCloudJsonStats.  This class has the addAction and getStats functions.  I also
added a few more public functions so callers could validate behavior since JSON is not supported natively yet
in C++.  (Sounds like 2022 is the year for native JSON support in C++).

The main function in JumpCloudJsonStats.cpp is the test driver for CJumpCloudJsonStats class.