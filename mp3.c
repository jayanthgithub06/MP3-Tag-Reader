#include "mp3.h"
#include <stdio.h>
#include "types.h"
#include <stdlib.h>
#include <string.h>



Status open_files(char *file_name, FILE **fptr)
{
    // mp3->file_name = "song3.mp3";
    *fptr = fopen(file_name, "r+");

    if (*fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",file_name);
        return failure;
    }
    fseek(*fptr, 10, SEEK_SET); 
}

// to check for operation type
OperationType Check_operation(char *argv[])
{
    if(strncmp (argv[1],"-v",2) == 0)   //for view 
        return mp3_view;

    else if (strncmp (argv[1],"-e",2) == 0)   //for edit
        return mp3_edit;
    
    else if (strncmp (argv[1], "--help", 6) ==0 )    // for help
        return mp3_help;

    else if (strncmp (argv[1], "-s", 2) == 0)   //for view all tags
        return mp3_search;

    else
        mp3_unsupported;         
}


// this function list all the tags present in the song
Status list_all_tags(Mp3_tag *mp3)
{

    mp3->fptr=fopen(mp3->file_name,"r");

    if (mp3->fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",mp3->file_name);
        return failure;
    }
    fseek(mp3->fptr, 10, SEEK_SET);   //skip first 10 header files

    int tag_count = 0;      //to keep count of tags present in song

    while (1)
    {
        char tag[4] = {0}; 


        // to read tag from the song
        if (fread(tag, 1, 4, mp3->fptr) != 4)
        {
            if (feof(mp3->fptr))
            {
                printf("End of file reached.\n");
                break;
            }
            else
            {
                perror("Error reading tag");
                fclose(mp3->fptr);
                return failure;
            }
        }

        //condition for end of tags
        if (tag[0] == '\0') 
        {
            printf("Empty tag encountered. Ending tag processing.\n");
            break;
        }

        printf("tag = %s\n", tag);  //printing the tag

        // to read the data size of the data of that particular tag
        unsigned int data_size;
        if (fread(&data_size, 4, 1, mp3->fptr) != 1)
        {
            perror("Error reading data size");
            fclose(mp3->fptr);
            return failure;
        }

        // for invalid data size
        if (data_size <= 0)
        {
            // fprintf(stderr, "Invalid data size: %u\n", data_size);
            printf("invalid data size =%d\n",data_size);
            break;
        }

        // to convert little indian to big indian
        char *ptr = (char *)&data_size;
        for (int i = 0; i < sizeof(int) / 2; i++)
        {
            char temp = *(ptr + i);
            *(ptr + i) = *(ptr + sizeof(int) - 1 - i);
            *(ptr + sizeof(int) - 1 - i) = temp;
        }

        // printing data size
        printf("data size = %d\n", data_size);

        //dynamic allocation of data usnig data size
        char *data = (char *)malloc(data_size * sizeof(char));
        if (data == NULL)
        {
            perror("Memory allocation failed");
            fclose(mp3->fptr);
            return failure;
        }

        //after reading data we have to skip the flags
        fseek(mp3->fptr, 3L, SEEK_CUR);

        // reading the data from the file and storing in to the memory allocated data variable
        if (fread(data, 1, data_size - 1, mp3->fptr) != data_size - 1)
        {
            perror("Error reading data");
            free(data);
            fclose(mp3->fptr);
            return failure;
        }

        data[data_size - 1] = '\0';  //after storing the data initialize it to null

        // for(int i=0;i<data_size;i++)
        // {
        //     if(data[i]!='\0')
        //     {
        //         printf("%c",data[i]);
        //     }
        // }

        printf("data = ");
        for (int i = 0; i < data_size; i++) 
        {
            if (data[i] >= 32 && data[i] <= 122) 
            { // Printable ASCII range
                printf("%c", data[i]);
            }
        }

        printf("\n");
        // printf("data = %s\n", data);   //printing the data

        tag_count++;   //increment the tag count
        free(data);
    }

    printf("Total tags processed: %d\n", tag_count);   //printing the total tags preset in the song
    fclose(mp3->fptr);    //close file
    return success;
}

// read and validation for search
Status read_validate_mp3files_search(Mp3_tag *mp3, char *argv[])
{
    if(argv[2]==NULL)
    {
        printf ("INFO: For Viewing the Tags -> ./a.out -s <file_name.mp3>\n");
        return failure;
    }
    else
    {
        if(strstr(argv[2],".mp3"))   //check weather .mp3  file extension is present or not
        {
            mp3->file_name=argv[2];
            return success;
        }
        else
        {
            printf("unable to open the file %s\n",argv[2]);
            return failure;
        }

    }
}

