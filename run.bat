@ECHO OFF

start "" %~dp0\file-copy-shared-memory.exe --in="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\Itou_Junj_Collection_[01]_[AniLibria_TV]_[HDTV-Rip_720p].mkv" --out="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\output.mkv" --type="producer"

start "" %~dp0\file-copy-shared-memory.exe --in="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\Itou_Junj_Collection_[01]_[AniLibria_TV]_[HDTV-Rip_720p].mkv" --out="C:\My\Projects\cpp\file-copy-shared-memory\deploy\Debug\output.mkv" --type="consumer"

@ECHO ON