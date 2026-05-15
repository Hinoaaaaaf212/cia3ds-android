#include "lib.h"
#include <mbedtls/base64.h>

#define IO_BLOCKSIZE 5*MB

// Memory
int CopyData(u8 **dest, const u8 *source, u64 size)
{
	if(!*dest){
		*dest = malloc(size);
		if(!*dest) return -1;
	}
	memcpy(*dest,source,size);
	return 0;
}

void rndset(void *ptr, u64 num)
{
	u8 *tmp = (u8*)ptr;
	for(u64 i = 0; i < num ; i++)
		tmp[i] = u8GetRand();
}

void clrmem(void *ptr, u64 num)
{
	memset(ptr,0,num);
}

// Misc
u64 roundup(u64 value, u64 alignment)
{
	return value + alignment - value % alignment;
}

u64 align(u64 value, u64 alignment)
{
	if(value % alignment != 0)
		return roundup(value,alignment);
	else
		return value;
}

u64 min64(u64 a, u64 b)
{
	if(a < b) return a;
	return b;
}

u64 max64(u64 a, u64 b)
{
	if(a > b) return a;
	return b;
}

// Strings
char* replace_filextention(const char *input, const char *new_ext)
{
	if(input == NULL || new_ext == NULL)
		return NULL;

	char *new_name;
	char *ext = strrchr(input, '.');

	// If there is no existing extention, just append new_ext
	if (!ext) {
		new_name = calloc(strlen(input) + strlen(new_ext), 1);
		sprintf(new_name, "%s%s", input, new_ext);
	}
	else {
		u32 size = ext - input;
		new_name = calloc(size + strlen(new_ext) + 1, 1);
		strncpy(new_name, input, size);
		sprintf(new_name, "%s%s", new_name, new_ext);
	}
	
	return new_name;
}

void memdump(FILE* fout, const char* prefix, const u8* data, u32 size)
{
	u32 i;
	u32 prefixlen = strlen(prefix);
	u32 offs = 0;
	u32 line = 0;
	while(size)
	{
		u32 max = 32;

		if (max > size)
			max = size;

		if (line==0)
			fprintf(fout, "%s", prefix);
		else
			fprintf(fout, "%*s", prefixlen, "");


		for(i=0; i<max; i++)
			fprintf(fout, "%02X", data[offs+i]);
		fprintf(fout, "\n");
		line++;
		size -= max;
		offs += max;
	}
}

// Base64
bool IsValidB64Char(char chr)
{
	return (isalnum(chr) || chr == '+' || chr == '/' || chr == '=');
}

size_t b64_strlen(const char *str)
{
	size_t count = 0;
	size_t i = 0;
	while(str[i] != 0x0){
		if(IsValidB64Char(str[i])) {
			//printf("Is Valid: %c\n",str[i]);
			count++;
		}
		i++;
	}

	return count;
}

void b64_strcpy(char *dst, const char *src)
{
	size_t src_len = strlen(src);
	size_t j = 0;
	for(size_t i = 0; i < src_len; i++){
		if(IsValidB64Char(src[i])){
			dst[j] = src[i];
			j++;
		}
	}
	dst[j] = 0;

	//memdump(stdout,"src: ",(u8*)src,src_len+1);
	//memdump(stdout,"dst: ",(u8*)dst,j+1);
}

int b64_decode(u8 *dst, const char *src, size_t dst_size)
{
	int ret;
	size_t size = dst_size;
	
	ret = mbedtls_base64_decode(dst, size, &size, (const u8*)src, strlen(src));
	
	if(size != dst_size)
		ret = MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
	
	return ret;
}

// Pseudo-Random Number Generator
void initRand(void)
{
	srand(time(0));
}

u8 u8GetRand(void)
{
	return rand() % 0xff;
}

u16 u16GetRand(void)
{
	return rand() % 0xffff;
}

u32 u32GetRand(void)
{
	return  (u32)u16GetRand() | (u32)u16GetRand() << 16;
}

u64 u64GetRand(void)
{	
	return (u64)u32GetRand() | (u64)u32GetRand() << 32;
}

//Char IO
bool AssertFile(char *filename)
{
	if(filename == NULL)
		return false;
#ifdef _WIN32
	struct _stat64 st;
	return _stat64(filename, &st) == 0;
#else
	struct stat st;
	return stat(filename, &st) == 0;
#endif
}

u64 GetFileSize64(char *filename)
{
#ifdef _WIN32
	struct _stat64 st;
	if( _stat64(filename, &st) != 0)
		return 0;
	else
		return st.st_size;
#else
	struct stat st;
	if( stat(filename, &st) != 0)
		return 0;
	else
		return st.st_size;
#endif
}

int makedir(const char* dir)
{
#ifdef _WIN32
	return _mkdir(dir);
#else
	return mkdir(dir, 0777);
#endif
}

char *getcwdir(char *buffer,int maxlen)
{
#ifdef _WIN32
	return _getcwd(buffer,maxlen);
#else
	return getcwd(buffer,maxlen);
#endif
}

int TruncateFile64(char *filename, u64 filelen)
{
#ifdef _WIN32
	HANDLE fh;
 
	LARGE_INTEGER fp;
	fp.QuadPart = filelen;
 
	fh = CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		printf("[!] Invalid File handle\n");
		return 1;
	}
 
	if (SetFilePointerEx(fh, fp, NULL, FILE_BEGIN) == 0 || SetEndOfFile(fh) == 0) {
		printf("[!] Truncate failed\n");
		CloseHandle(fh);
		return 1;
	}
 
	CloseHandle(fh);
	return 0;
#else
	return truncate(filename,filelen);
#endif	
}


