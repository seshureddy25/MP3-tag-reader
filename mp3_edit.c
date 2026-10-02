#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mp3_edit.h"
#include"mp3_view.h"

static int read_syncsafe_size(const unsigned char *size)
{
    return ((int)(size[0] & 0x7F) << 21) |
           ((int)(size[1] & 0x7F) << 14) |
           ((int)(size[2] & 0x7F) << 7) |
           (int)(size[3] & 0x7F);
}

static int update_id3v1_field(FILE *ptr, long offset_from_end, const char *value, size_t width)
{
    unsigned char tag[128] = {0};
    if (fseek(ptr, -128, SEEK_END) != 0)
        return 0;
    if (fread(tag, 1, 128, ptr) != 128)
        return 0;
    if (strncmp((const char *)tag, "TAG", 3) != 0)
        return 0;

    char field[64] = {0};
    size_t copy_len = strlen(value);
    if (copy_len > width)
        copy_len = width;
    memset(field, ' ', width);
    memcpy(field, value, copy_len);

    fseek(ptr, offset_from_end, SEEK_END);
    fwrite(field, 1, width, ptr);
    return 1;
}

static int update_id3v2_frame(FILE *ptr, const char *frame_id, const char *value)
{
    unsigned char header[10] = {0};
    rewind(ptr);
    if (fread(header, 1, 10, ptr) != 10)
        return 0;
    if (strncmp((const char *)header, "ID3", 3) != 0)
        return 0;

    int version = header[3];
    int tag_size = read_syncsafe_size(header + 6);
    long pos = 10;
    long end = 10 + tag_size;

    while (pos + 10 <= end)
    {
        unsigned char frame[5] = {0};
        unsigned char size_bytes[4] = {0};
        unsigned char flags[2] = {0};
        int frame_size = 0;

        fseek(ptr, pos, SEEK_SET);
        if (fread(frame, 1, 4, ptr) != 4)
            break;
        if (frame[0] == '\0')
            break;

        if (version == 2)
        {
            if (fread(frame, 1, 3, ptr) != 3)
                break;
            frame[3] = '\0';
            if (fread(size_bytes, 1, 3, ptr) != 3)
                break;
            frame_size = ((int)size_bytes[0] << 16) |
                         ((int)size_bytes[1] << 8) |
                         (int)size_bytes[2];
        }
        else
        {
            frame[4] = '\0';
            if (fread(size_bytes, 1, 4, ptr) != 4)
                break;
            if (fread(flags, 1, 2, ptr) != 2)
                break;
            frame_size = ((int)size_bytes[0] << 24) |
                         ((int)size_bytes[1] << 16) |
                         ((int)size_bytes[2] << 8) |
                         (int)size_bytes[3];
        }

        if (frame_size <= 0)
            break;

        if (strcmp((const char *)frame, frame_id) == 0)
        {
            char *payload = malloc((size_t)frame_size + 1);
            if (payload == NULL)
                return 0;

            fseek(ptr, pos + (version == 2 ? 6 : 10), SEEK_SET);
            if (fread(payload, 1, (size_t)frame_size, ptr) != (size_t)frame_size)
            {
                free(payload);
                return 0;
            }
            payload[frame_size] = '\0';

            size_t available = (size_t)frame_size;
            if (frame_size > 0 && (unsigned char)payload[0] <= 3)
            {
                memmove(payload, payload + 1, (size_t)frame_size - 1);
                payload[frame_size - 1] = '\0';
                available = (size_t)frame_size - 1;
            }

            size_t copy_len = strlen(value);
            if (copy_len > available)
                copy_len = available;

            char *new_payload = calloc((size_t)frame_size + 1, 1);
            if (new_payload == NULL)
            {
                free(payload);
                return 0;
            }
            if (frame_size > 0)
                new_payload[0] = 0;
            memcpy(new_payload + (frame_size > 0 ? 1 : 0), value, copy_len);

            fseek(ptr, pos + (version == 2 ? 6 : 10), SEEK_SET);
            fwrite(new_payload, 1, (size_t)frame_size, ptr);
            free(payload);
            free(new_payload);
            rewind(ptr);
            return 1;
        }

        pos += (version == 2) ? (6 + frame_size) : (10 + frame_size);
    }

    return 0;
}