// read and validation for view
Status read_validate_mp3files_view(Mp3_tag *mp3, char *argv[])
{
    if(argv[2]==NULL)
    {
        printf ("INFO: For Viewing the Tags -> ./a.out -v <file_name.mp3>\n");
        return failure;
    }
    else
    {
        if(strstr(argv[2],".mp3"))   //check weather .mp3  file extension is present or not
        {
            mp3->file_name=argv[2];
            // mp3->fptr=fopen(mp3->file_name,"r");
            return success;
        }
        else
        {
            printf("unable to open the file %s\n",argv[2]);
            return failure;
        }

    }


}

// read and validation for edit 
Status read_validate_mp3files_edit(Mp3_tag *mp3, char *argv[])
{
    if (argv[2] == NULL)
    {
        printf("ERROR: No tag modifier provided.\n");
        return failure;
    }

    // Match tag modifier using exact string comparisons
    if (strcmp(argv[2], "-t") == 0)
    {
        strncpy(mp3->tag_modifier, "TIT2",4);
        
    }
    else if (strcmp(argv[2], "-to") == 0)
    {
        strcpy(mp3->tag_modifier, "TOFN");
    }
    else if (strcmp(argv[2], "-tp") == 0)
    {
        strcpy(mp3->tag_modifier, "TPE1");
    }
    else if (strcmp(argv[2], "-top") == 0)
    {
        strncpy(mp3->tag_modifier, "TOPE",4);
    }
    else if (strcmp(argv[2], "-ta") == 0)
    {
        strcpy(mp3->tag_modifier, "TALB");
    }
    else if (strcmp(argv[2], "-y") == 0)
    {
        strcpy(mp3->tag_modifier, "TYER");
    }
    else if (strcmp(argv[2], "-w") == 0)
    {
        strcpy(mp3->tag_modifier, "WCOP");
    }
    else if (strcmp(argv[2], "-tp2") == 0)
    {
        strcpy(mp3->tag_modifier, "TPE2");
    }
    else if (strcmp(argv[2], "-tc") == 0)
    {
        strcpy(mp3->tag_modifier, "TCON");
    }
    else if (strcmp(argv[2], "-tow") == 0)
    {
        strcpy(mp3->tag_modifier, "TOWN");
    }
    else if (strcmp(argv[2], "-wx") == 0)
    {
        strcpy(mp3->tag_modifier, "WXXX");
    }
    else if (strcmp(argv[2], "-ts") == 0)
    {
        strcpy(mp3->tag_modifier, "TSSE");
    }
    else if(strcmp(argv[2],"-A")==0)
    {
        strcpy(mp3->tag_modifier,"APIC");
    }
    else
    {
        printf("ERROR: Unknown tag modifier '%s'.\n", argv[2]);  //for unknown tags
        return failure;
    }

    printf("Frame ID to be changed: %s.\n", mp3->tag_modifier);   //prints frame id to be changed

    if (argv[3] == NULL)
    {
        printf("ERROR: New value for tag %s is missing.\n", mp3->tag_modifier);   // if no data is provided in CLA
        return failure;
    }
    else
    {
        mp3->new_data_size = strlen(argv[3]) + 1;

        //to check the data size is more than 100
        if (mp3->new_data_size < 100)
        {
            //if data siz is less then continue the process
            printf("Length of the data in passed command line argument = %d.\n", mp3->new_data_size);
            strcpy(mp3->new_data, argv[3]);
            printf("Data to be modified in tag is %s.\n", mp3->new_data);
        }
        else
        {
            //print error message that data is long
            printf("ERROR: New data length is too long.\n");
            return failure;
        }
    }

    if (argv[4] == NULL) //if argument 4 is null
    {
        //print the instruction to edit the tag
        printf("ERROR: File name is missing (Usage: ./a.out -e modifier newdata file_name.mp3)\n");
        return failure;
    }
    else
    {
        //if it is not null check for the .mp3 extension 
        if (strstr(argv[4], ".mp3") != NULL)
        {
            mp3->file_name = argv[4];
            return success;
        }
        else
        {
            printf("ERROR: Unable to open file '%s'. Ensure it's an mp3 file.\n", argv[4]);
            return failure;
        }
    }
}

