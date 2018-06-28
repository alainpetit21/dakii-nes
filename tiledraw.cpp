/*
 * tiledraw.c
 *
 * drawing routines for tilecached VDPs
 */

#include "libTypes.h"

#include "tiledraw.h"

void tiledraw_8(u8 *cur_vbp, u8 **tiles, u8 **palettes, int num_tiles, int finescroll)
{
int i;
u8 *palette= *palettes++;
u8 *tiledata= *tiles++;

	if(!tiledata)
		cur_vbp+= (8 - finescroll);
	else
	{
		tiledata+= finescroll;
		
		switch(finescroll)
		{
		case 0:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 1:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 2:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 3:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 4:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 5:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 6:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		case 7:
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		}
	}
    
	for(i = num_tiles; i > 1; i--)
	{
		palette = *palettes++;
		tiledata = *tiles++;
		
		if (!tiledata)
			cur_vbp += 8;
		else
		{
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
			
			if (*tiledata)
				*cur_vbp = palette[*tiledata];
				
			cur_vbp++;
			tiledata++;
		}
	}
    
    if(finescroll)
	{
		palette = *palettes++;
		tiledata = *tiles++;
		
		if(tiledata)
		{
			switch (finescroll)
			{
			case 7:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 6:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 5:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 4:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 3:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 2:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			case 1:
				if (*tiledata)
					*cur_vbp = palette[*tiledata];
					
				cur_vbp++;
				tiledata++;
			}
		}
    }
}
