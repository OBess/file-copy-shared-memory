# file-copy-shared-memory-shared-memory
The tool to copy data from one file to another using named shared memory

## How to use
You can use this tool to copy any file you have access to.

### For producer
To start copy, just write in the command line 
```
file-copy-shared-memory.exe target source produer
```
or
```
file-copy-shared-memory.exe "target" "source" "producer"
```
or
```
file-copy-shared-memory.exe --in="target" --out="source" --type="producer"
```

### For consumer
To start copy, just write in the command line 
```
file-copy-shared-memory.exe target source consumer
```
or
```
file-copy-shared-memory.exe "target" "source" "consumer"
```
or
```
file-copy-shared-memory.exe --in="target" --out="source" --type="consumer"
```

Or, you can execute "run.bat" in the same dir where is "file-copy-shared-memory.exe", but redefine you pathes

Also, there is a command "help", that you can get running this like
```
file-copy-shared-memory.exe --help
```

After you run file-copy-shared-memory, you must see the log files where you will see messages about processes done their tasks.