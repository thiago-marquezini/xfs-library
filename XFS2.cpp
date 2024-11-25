#include "export_header.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "xfs.h"

char *prog;
char *outputDir;
Bytef *buff;

Bytef *ReadFileBuff(char * file){
	FILE *fi = fopen(file,"rb");
	if(!fi) return 0;
	fseek(fi,0,SEEK_END);	
	long filesize = ftell(fi);
	fseek(fi,0,SEEK_SET);

	Bytef *buff = (Bytef *)malloc(filesize);
	fread(buff, 1, filesize, fi);
	fclose(fi);
	return buff;
}

int Unpack()
{
	xfs *myXfs = new xfs;
	if(myXfs->init(buff,outputDir)) myXfs->unpack();
	delete myXfs;
	return 1;
}

EXPORT int Decompress(char* filename, char* outputdirA, int unpackorinfo)
{
    char outmode[20] = "\0\0\0\0\0";
	
	// Output Dir
	outputDir = outputdirA;

	if(!(buff = ReadFileBuff(filename))) {
		MessageBox(0, "Error: Failed opening file", "Error", 0);
		ExitProcess(0);
	}
	ZlibEngine *c_stream = new ZlibEngine;

	if (unpackorinfo == 0) {
            xfs *myXfs = new xfs;
			if(myXfs->init(buff, outputDir)) { 
                
            // Directories //                 
            TCHAR NewDB[MAX_PATH];
            TCHAR PackDB[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, NewDB);
            GetCurrentDirectory(MAX_PATH, PackDB);
            strcat(NewDB, "\\Hunter\\FileSystemReport.hex");
            strcat(PackDB, "\\Hunter\\FileSystemReport.db");
            /////////////////
             /* Delete last DB */
                DeleteFile(PackDB);
            
             /* Write new database */
			FILE *fo = fopen(NewDB, "wb");
			fwrite(myXfs->xfsInfo, myXfs->xfsInfoLen, 1, fo);
			fclose(fo);
			 /* Compress database */
			c_stream->compress(NewDB, PackDB);
			//c_stream->decompress(PackDB, "Depack");
			 /* Delete original (uncompressed) database */
			DeleteFile(NewDB);
			 /* Kill XFS Instance */
			delete myXfs;	
            }
		 	
	}
	        // Depack all files into the folder
		    else 
             CreateThread(0,0,(LPTHREAD_START_ROUTINE)Unpack, NULL,0,NULL);
}

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        return TRUE;
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}