FILE *file_check(char *file_name)
{
    FILE *ptr;
    if((ptr=fopen(file_name, "r+b"))==NULL)
    {
        printf("ERROR : file is not found\n");
        return NULL;
    }

    unsigned char header[10] = {0};
    rewind(ptr);
    if (fread(header, 1, 10, ptr) == 10 && strncmp((const char *)header, "ID3", 3) == 0)
        return ptr;

    unsigned char tag[128] = {0};
    if (fseek(ptr, -128, SEEK_END) == 0)
    {
        if (fread(tag, 1, 128, ptr) == 128 && strncmp((const char *)tag, "TAG", 3) == 0)
            return ptr;
    }

    fclose(ptr);
    printf("ERROR : ID3V1/V2 TAG is not found\n");
    return NULL;
}

void title_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    if (update_id3v2_frame(ptr, "TIT2", edit_text))
    {
        printf("=========================================================\n");
        printf("            TITLE UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35s |\n", "Title", edit_text);
        printf("=========================================================\n");
        fclose(ptr);
        return;
    }

    if (update_id3v1_field(ptr, -125, edit_text, 30))
    {
        printf("=========================================================\n");
        printf("            TITLE UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35.30s |\n", "Title", edit_text);
        printf("=========================================================\n");
    }
    fclose(ptr);
}

void album_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    if (update_id3v2_frame(ptr, "TALB", edit_text))
    {
        printf("=========================================================\n");
        printf("            ALBUM UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35s |\n", "Album", edit_text);
        printf("=========================================================\n");
        fclose(ptr);
        return;
    }

    if (update_id3v1_field(ptr, -65, edit_text, 30))
    {
        printf("=========================================================\n");
        printf("            ALBUM UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35.30s |\n", "Album", edit_text);
        printf("=========================================================\n");
    }
    fclose(ptr);
}

void artist_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    if (update_id3v2_frame(ptr, "TPE1", edit_text))
    {
        printf("=========================================================\n");
        printf("            ARTIST UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35s |\n", "Artist", edit_text);
        printf("=========================================================\n");
        fclose(ptr);
        return;
    }

    if (update_id3v1_field(ptr, -95, edit_text, 30))
    {
        printf("=========================================================\n");
        printf("            ARTIST UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35.30s |\n", "Artist", edit_text);
        printf("=========================================================\n");
    }
    fclose(ptr);
}

void year_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    if (update_id3v2_frame(ptr, "TYER", edit_text))
    {
        printf("=========================================================\n");
        printf("            YEAR UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35s |\n", "Year", edit_text);
        printf("=========================================================\n");
        fclose(ptr);
        return;
    }

    if (update_id3v1_field(ptr, -31, edit_text, 4))
    {
        printf("=========================================================\n");
        printf("            YEAR UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35.30s |\n", "Year", edit_text);
        printf("=========================================================\n");
    }
    fclose(ptr);
}

void comment_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    if (update_id3v2_frame(ptr, "COMM", edit_text))
    {
        printf("=========================================================\n");
        printf("            COMMENT UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35s |\n", "Comment", edit_text);
        printf("=========================================================\n");
        fclose(ptr);
        return;
    }

    if (update_id3v1_field(ptr, -30, edit_text, 28))
    {
        printf("=========================================================\n");
        printf("            COMMENT UPDATED SUCCESSFULLY \n");
        printf("=========================================================\n");
        printf("| %-15s | %-35.30s |\n", "Comment", edit_text);
        printf("=========================================================\n");
    }
    fclose(ptr);
}

void content_edit(char *file_name, char *edit_text)
{
    FILE *ptr=file_check(file_name);
    if(ptr==NULL)
        return;

    printf("ERROR : content editing is not supported in this version\n");
    fclose(ptr);
}
