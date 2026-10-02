#include<stdio.h>
#include<string.h>
#include"mp3_view.h"
#include"mp3_edit.h"
#include<stdlib.h>

static int read_syncsafe_size(const unsigned char *size)
{
    return ((int)(size[0] & 0x7F) << 21) | ((int)(size[1] & 0x7F) << 14) | ((int)(size[2] & 0x7F) << 7) |(int)(size[3] & 0x7F);
}

int read_size(unsigned char *size)
{
    return ((int)size[0] << 24) |((int)size[1] << 16) | ((int)size[2] << 8) | (int)size[3];
}

static void strip_encoding_byte(char *data, int frame_size)
{
    if (frame_size > 0 && (unsigned char)data[0] <= 3)
    {
        memmove(data, data + 1, (size_t)frame_size);
        data[frame_size - 1] = '\0';
    }
}

void display_mp3(const char *file_name)
{
    FILE *fptr = fopen(file_name, "rb");
    if (fptr == NULL)
    {
        printf("ERROR : File is not found\n");
        return;
    }

    unsigned char header[10] = {0};
    rewind(fptr);
    if (fread(header, 1, 10, fptr) == 10 && strncmp((const char *)header, "ID3", 3) == 0)
    {
        display_ID3V2(fptr);
        fclose(fptr);
        return;
    }

    unsigned char tag[128] = {0};
    if (fseek(fptr, -128, SEEK_END) == 0)
    {
        if (fread(tag, 1, 128, fptr) == 128 && strncmp((const char *)tag, "TAG", 3) == 0)
        {
            display_ID3V1(fptr);
            fclose(fptr);
            return;
        }
    }

    fclose(fptr);
    printf("ERROR : TAG/ID3 is not found\n");
}

void display_ID3V1(FILE *fptr)
{
    uch tag[128];
    if (fseek(fptr, -128, SEEK_END) != 0)
    {
        printf("ERROR: Unable to seek ID3V1 tag\n");
        return;
    }
    if (fread(tag, 1, 128, fptr) != 128)
        return;

    uch title[31];
    uch artist[31];
    uch album[31];
    uch year[5];
    uch comment[31];

    memcpy(title, tag + 3, 30);
    title[30] = '\0';

    memcpy(artist, tag + 33, 30);
    artist[30] = '\0';

    memcpy(album, tag + 63, 30);
    album[30] = '\0';

    memcpy(year, tag + 93, 4);
    year[4] = '\0';

    memcpy(comment, tag + 97, 30);
    comment[30] = '\0';

    printf("=========================================================\n");
    printf("              SELECTED VIEW DETAILS    \n");
    printf("=========================================================\n");
    printf("        MP3 TAG READER FOR ID3V1 VERSION \n");
    printf("=========================================================\n");
    printf("| %-15s | %-35s |\n", "Title", title);
    printf("| %-15s | %-35s |\n", "Artist", artist);
    printf("| %-15s | %-35s |\n", "Album", album);
    printf("| %-15s | %-35s |\n", "Year", year);
    printf("| %-15s | %-35s |\n", "Comment", comment);
    printf("| %-15s | %-35d |\n", "Genre", (unsigned char)tag[127]);
    printf("=========================================================\n");
    printf("         DETAILS DISPLAYED SUCCESSFULLY\n");
    printf("=========================================================\n");
}

