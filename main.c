//name : K Jayanth



#include "mp3.h"

#include<stdio.h>
int main(int argc,char *argv[])
{
    Mp3_tag mp3;  //creating structure variable

    

    if(argc<2)
    {
        printf("ERROR: incorrect way of command line argument.\n");
        printf("after compile please enter (./a.out --help) for help\n");
    }
    else
    {
        OperationType ret = Check_operation (argv);  //check operationtype

        if (ret==mp3_search)  //if mp3 operation is search
        {
            printf("---------------------\n");
            printf("listing all tags and data present in .mp3file.\n");
            printf("---------------------\n");
            Status ret1 = read_validate_mp3files_search(&mp3,argv);
            if(ret1==success)
            {
                // open_files(mp3.file_name,&mp3.fptr);
                list_all_tags(&mp3);        //list all tags
                // copy_remaining_data(&mp3);
            }
        }
        else if (ret == mp3_edit)   //if mp3 operation is edit
        {
            // printf("command line to edit a specific tag is -> ./a.out -e (-t (TIT2) ) file_name.mp3\n");
            
            Status ret1 = read_validate_mp3files_edit(&mp3,argv);  //read and validate the edit instruction

            edit_tag_data(&mp3);

        }
        else if (ret==mp3_view)  //if ret is mp3_view
        {
            Status ret1 = read_validate_mp3files_view(&mp3,argv);  //read and validate the view

            mp3.fptr=fopen(mp3.file_name,"r");

            display_all_tags(mp3.fptr);   //call the display function

        }
        else if(ret==mp3_help)  
        {
            printf("to view the specific tag information please enter the following command -> ./a.out -v file_name.mp3\n\n");

            printf("to list all the tags present in the song file please enter the following command -> ./a.out -s file_name.mp3\n\n");

            printf("to edit we the tag data please enter the following command -> ./a.out -e (tag modifier ex-: -t for TIT2)  new_data file_name.mp3\n\n");

            printf("edit tags modifiers\n");

            printf("TIT2 -> -t\n");
            printf("TOFN -> -to\n");
            printf("TPE1 -> -tp\n");
            printf("TOPE -> -top\n");
            printf("TLAB -> -ta\n");
            printf("TYER -> -y\n");
            printf("WCOP -> -w\n");
            printf("TPE2 -> -tp2\n");
            printf("TCON -> -tc\n");
            printf("TOWN -> -tow\n");
            printf("WXXX -> -wx\n");
            printf("TSSE -> -ts\n");
            printf("APIC -> -A\n");    

        }

    }

}