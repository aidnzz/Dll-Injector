# Dll-Injector

A simple dll injector which uses VirtualAllocEx and CreateRemoteThread to launch dll in target process. Dll Injector is a simple to use command line tool which efficiently injects dll's into almost any program:

```
  ============
  Dll Injector
  ============

  Usage:
    Dllinject.exe <DLL path> <process name>
    Dllinject.exe <DLL path>
```

# To use the simple Injector class:
   
``` cpp
#include "Injector.h"
#include <iostream> // For input / output and exceptions

int main()
{
  Injector injector
  
  try 
  {
    injector.attach(processName);
    injector.inject(dllPath);
  }
  catch(const std::runtime_error& e) // To catch any exceptions
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
```