Status edit_tag_data(Mp3_tag *mp3)
{
    mp3->fptr = fopen(mp3->file_name, "r");  //open file
    if (mp3->fptr == NULL)  //if file is failed to open then print error message
    {
        printf("ERROR: Could not open file '%s' for editing.\n", mp3->file_name);
        return failure;
    }
    mp3->fptr_copy = fopen("new.mp3","w");  //create the new.mp3 file
    if (mp3->fptr == NULL)
    {
        //print error message if not opend
        printf("ERROR: Could not open file '%s' for editing.\n", "new.mp3");
        return failure;
    }

    fseek(mp3->fptr,0L,SEEK_SET);    // set the source pointer to beginning
    fseek(mp3->fptr_copy,0L,SEEK_SET);  //set the dest pointer to beginning

    char header[10]={0};

    fread(header,1,10,mp3->fptr);   //read the header from source file
    fwrite(header,1,10,mp3->fptr_copy);   //copy the header file to dest file

    while(mp3->fptr!=NULL)
    {
        unsigned char tag[5]={0};

        fread(tag,1,4,mp3->fptr);   //read the tag of 4 bytes from the source file
        fwrite(tag,1,4,mp3->fptr_copy);  //copy the tag to dest file


        //if tag[0] encounters null then no tags are present
        if(tag[0]=='\0')
        {
            printf("Empty tag encountered. Ending tag processing.\n");
            break;
        }

        //if end of file reached then print end of file
        if (feof(mp3->fptr))
        {
            printf("end of file reached\n");
        }
        printf("tag=%s\n",tag);  //print tag name

        //if tag is found 
        if(strcmp(tag,mp3->tag_modifier)==0)
        {
            printf("%s tag is found\n",tag);  //print the tag found message

            printf("size of new data=%d\n",mp3->new_data_size);  //print new data size
            printf("data to be changed is -> %s\n",mp3->new_data);  //print the new data to be changed

           mp3->new_data_size = swap(mp3->new_data_size);  //convert the little indian to big indian

            // char *ptr2 = (char *)&mp3->new_data_size;
            // for (int i = 0; i < sizeof(int) / 2; i++)
            // {
            //     char temp = *(ptr2 + i);
            //     *(ptr2 + i) = *(ptr2 + sizeof(int) - 1 - i);
            //      *(ptr2 + sizeof(int) - 1 - i) = temp;
            // }
            fwrite(&mp3->new_data_size,4,1,mp3->fptr_copy);  //write the new data size to the dest file

            int skip_old_size;   

            fread(&skip_old_size,4,1,mp3->fptr);     //skip old data size from the source file //skip old data from the source file

            skip_old_size=swap(skip_old_size);  //convert the old data size to the little indian

        // char *ptr = (char *)&skip_old_size;
        // for (int i = 0; i < sizeof(int) / 2; i++)
        // {
        //     char temp = *(ptr + i);
        //     *(ptr + i) = *(ptr + sizeof(int) - 1 - i);
        //     *(ptr + sizeof(int) - 1 - i) = temp;
        // }

        skip_old_size--;  //decrement the old size

        char ch[4];
        fread(ch,3,1,mp3->fptr);  //read three flags from the source file
        fwrite(ch,3,1,mp3->fptr_copy);  //write the 3 flags to dest files
        fwrite(mp3->new_data,strlen(mp3->new_data),1,mp3->fptr_copy);  //write the new data to dest file
        fseek(mp3->fptr,skip_old_size,SEEK_CUR);  //skip old size from source file

        }
        else
        {

            //if tag not found
            int old_data_size;
            fread(&old_data_size,4,1,mp3->fptr);  //read the old size from the source file
            fwrite(&old_data_size,4,1,mp3->fptr_copy);  //write the old data size to dest file
            
            old_data_size=swap(old_data_size);  //convert big endian to little endian

            // char *ptr1 = (char *)&old_data_size;
            // for (int i = 0; i < sizeof(int) / 2; i++)
            // {
            //     char temp = *(ptr1 + i);
            //     *(ptr1 + i) = *(ptr1 + sizeof(int) - 1 - i);
            //     *(ptr1 + sizeof(int) - 1 - i) = temp;
            // }

            old_data_size--;  //decrement the old data size

            char skip3bytes[3]= {0};   

            fread(skip3bytes,3,1,mp3->fptr);   //read the 3 flag bytes from the src file
            fwrite(skip3bytes,3,1,mp3->fptr_copy);  //write the 3 flag bytes to dest file

            char data[old_data_size];    
        
            fread(data,old_data_size,1,mp3->fptr);   //read the old data from the source file
            fwrite(data,old_data_size,1,mp3->fptr_copy);   //write the old data to dest file
        }
    }
    char ch;

    //copy remaining data to source to dest files
    while (fread(&ch,1,1,mp3->fptr))
    {
        fwrite(&ch,1,1,mp3->fptr_copy);

    }

    remove(mp3->file_name);   //remove source file 
    
    rename("new.mp3",mp3->file_name);  //rename the dest file as source file name
    

    fclose(mp3->fptr);  //close source file pointer
    fclose(mp3->fptr_copy);  //close dest file pointer




}

