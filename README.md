# Dll-Injector

A simple dll injector which uses VirtualAllocEx and CreateRemoteThread to launch dll in target process

Dll Injector is a simple to use command line tool which efficiently injects dll's into almost any program:

```
  ============
  Dll Injector
  ============

  Usage:
    Dllinject.exe <DLL path> <process name>
    Dllinject.exe <DLL path>
```

To use the simple Injector class:
   
`injector.attach(processName);`
`injector.inject(dllPath)`
