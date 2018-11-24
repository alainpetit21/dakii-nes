#ifndef TOOL_H
#define TOOL_H

extern unsigned long system_flags;

#define F_NONE    0x00000000
#define F_UNIMPL  0x00000001
#define F_BREAK   0x00000002
#define F_QUIT    0x00000004
#define F_GUI     0x00000008

typedef struct tag_rom_file
{
	char *filename;
	unsigned char *data;
	int size;
}rom_file;

rom_file*	read_romimage(const char *filename);
void 		free_romfile(rom_file* romfile);

void 		activate_system(int system_type, rom_file* romfile);
int 		guess_system(rom_file* romfile);

rom_file* 	init_battery_file(rom_file* romfile, int size);
void 		save_battery_file(rom_file* batteryfile);

#endif /* TOOL_H */
