#ifndef MP3_VIEW_H
#define MP3_VIEW_H

typedef unsigned char uch;

void display_mp3(const char *file);
void display_ID3V1(FILE *ptr);
void display_ID3V2(FILE *ptr);
int read_size(unsigned char *size);
#endif