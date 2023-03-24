@ECHO OFF

start "Process 1" %~dp0\file-copy-shared-memory.exe --in="your.file" --out="output.file" --name="FILE_COPY_SHARED"

start "Process 2" %~dp0\file-copy-shared-memory.exe --in="your.file" --out="output.file" --name="FILE_COPY_SHARED"

@ECHO ON