#pragma once
void FileToMemoryFunciton(char* FileBuffer, char* ImageBuffer);

void MemoryToFileFunciton(char* FileBuffer, char* ImageBuffer);

int RvaToFoa(int Rva, char* ImageBuffer);

int GetPeHeaders(char* Filepath);