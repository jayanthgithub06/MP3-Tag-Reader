#ifndef MP3_H
#define MP3_H
#include "types.h"
#include<stdio.h>
#include<unistd.h>


#define MODE 5
#define VIEW 5

typedef struct Mp3_tag
{
    char *file_name;
    FILE *fptr;
   
    char tag_modifier[5];
    char new_data[100];
    int new_data_size;
    FILE *fptr_copy;



}Mp3_tag;

Status open_files(char *file_name, FILE **fptr);
Status list_all_tags(Mp3_tag *mp3);

OperationType Check_operation(char *argv[]);
Status read_validate_mp3files_view(Mp3_tag *mp3, char *argv[]);
Status read_validate_mp3files_search(Mp3_tag *mp3, char *argv[]);
Status read_validate_mp3files_edit(Mp3_tag *mp3, char *argv[]);

Status edit_tag_data(Mp3_tag *mp3);
unsigned int swap(unsigned int size);
void display_all_tags(FILE *fptr);
Status find_and_display_tag(FILE *fptr, const char *tag_id);
const char* get_tag_name(const char *tag_id); 

#endif