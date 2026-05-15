#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lzss.h"

uint32_t getle32(const uint8_t* p)
{
	return (p[0]<<0) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}

uint32_t lzss_get_decompressed_size(uint8_t* compressed, uint32_t compressedsize)
{
	uint8_t* footer = compressed + compressedsize - 8;

	//uint32_t buffertopandbottom = getle32(footer+0);
	uint32_t originalbottom = getle32(footer+4);

	return originalbottom + compressedsize;
}

int lzss_decompress(uint8_t* compressed, uint32_t compressedsize, uint8_t* decompressed, uint32_t decompressedsize)
{
	uint8_t* footer = compressed + compressedsize - 8;
	uint32_t buffertopandbottom = getle32(footer+0);
	//uint32_t originalbottom = getle32(footer+4);
	uint32_t i, j;
	uint32_t out = decompressedsize;
	uint32_t index = compressedsize - ((buffertopandbottom>>24)&0xFF);
	uint32_t segmentoffset;
	uint32_t segmentsize;
	uint8_t control;
	uint32_t stopindex = compressedsize - (buffertopandbottom&0xFFFFFF);

	memset(decompressed, 0, decompressedsize);
	memcpy(decompressed, compressed, compressedsize);

	
	while(index > stopindex)
	{
		control = compressed[--index];
		

		for(i=0; i<8; i++)
		{
			if (index <= stopindex)
				break;

			if (index <= 0)
				break;

			if (out <= 0)
				break;

			if (control & 0x80)
			{
				if (index < 2)
				{
					fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}

				index -= 2;

				segmentoffset = compressed[index] | (compressed[index+1]<<8);
				segmentsize = ((segmentoffset >> 12)&15)+3;
				segmentoffset &= 0x0FFF;
				segmentoffset += 2;

				
				if (out < segmentsize)
				{
					fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}

				for(j=0; j<segmentsize; j++)
				{
					uint8_t data;
					
					if (out+segmentoffset >= decompressedsize)
					{
						fprintf(stderr, "Error, compression out of bounds\n");
						goto clean;
					}

					data  = decompressed[out+segmentoffset];
					decompressed[--out] = data;
				}
			}
			else
			{
				if (out < 1)
				{
					fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}
				decompressed[--out] = compressed[--index];
			}

			control <<= 1;
		}
	}

	return 1;
clean:
	return 0;
}
