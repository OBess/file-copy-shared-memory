@ECHO OFF

start "Process 1" %~dp0\file-copy-shared-memory.exe --in="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\Itou_Junj_Collection_[01]_[AniLibria_TV]_[HDTV-Rip_720p].mkv" --out="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\output.mkv" --name="FILE_COPY_SHARED"

start "Process 2" %~dp0\file-copy-shared-memory.exe --in="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\Itou_Junj_Collection_[01]_[AniLibria_TV]_[HDTV-Rip_720p].mkv" --out="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\output.mkv" --name="FILE_COPY_SHARED"

@ECHO ON