//convert big endian to little endian
unsigned int swap(unsigned int size)
{
    char *ptr = (char *)&size;
        for (int i = 0; i < sizeof(int) / 2; i++)
        {
            char temp = *(ptr + i);
            *(ptr + i) = *(ptr + sizeof(int) - 1 - i);
            *(ptr + sizeof(int) - 1 - i) = temp;
        }
    return size;  //return size after converting
}

// function for return tag name
const char* get_tag_name(const char *tag_id) 
{
    if (strcmp(tag_id, "TIT2") == 0) return "Title";
    else if (strcmp(tag_id, "TPE1") == 0) return "Artist";
    else if (strcmp(tag_id, "TALB") == 0) return "Album";
    else if (strcmp(tag_id, "TYER") == 0) return "Year";
    else if (strcmp(tag_id, "TCON") == 0) return "Content";
    else if (strcmp(tag_id, "COMM") == 0) return "Comments";
    else return "Unknown";  // For any unrecognized tags
}
Status find_and_display_tag(FILE *fptr, const char *tag_id)
{
    rewind(fptr);  //set the pointer to the first index
    fseek(fptr,10L,SEEK_SET);  //skip first 10 bytes
    while(fptr!=NULL)
    {
        char tag[5]={0};
        

        int read_bytes=fread(tag,1,4,fptr);  //read the 4 bytes from the source file

        if(read_bytes!=4)
        {
            if (feof(fptr)) 
            {
                break;  
            }
            printf("ERROR: Failed to read tag ID.\n");
            return failure;

        }

        // if(tag[0]=='\0')
        // {
        //     printf("Empty tag encountered. Ending tag processing.\n");
        //     break;
        // }

        // if (feof(fptr))
        // {
        //     printf("end of file reached\n");
        // }

        if(strcmp(tag,tag_id)==0)
        {
            unsigned int data_size;   
            fread(&data_size,4,1,fptr);  //read the data size from source file

            data_size=swap(data_size);   //convert the big endian to little endian 
            data_size--;
            
            fseek(fptr,3,SEEK_CUR);  //skip the 3 bytes flags

            char *data=malloc(data_size);
            fread(data,data_size,1,fptr);  //read the data from the source file pointer
            data[data_size]='\0';   //

            const char *tag_name = get_tag_name(tag_id); //collect the tag title

            // printf("%s = %s\n",tag_name,data);  //print the function of tag 

            printf("%s = ",tag_name);

            
        for (int i = 0; i < data_size; i++) 
        {
            if (data[i] >= 32 && data[i] <= 126) 
            { // Printable ASCII range
                printf("%c", data[i]);
            }
        }
        free(data);

        printf("\n");
        return success;

               
            // printf("Title = %s",);

        }
        else
        {
           unsigned int skip_data;  
           fread(&skip_data,4,1,fptr);  //read the 

           skip_data=swap(skip_data);  //convert the big endian to little endian
           skip_data--;   //decrement the skip data 

           fseek(fptr,3,SEEK_CUR);  //skip the 3 flag data

           fseek(fptr,skip_data,SEEK_CUR);   //skip the data size 

        }

    }

    printf("ERROR: Tag '%s' not found.\n", tag_id);   //print error message if no tag is found
    return failure;
}


// to display the main tags 
void display_all_tags(FILE *fptr)
{
    const char *tags_to_find[] = {"TPE1","TIT2","TALB","TYER","TCON","COMM"};  //array of main tags

    unsigned int num_tags = sizeof(tags_to_find) / sizeof(tags_to_find[0]);   //to find the size for tags

    printf("\t\t");
    printf("-----------listing main tags---------\n\n");

    for(unsigned int i=0; i < num_tags; i++)
    {
        find_and_display_tag(fptr,tags_to_find[i]);
    }

}




     




    







