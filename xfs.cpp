#include "stdio.h"
#include "xfs.h"
#include <iostream>

char *FileToSearch;
int ExtractAllFiles;
HWND WindowExtractor;
int TotalExtracted = 0;

#define WM_SHOWPROGRESS WM_USER + 1

xfs::xfs(void)
{
}

xfs::~xfs(void)
{
}

int xfs::decompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
	ZlibEngine *c_stream = new ZlibEngine;

	uLongf total_Len = *destLen;
	uLongf currentSize = 0;

	while(currentSize < total_Len){
		*destLen = total_Len;
		sourceLen = (*(uLongf *)source) & 0xFFFFFF;
		source += 5;
		int error = c_stream->decompress(dest + currentSize, destLen, source, sourceLen);
		if (error != Z_OK) {
			*destLen = currentSize;
			break;
		}
		source		+= sourceLen;
		currentSize += *destLen;
	}

	return 1;
}

int xfs::unpack(void)
{
	FileInfoBlock *myFile = (FileInfoBlock*)this->xfsInfo;
	
	for (int i=0; i < this->xfsInfoLen / FILE_INFO_LEN; i++)
        {  
                                  
		int size = strlen(this->OutputDir) + strlen(myFile->header);
		
		// Liberar espaço suficiente na memoria (FIX THEMARKS)
		char *fileToCreate = (char *)malloc(size+sizeof(size));
             
		strcpy(fileToCreate, this->OutputDir);
		strcat(fileToCreate, "\\");
		strcat(fileToCreate, myFile->header);

		Bytef *pCurrFile	= buff + myFile->position;
		uLongf outputSize	= myFile->unpackedSize;
		uLongf inputSize	= myFile->packedSize;
		uLongf initSize		= (*(uLongf *)pCurrFile) & 0xFFFF;
		if (outputSize == initSize) pCurrFile += 3;
		
		//if(*(pCurrFile+5) == 0x78) pCurrFile += 0x05;
		//else pCurrFile +=0x08;
		
		Bytef *out = (Bytef *)malloc(outputSize);
		
		// VERIFICAR SE É PRA EXTARIR TUDO OU APENAS 1 ARQUIVO DEFINIDO
		if (ExtractAllFiles != 1)
           {
                            
                            
        // Dividir o argumento que possui as extenções dos arquivos a serem extraídos
        const std::string Arguments = FileToSearch;
        Splitter split (Arguments, "-");
        for (Splitter::size_type i = 0; i < split.size(); i++ )
        {
        
        std::string CurrentFile = split[i];
        char* ArgumentName = (char*)malloc(sizeof(char) *(CurrentFile.length() +1) );
        strcpy(ArgumentName, CurrentFile.c_str());
        
        // EXTRAIR ARQUIVO DEFINIDO               
		if (strstr(fileToCreate, ArgumentName))
        {
        
        // Add 1 na contagem
        TotalExtracted++;
                                 
		char displayFileName[300] = "";
		strcat(displayFileName, myFile->header);
		strcat(displayFileName, "\n");
		printf(displayFileName);
                                 
		int error = this->decompress(out, &outputSize, pCurrFile, inputSize);
		FILE *fo = fopen(fileToCreate,"wb");
		fwrite(out, outputSize, 1, fo);
		
		Sleep(1);
		fclose(fo);
		
        } // INF END
         } // FOR END
        
        // EXTRAIR TUDO
        } else 
               {
                   
                   // EXTRAI TUDO
                         char displayFileName[300] = "";
		                 strcat(displayFileName, myFile->header);
		                 strcat(displayFileName, "\n");
		                 printf(displayFileName);
                                 
		                 int error = this->decompress(out, &outputSize, pCurrFile, inputSize);
		                 FILE *fo = fopen(fileToCreate,"wb");
		                 fwrite(out, outputSize, 1, fo);
		
		                 Sleep(1);
		                 fclose(fo);
               }

		myFile++;
}
    // Manda mostrar o progress
	SendMessage(WindowExtractor, WM_SHOWPROGRESS, 0, TotalExtracted);
	return 1;
}

int xfs::unpackInfo(){
	return 1;
}

int xfs::init(Bytef *fbuff, char *outputDir){
    
        // DEVIDIR A STRING DO OUTPUT
        // copiar o output passado pra usar na extração
        const std::string line = outputDir;
        Splitter split (line, ",");
        //for ( Splitter::size_type i = 0; i < split.size(); i++ )
        //std::cout << split[i] << "\n";
        
        // PEGAR O DIRETORIO A SER CRIADO //
        std::string Dir = split[0];
        char* pch = (char*)malloc(sizeof(char) *(Dir.length() +1) );
        strcpy(pch, Dir.c_str());
        
        // PEGAR NOME DO ARQUIVO A SER EXTRAIDO
        std::string SFile = split[1];
        FileToSearch = (char*)malloc(sizeof(char) *(SFile.length() +1));
        strcpy(FileToSearch, SFile.c_str());
        
        if (strstr(FileToSearch, "ALL"))
            {
             ExtractAllFiles = 1;
            }
        
        
	this->OutputDir = pch;
	int err = CreateDirectoryA(this->OutputDir, NULL);

	this->buff = fbuff;
	this->pPacketTail	= buff + *((long *)fbuff);
	this->signBlockLen	= *this->pPacketTail;

	this->pPacketInfo	= this->pPacketTail + this->signBlockLen + 1;
	this->xfsInfoLen	= (*(long *)pPacketInfo) & 0x00FFFFFF;
	this->pPacketInfo	+= 3;

	
    ZlibEngine *c_stream = new ZlibEngine;
	unsigned long outLength = this->xfsInfoLen * 100;
	this->xfsInfo = (Bytef *)malloc(outLength);
	c_stream->decompress(this->xfsInfo,&outLength,this->pPacketInfo,this->xfsInfoLen);
	this->xfsInfoLen = outLength;
	
	// Pegar janela do extractor
	WindowExtractor = FindWindow(0, "XFS Extractor - coded by themarks");
	// Mostra a progress
	SendMessage(WindowExtractor, WM_SHOWPROGRESS, 1, 1);
	
	return INIT_OK;
}