void display_ID3V2(FILE *fptr)
{
    uch header[10];
    rewind(fptr);

    if (fread(header, 1, 10, fptr) != 10)
    {
        printf("ERROR : unable to read ID3V2 header\n");
        return;
    }

    if (strncmp((const char *)header, "ID3", 3) != 0)
    {
        printf("ERROR : ID3 tag not found\n");
        return;
    }

    int version = header[3];
    int tag_size = read_syncsafe_size(header + 6);
    int remaining = tag_size;

    printf("ID3V2.%d.%d\n", header[3], header[4]);

    uch title[100] = "Not Available";
    uch artist[100] = "Not Available";
    uch album[100] = "Not Available";
    uch year[20] = "Not Available";
    uch genre[100] = "Not Available";
    uch composer[100] = "Not Available";

    while (remaining >= 10)
    {
        uch frame_id[5] = {0};
        uch size_bytes[4] = {0};
        uch flags[2] = {0};
        int frame_size = 0;
        
        if (fread(frame_id, 1, 4, fptr) != 4)
            break;

        if (frame_id[0] == '\0')
            break;

        if (version == 2)
        {
            if (fread(frame_id, 1, 3, fptr) != 3)
                break;
            frame_id[3] = '\0';
            if (frame_id[0] == '\0')
                break;
            if (fread(size_bytes, 1, 3, fptr) != 3)
                break;
            frame_size = ((int)size_bytes[0] << 16) | ((int)size_bytes[1] << 8) | (int)size_bytes[2];
        }
        else
        {
            frame_id[4] = '\0';
            if (fread(size_bytes, 1, 4, fptr) != 4)
                break;
            if (fread(flags, 1, 2, fptr) != 2)
                break;
            frame_size = read_size(size_bytes);
        }

        if (frame_size <= 0)
            break;

        if (strcmp((char *)frame_id, "TIT2") != 0 &&
            strcmp((char *)frame_id, "TT2") != 0 &&
            strcmp((char *)frame_id, "TPE1") != 0 &&
            strcmp((char *)frame_id, "TP1") != 0 &&
            strcmp((char *)frame_id, "TALB") != 0 &&
            strcmp((char *)frame_id, "TAL") != 0 &&
            strcmp((char *)frame_id, "TYER") != 0 &&
            strcmp((char *)frame_id, "TYE") != 0 &&
            strcmp((char *)frame_id, "TCON") != 0 &&
            strcmp((char *)frame_id, "TCO") != 0 &&
            strcmp((char *)frame_id, "TCOM") != 0 &&
            strcmp((char *)frame_id, "TCM") != 0)
        {
            if (fseek(fptr, frame_size, SEEK_CUR) != 0)
                break;
            remaining -= (version == 2) ? (6 + frame_size) : (10 + frame_size);
            continue;
        }

        char *data = malloc((size_t)frame_size + 1);
        if (data == NULL)
        {
            printf("ERROR : Memory allocation failed\n");
            return;
        }

        if (fread(data, 1, (size_t)frame_size, fptr) != (size_t)frame_size)
        {
            free(data);
            break;
        }

        data[frame_size] = '\0';
        if (frame_size > 0 && (unsigned char)data[0] <= 3)
            strip_encoding_byte(data, frame_size);

        if (strcmp((char *)frame_id, "TIT2") == 0 || strcmp((char *)frame_id, "TT2") == 0)
        {
            strncpy((char *)title, data, sizeof(title) - 1);
            title[sizeof(title) - 1] = '\0';
        }
        else if (strcmp((char *)frame_id, "TPE1") == 0 || strcmp((char *)frame_id, "TP1") == 0)
        {
            strncpy((char *)artist, data, sizeof(artist) - 1);
            artist[sizeof(artist) - 1] = '\0';
        }
        else if (strcmp((char *)frame_id, "TALB") == 0 || strcmp((char *)frame_id, "TAL") == 0)
        {
            strncpy((char *)album, data, sizeof(album) - 1);
            album[sizeof(album) - 1] = '\0';
        }
        else if (strcmp((char *)frame_id, "TYER") == 0 || strcmp((char *)frame_id, "TYE") == 0)
        {
            strncpy((char *)year, data, sizeof(year) - 1);
            year[sizeof(year) - 1] = '\0';
        }
        else if (strcmp((char *)frame_id, "TCON") == 0 || strcmp((char *)frame_id, "TCO") == 0)
        {
            strncpy((char *)genre, data, sizeof(genre) - 1);
            genre[sizeof(genre) - 1] = '\0';
        }
        else if (strcmp((char *)frame_id, "TCOM") == 0 || strcmp((char *)frame_id, "TCM") == 0)
        {
            strncpy((char *)composer, data, sizeof(composer) - 1);
            composer[sizeof(composer) - 1] = '\0';
        }

        free(data);
        remaining -= (version == 2) ? (6 + frame_size) : (10 + frame_size);
    }

    printf("\n");
    printf("=========================================================\n");
    printf("               MP3 TAG READER FOR ID3V2 VERSION\n");
    printf("=========================================================\n");
    printf("| %-15s | %-35s |\n", "Field", "Value");
    printf("=========================================================\n");
    printf("| %-15s | %-35s |\n", "Title", title);
    printf("| %-15s | %-35s |\n", "Artist", artist);
    printf("| %-15s | %-35s |\n", "Album", album);
    printf("| %-15s | %-35s |\n", "Year", year);
    printf("| %-15s | %-35s |\n", "Genre", genre);
    printf("| %-15s | %-35s |\n", "Composer", composer);
    printf("=========================================================\n");
}
