#include "lib.h"
#include "ncch_build.h"
#include "ncsd_build.h"
#include "cia_build.h"

int main(int argc, char *argv[])
{
	// Setting up user settings
	user_settings *set = calloc(1,sizeof(user_settings));
	if(set == NULL) {
		fprintf(stderr,"[!] Not enough memory\n"); 
		return -1;
	}	
	init_UserSettings(set);
	initRand();
	
	int result;

	// Parsing command args
	if((result = ParseArgs(argc,argv,set)) < 0) 
		goto finish;
	

	// Import RSF Settings if present
	if((result = GetRsfSettings(set)) < 0) 
		goto finish;

	// Setup Content 0
	if(!set->ncch.buildNcch0){ // Import Content
		if(set->common.workingFileType == infile_ncch){
			if(!AssertFile(set->common.contentPath[0])){
				fprintf(stderr,"[MAKEROM ERROR] Failed to open Content 0: %s\n",set->common.contentPath[0]); 
				goto finish;
			}
			u64 fileSize = GetFileSize64(set->common.contentPath[0]);
			u64 calcSize = 0;

			FILE *ncch0 = fopen(set->common.contentPath[0],"rb");
			
			ncch_hdr hdr;
			ReadNcchHdr(&hdr,ncch0);
			calcSize = GetNcchSize(&hdr);
			if(calcSize != fileSize){
				fprintf(stderr,"[MAKEROM ERROR] Content 0 is corrupt\n"); 
				fclose(ncch0);
				goto finish;
			}

			set->common.workingFile.size = fileSize;
			set->common.workingFile.buffer = malloc(fileSize);
			ReadFile64(set->common.workingFile.buffer, set->common.workingFile.size,0,ncch0);
			fclose(ncch0);
		}
		else{
			if(!AssertFile(set->common.workingFilePath)) {
				fprintf(stderr,"[MAKEROM ERROR] Failed to open: %s\n",set->common.workingFilePath); 
				goto finish;
			}
			u64 size = GetFileSize64(set->common.workingFilePath);
			set->common.workingFile.size = align(size,0x10);
			set->common.workingFile.buffer = malloc(set->common.workingFile.size);
			FILE *fp = fopen(set->common.workingFilePath,"rb");
			ReadFile64(set->common.workingFile.buffer,size,0,fp);
			fclose(fp);
		}
	}
	else{// Build Content 0
		result = build_NCCH(set);
		if(result < 0) { 
			//fprintf(stderr,"[ERROR] %s generation failed\n",set->build_ncch_type == CXI? "CXI" : "CFA"); 
			fprintf(stderr,"[RESULT] Failed to build NCCH (ret = %d)\n", result); 
			goto finish; 
		}	
	}
	// Make CCI
	if(set->common.outFormat == CCI){
		result = build_CCI(set);
		if(result < 0) { 
			fprintf(stderr,"[RESULT] Failed to build CCI (ret = %d)\n", result); 
			goto finish; 
		}
	}
	// Make CIA
	else if(set->common.outFormat == CIA){
		result = build_CIA(set);
		if(result < 0) { 
			fprintf(stderr,"[RESULT] Failed to build CIA (ret = %d)\n", result); 
			goto finish; 
		}
	}
	// No Container Raw CXI/CFA
	else if(set->common.outFormat == CXI || set->common.outFormat == CFA){
		FILE *ncch_out = fopen(set->common.outFileName,"wb");
		if(!ncch_out) {
			fprintf(stderr,"[MAKEROM ERROR] Failed to create '%s'\n",set->common.outFileName); 
			fprintf(stderr,"[RESULT] Failed to build '%s'\n",set->common.outFormat == CXI? "CXI" : "CFA"); 
			result = FAILED_TO_CREATE_OUTFILE; 
			goto finish;
		}
		WriteBuffer(set->common.workingFile.buffer,set->common.workingFile.size,0,ncch_out);
		fclose(ncch_out);
	}
	
finish:
	free_UserSettings(set);
	return result;
}