//IO Misc
u8* ImportFile(char *file, u64 size)
{
	u64 fsize = GetFileSize64(file);
	if(size > 0 && size != fsize){
		fprintf(stderr,"[!] %s has an invalid size (0x%"PRIx64")\n",file, fsize);
		return NULL;
	}

	u8 *data = (u8*)calloc(1,fsize);
	if(!data){
		fprintf(stderr,"[!] Not enough memory\n");
			return NULL;
	}
	FILE *fp = fopen(file,"rb");
	ReadFile64(data, fsize, 0, fp);
	fclose(fp);

	return data;
}

void WriteBuffer(const void *buffer, u64 size, u64 offset, FILE *fp)
{
	const u8* _buffer = (const u8*)buffer;
	fseek_64(fp,offset);
	for (; size > IO_BLOCKSIZE; size -= IO_BLOCKSIZE, _buffer += IO_BLOCKSIZE)
		fwrite(_buffer, IO_BLOCKSIZE, 1, fp);
	fwrite(_buffer,size,1,fp);
} 

void ReadFile64(void *outbuff, u64 size, u64 offset, FILE *fp)
{
	u8* _buffer = (u8*)outbuff;
	fseek_64(fp, offset);
	for (; size > IO_BLOCKSIZE; size -= IO_BLOCKSIZE, _buffer += IO_BLOCKSIZE)
		fread(_buffer, IO_BLOCKSIZE, 1, fp);
	fread(_buffer, size, 1, fp);
}

int fseek_64(FILE *fp, u64 file_pos)
{
#ifdef _WIN32
	fpos_t pos = file_pos;
	return fsetpos(fp,&pos);
#else
	return fseeko(fp,file_pos,SEEK_SET);
#endif
}

//Data Size conversion
u16 u8_to_u16(const u8 *value, u8 endianness)
{
	u16 new_value = 0;
	switch(endianness){
		case(BE): new_value =  (value[1]<<0) | (value[0]<<8); break;
		case(LE): new_value = (value[0]<<0) | (value[1]<<8); break;
	}
	return new_value;
}

u32 u8_to_u32(const u8 *value, u8 endianness)
{
	u32 new_value = 0;
	switch(endianness){
		case(BE): new_value = (value[3]<<0) | (value[2]<<8) | (value[1]<<16) | (value[0]<<24); break;
		case(LE): new_value = (value[0]<<0) | (value[1]<<8) | (value[2]<<16) | (value[3]<<24); break;
	}
	return new_value;
}


u64 u8_to_u64(const u8 *value, u8 endianness)
{
	u64 ret = 0;
	switch(endianness){
		case(BE): 
			ret |= (u64)value[7]<<0;
			ret |= (u64)value[6]<<8;
			ret |= (u64)value[5]<<16;
			ret |= (u64)value[4]<<24;
			ret |= (u64)value[3]<<32;
			ret |= (u64)value[2]<<40;
			ret |= (u64)value[1]<<48;
			ret |= (u64)value[0]<<56;
			break;
			//return (value[7]<<0) | (value[6]<<8) | (value[5]<<16) | (value[4]<<24) | (value[3]<<32) | (value[2]<<40) | (value[1]<<48) | (value[0]<<56);
		case(LE): 
			ret |= (u64)value[0]<<0;
			ret |= (u64)value[1]<<8;
			ret |= (u64)value[2]<<16;
			ret |= (u64)value[3]<<24;
			ret |= (u64)value[4]<<32;
			ret |= (u64)value[5]<<40;
			ret |= (u64)value[6]<<48;
			ret |= (u64)value[7]<<56;
			break;
			//return (value[0]<<0) | (value[1]<<8) | (value[2]<<16) | (value[3]<<24) | (value[4]<<32) | (value[5]<<40) | (value[6]<<48) | (value[7]<<56);
	}
	return ret;
}

int u16_to_u8(u8 *out_value, u16 in_value, u8 endianness)
{
	switch(endianness){
		case(BE):
			out_value[0]=(in_value >> 8);
			out_value[1]=(in_value >> 0);
			break;
		case(LE):
			out_value[0]=(in_value >> 0);
			out_value[1]=(in_value >> 8);
			break;
	}
	return 0;
}

int u32_to_u8(u8 *out_value, u32 in_value, u8 endianness)
{
	switch(endianness){
		case(BE):
			out_value[0]=(in_value >> 24);
			out_value[1]=(in_value >> 16);
			out_value[2]=(in_value >> 8);
			out_value[3]=(in_value >> 0);
			break;
		case(LE):
			out_value[0]=(in_value >> 0);
			out_value[1]=(in_value >> 8);
			out_value[2]=(in_value >> 16);
			out_value[3]=(in_value >> 24);
			break;
	}
	return 0;
}

int u64_to_u8(u8 *out_value, u64 in_value, u8 endianness)
{
	switch(endianness){
		case(BE):
			out_value[0]=(in_value >> 56);
			out_value[1]=(in_value >> 48);
			out_value[2]=(in_value >> 40);
			out_value[3]=(in_value >> 32);
			out_value[4]=(in_value >> 24);
			out_value[5]=(in_value >> 16);
			out_value[6]=(in_value >> 8);
			out_value[7]=(in_value >> 0);
			break;
		case(LE):
			out_value[0]=(in_value >> 0);
			out_value[1]=(in_value >> 8);
			out_value[2]=(in_value >> 16);
			out_value[3]=(in_value >> 24);
			out_value[4]=(in_value >> 32);
			out_value[5]=(in_value >> 40);
			out_value[6]=(in_value >> 48);
			out_value[7]=(in_value >> 56);
			break;
	}
	return 0;
}


