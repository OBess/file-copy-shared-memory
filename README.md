# file-copy-shared-memory-shared-memory
The tool to copy data from one file to another using named shared memory

## How to use
You can use this tool to copy any file you have access to.

To start copy, just write in the command line 
```
file-copy-shared-memory.exe target source name_shared_memory
```
or
```
file-copy-shared-memory.exe "target" "source" "name_shared_memory"
```
or
```
file-copy-shared-memory.exe --in="target" --out="source" --name="name_shared_memory"
```

Or, you can execute "run.bat" in the same dir where is "file-copy-shared-memory.exe", but redefine your pathes

Also, there is a command "help", that you can get running this like
```
file-copy-shared-memory.exe --help
```

After you run file-copy-shared-memory, you must see the log files where you will see messages about processes done their tasks.