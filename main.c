/*documentation:

NAME       : veeramreddigari seshu kumar reddy.
ADMIN ID   : 2601_108.
START DATE : 10/09/2026.
END DATE   : 29/09/2026

MP3 TAG READER:
Developed a C-based MP3 Tag Reader that extracts and displays metadata stored in MP3 audio files using ID3v2 tags. 
The project demonstrates practical implementation of file handling, binary data processing, structures, pointers, and 
command-line arguments** in C.

 KEY FEATURES
* Reads and validates MP3 files.
* Detects and processes ID3v2 metadata.
* Extracts information such as:

  * Title
  * Artist
  * Album
  * Year
  * Genre
  * Comment
* Displays extracted metadata in a user-friendly format.
* Handles file operations using standard C library functions.
* Includes validation and error handling for invalid files or missing tags.

TECHNOLOGIES USED
C | File Handling | Structures | Pointers | Binary File Processing | ID3v2 Metadata**

USAGE:
---------------------------------------------------------------------------------------
ERROR : ./a.out : Invalid arguments
usage :
To view please pass like : ./a.out -v mp3_file_name
To edit please pass like : ./a.out -e -t/-a/-A/-y/-m/-c changing_text mp3_file_name
To help please pass like : ./a.out --help
---------------------------------------------------------------------------------------

SAMPLE OUTPUT:
ID3V2.3.0
=========================================================
               MP3 TAG READER FOR ID3V2 VERSION
=========================================================
| Field           | Value                               |
=========================================================
| Title           | ramul                               |
| Artist          | Sid Sriram                          |
| Album           | Ala Vaikunthapurramuloo (2020)      |
| Year            | 2020                                |
| Genre           | Telugu                              |
| Composer        | Thaman S                            |
=========================================================
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"mp3_edit.h"
#include"mp3_view.h"


int main(int argc, char*argv[])
{
    if(argc<3)
    {
        printf("---------------------------------------------------------------------------------------\n");
        printf("ERROR : ./a.out : Invalid arguments\n");
        printf("usage :\n");
        printf("To view please pass like : ./a.out -v mp3_file_name\n");
        printf("To edit please pass like : ./a.out -e -t/-a/-A/-y/-m/-c changing_text mp3_file_name\n");
        printf("To help please pass like : ./a.out --help\n");
        printf("---------------------------------------------------------------------------------------\n");
        return 0;
    }
    else if(strcmp(argv[1],"-v")==0)
    {
        if(argc >= 3 && argv[2][0] != '\0')
        {
            char *ext = strrchr(argv[2], '.');
            if(ext != NULL && strcmp(ext, ".mp3") == 0)
            {
                display_mp3(argv[2]);
                return 0;
            }
        }
        printf("ERROR : Invalid MP3 file name\n");
        return 0;
    }
    else if(strcmp(argv[1],"-e")==0)
    {
        if(argc >= 5)
        {
            if(strcmp(argv[2],"-t")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    title_edit(argv[4], argv[3]);
                    return 0;
                }
            }
            else if(strcmp(argv[2],"-a")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    artist_edit(argv[4], argv[3]);
                    return 0;
                }
            }
            else if(strcmp(argv[2],"-A")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    album_edit(argv[4], argv[3]);
                    return 0;
                }
            }
            else if(strcmp(argv[2],"-y")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    year_edit(argv[4], argv[3]);
                    return 0;
                }
            }
            else if(strcmp(argv[2],"-m")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    comment_edit(argv[4], argv[3]);
                    return 0;
                }
            }
            else if(strcmp(argv[2],"-c")==0)
            {
                char *ext = strrchr(argv[4], '.');
                if(ext != NULL && strcmp(ext, ".mp3") == 0)
                {
                    content_edit(argv[4], argv[3]);
                    return 0;
                }
            }
        }

        printf("ERROR : please enter the followed operation\n");
        return 0;
    }
    else if(strcmp(argv[1],"--help")==0)
    {
        printf("----------------- HELP ----------------\n");
        printf("1. -v -> to view mp3 file content\n");
        printf("2. -e -> to edit mp3 file content\n");
        printf("    2.1. -t -> to edit song tile \n");
        printf("    2.2. -a -> to edit artist name\n");
        printf("    2.3. -A -> to edit album name\n");
        printf("    2.4. -y -> to edit year\n");
        printf("    2.5. -m -> to edit content\n");
        printf("    2.6. -c -> to edit comment\n");
        printf("----------------------------------------\n");
        return 0;
    }
    else
    {
        printf("ERROR : Invalid file \n");
        return 0;
    }
    
}