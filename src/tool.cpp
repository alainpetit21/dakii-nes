/*
 * tool.cpp
 *
 * Rom loading & dispatching
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "libMacros.h"

#include "machdep.h"
#include "ui.h"
#include "video.h"
#include "tool.h"
#include "system.h"

unsigned long system_flags;

rom_file*
read_romimage(const char *filename)
{
rom_file	*retval	= new rom_file;
FILE		*file	= NULL;
int			numread;
//int			error;

	ASSERT2(retval, "Cannot Allocate rom_file Structure");
	retval->filename = NULL;
	retval->data = NULL;

	file = fopen(filename, "rb");

	ASSERT3(file, "Cannot Open %s", filename);

	fseek(file, 0, SEEK_END);

	retval->size = ftell(file);
	fseek(file, 0, SEEK_SET);

	retval->filename = (char*)filename;
	retval->data = new u8[retval->size];

	ASSERT3((retval->data && retval->filename), "Trouble Processing Openning of %s", filename);

	numread = fread(retval->data, 1, retval->size, file);
	ASSERT3(numread == retval->size, "Trouble Reading %s", filename);

    if(file)
		fclose(file);

	return retval;
}

void
free_romfile(rom_file* romfile)
{
	if(!romfile)
	{
		printf("free_romfile(): warning: NULL romfile.\n");
		return;
	}

    if(romfile->filename)
		delete [] romfile->filename;

	if(romfile->data)
		delete [] romfile->data;

	delete romfile;
}

char*
get_battery_filename(const char *filename)
{
char *retval;
int length;
int last_period= 0;

	for (length = 0; filename[length]; length++)
		if (filename[length] == '.')
			last_period = length;


	if(last_period == 0)
		last_period = length;

	retval = new char[last_period + 5];

	ASSERT2(retval, "cannot allocate memmory");

	strncpy(retval, filename, last_period);

	retval[last_period++] = '.';
	retval[last_period++] = 's';
	retval[last_period++] = 'a';
	retval[last_period++] = 'v';
	retval[last_period++] = '\0';

	return retval;
}

rom_file*
init_battery_file(rom_file* romfile, int size)
{
FILE 		*file	= NULL;
rom_file	*retval	= new rom_file;

	ASSERT2(retval, "cannot allocate memmory");

	retval->filename= get_battery_filename(romfile->filename);
	retval->data	= new u8[size];
	retval->size	= size;

	/* FIXME: retval->data and retval->filename can both be NULL right now */
	file = fopen(retval->filename, "rb");

	/* No file? just return an empty buffer */
	if (!file)
		return retval;

	/* FIXME: error handling? */
	fread(retval->data, 1, retval->size, file);

	fclose(file);

	return retval;
}

void
save_battery_file(rom_file* batteryfile)
{
FILE *batfile= fopen(batteryfile->filename, "wb");

	ASSERT2(batfile, "Cannot open battery file");

	fwrite(batteryfile->data, batteryfile->size, 1, batfile);
	fclose(batfile);
}

typedef struct tagSystemDetect
{
	int type;
	char *file_suffix;
}SystemDetect;

SystemDetect system_detect[] =
{
	{ST_NES,       ".nes"},
	{ST_MASTERSYS, ".sms"},
	{ST_GAMEGEAR,  ".gg"},
	{ST_COLECO,    ".col"},
	{ST_GENESIS,   ".smd"},
	{ST_GENESIS,   ".bin"},
	{ST_PCENGINE,  ".pce"},
	{ST_SG1000,    ".sg"},
	{ST_SC3000,    ".sc"},
	{ST_NONE, NULL}, /* Must be last entry */
};

int
guess_system(rom_file* romfile)
{
int len_filename= strlen(romfile->filename);
int len_suffix;
int string_offset;
int i;

	for(i= 0; system_detect[i].type != ST_NONE; ++i)
	{
		len_suffix		= strlen(system_detect[i].file_suffix);
		string_offset	= len_filename - len_suffix;

		if(string_offset < 0)
			continue;
		if (!strcmp(romfile->filename + string_offset, system_detect[i].file_suffix))
			return system_detect[i].type;
	}

	return ST_NONE;
}

typedef struct tagSystems{
	int type;
	void (*activate)(rom_file* romfile);
}Systems;

Systems systems[] = {
	{ST_NES,       nes_run},
	{ST_PCENGINE,  pce_run},
	{ST_MASTERSYS, sms_run},
	{ST_GAMEGEAR,  gg_run},
	{ST_COLECO,    cv_run},
	{ST_SG1000,    sg1k_run},
	{ST_SC3000,    sc3k_run},
	{ST_MSX,       msx_run},
	{ST_GENESIS,   genesis_run},
	{ST_NONE,      NULL}, /* Nust be last entry */
};

void
activate_system(int system_type, rom_file* romfile)
{
int i;

	for(i= 0; systems[i].type != ST_NONE; i++)
	{
		if (systems[i].type == system_type)
		{
			systems[i].activate(romfile);
			return;
		}
	}

	printf("unknown system type.\n");
}
