#ifndef MP3_EDIT_H
#define MP3_EDIT_H

void title_edit(char *file, char *edit_text);
void artist_edit(char *file, char *edit_text);
void album_edit(char *file, char *edit_text);
void year_edit(char *file, char *edit_text);
void comment_edit(char *file, char *edit_text);
void content_edit(char *file, char *edit_text);
FILE *file_check(char *file_name);

#endif