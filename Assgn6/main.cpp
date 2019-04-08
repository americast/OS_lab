#include <bits/stdc++.h>
#include <math.h>
using namespace std;

// typedef block my_file;

// struct FAT
// {
// 	char filename[100];
// 	block* ptr;
// };

struct FDT
{
	int index;
	int seek;
	int use;
};

FDT* fdt;

struct super_block_constants
{
	int sys_size, num_blocks, block_size, num_files;
};

struct vol_info
{
	char filename[100];
	int index;
};

void* *blocks;
super_block_constants *sbc;
int* used;
int* seek;	// Needed for file reading
vol_info* filename_map;
int *fat;
char* other_blocks;

int add_to_fdt(int index)
{
	for (int i = 0; i < sbc->num_blocks; i++)
	{
		// cout<<"i "<<i<<endl;
		// cout<<"use "<<fdt[i].use<<endl;
		if (fdt[i].use == 0)
		{
			fdt[i].use = 1;
			fdt[i].seek = 0;
			fdt[i].index = index;
			return i;
		}
	}
}

int index_from_fdt(int fdt_index)
{
	if (fdt[fdt_index].use)
		return fdt[fdt_index].index;
	else
	{
		fprintf(stderr, "Virtual segfault\n");
		exit(EXIT_FAILURE);
	}
}

int my_open(char *file_name)
{
	int tot_block_size = sbc->block_size * pow(2, 10);

	int num_files = sbc->num_files;
	int i;
	for (i = 0; i < num_files; i++)
		if (strcmp(filename_map[i].filename, file_name) == 0)
			return add_to_fdt(filename_map[i].index);
	// cout<<"Not found!"<<endl;

	int num_blocks = sbc->num_blocks;
	int found = 0, pos;
	// cout<<"num_blocks: "<<num_blocks<<endl;
	for (int i = 0; i < num_blocks; i++)
	{
		if (used[i] == 0)
		{
			found = 1;
			pos = i;
			break;
		}
	}
	if (found)
	{
		int i = pos;
		used[i] = 1;
		strcpy(filename_map[num_files].filename, file_name);
		filename_map[num_files].index = i;
		sbc->num_files++;
		return add_to_fdt(i);
	}
	else
	{
		fprintf(stderr, "Disk is full\n");
		return -1;
	}
}

int my_erase(int file)
{
	if (used[file] == 0)
		return -1;
	int file_org = file;
	while(1)
	{
		strcpy(other_blocks + (file * sbc->block_size), "\0");
		// memcpy(other_blocks + (file * sbc->block_size), 0, 1);
		// file->len = 0;
		if (fat[file] == -1)
			break;
		file = fat[file];
		used[file] = 0;
	}
	fat[file_org] = -1;
	// memcpy(other_blocks + file_org*
	return 1;
}

int my_write(int file, char *text, int length, char mode)
{
	file = index_from_fdt(file);
	if (mode == 'w')
		my_erase(file);
	else
	{
		while(1)
			if (fat[file] == -1)
				break;
			else
				file = fat[file];

	}
	int block_size = sbc->block_size;
	int count = 0;
	do
	{
		int len_here, end_flag = 0;
		if (length > block_size - 1 - strlen(other_blocks + sbc->block_size * file))
			len_here = block_size - 1 - strlen(other_blocks + sbc->block_size * file);
		else
		{
			len_here = length;
			end_flag = 1;
		}
		char *write_here;
		if (strlen(other_blocks + sbc->block_size * file) == 0)
			write_here = other_blocks + sbc->block_size * file;
		else
			write_here = (other_blocks + sbc->block_size * file + strlen(other_blocks + sbc->block_size * file));
		memcpy(write_here, text + count, len_here);
		write_here[len_here] = '\0';
		count+=len_here;
		length-=len_here;
		// file->len = file->len + len_here;
		if (end_flag)
		{
			fat[file] = -1;
			break;
		}
		else
		{
			int i, found_flag = 0;
			for (i = 0; i < sbc->num_blocks; i++)
			{
				if (used[i] == 0)
				{
					found_flag = 1;
					break;
				}
			}
			if (found_flag)
			{
				fat[file] = i;
				used[i] = 1;
				file = i;
			}
			else
			{
				fprintf(stderr, "Out of disk space\n");
				return -1;
			}
		}
	}while (length > 0);
	return length;
}

int my_close(int file)
{
	if (fdt[file].use)
	{
		fdt[file].use = 0;
		return 1;
	}
	else
	{
		fprintf(stderr, "File already closed\n");
		return -1;
	}
}


int my_cat(char *str)
{
	int num_files = sbc->num_files;
	int i, found = 0;
	for (i = 0; i < num_files; i++)
	{
		// cout<<"Name of file is: "<<((FAT *) blocks[1])[i].filename<<endl;
		if (strcmp(filename_map[i].filename, str) == 0)
		{
			// cout<<"Found :D\n";
			found = 1;
			break;
		}
	}

	if (!found)
	{
		fprintf(stderr, "File not found\n");
		return -1;
	}

	// cout<<"and i is: "<<i<<endl;
	// block *here = ((FAT *) blocks[1])[i].ptr;
	int index = filename_map[i].index;
	while(1)
	{
		// int len = here->len;
		// cout<<"len is "<<len<<endl;
		char *now = other_blocks + sbc->block_size * index;
		// for (int j = 0; j < len; j++)
		cout<<now;
		if (fat[index] != -1)
			index = fat[index];
		else
		{
			cout<<endl;
			return 0;
		}
	}
}

void my_read(char *text, int file, int len)
{
	// int num_files = sbc->num_files;
	int fdt_file = file;
	file = index_from_fdt(file);
	int i = 0;
	int here = file;
	int seek_here = fdt[fdt_file].seek;
	// cout<<"Seek here "<<seek_here<<endl;
	// cout<<"File here "<<file<<endl;
	// here = 5;
	while(1)
	{
		// int len = here->len;
		// cout<<"len is "<<len<<endl;
		char *now = other_blocks + (here * sbc->block_size);
		if (seek_here < (sbc->block_size))
		{
			now+=seek_here;
			for (int j = 0; j < strlen(now); j++)
			{
				text[i++] = now[j];
				fdt[fdt_file].seek++;
				if (i >= len)
					return;
			}	
		}
		else
			seek_here -= sbc->block_size;
		if (fat[here] != -1)
			here = fat[here];
		else
		{
			text[i] = '\0';
			break;
		}
	}
}

int my_copy(char *system_file, char *file_here)
{
	int file = my_open(file_here);
	FILE *s_file;
	s_file = fopen(system_file,"rb");
	fseek(s_file,0,SEEK_END);
	int size = ftell(s_file);
	// cout<<"size is: "<<size<<endl;
	char txt_here[size];
	fseek(s_file, 0, SEEK_SET);
	fread(txt_here, size, 1, s_file);
	txt_here[size - 1] = '\0';
	// cout<<"txt here is: "<<txt_here<<"\nDone."<<endl;
	fclose(s_file);
	int n = my_write(file, txt_here, size, 'w');
	if (n >= 0)
		return file;
	else
		return -1;
}

int main()
{
	int sys_size, block_size;
	cout<<"Enter size of file system in MB: ";
	cin>>sys_size;
	cout<<"\nEnter size of one block in KB: ";
	cin>>block_size;
	int num_blocks = ((sys_size * pow(2, 20)) - (sizeof(super_block_constants)) ) / ((block_size * pow(2, 10)) + (3 * sizeof(int) + sizeof(vol_info)));
	cout<<"Num blocks in main: "<<num_blocks<<endl;
	blocks = (void **) malloc(sys_size * pow(2, 20));

	// super_block *sb;
	// = (super_block *) malloc(sizeof(super_block));
	sbc = (super_block_constants*) blocks;
	// blocks[0] = sb;

	// sb->fat = 
	// sb->used = (int *) malloc(sizeof(int) * (num_blocks));
	fdt = (FDT*) malloc(num_blocks * sizeof(FDT));
	for (int i = 0; i < num_blocks; i++)
		fdt[i].use = 0;

	sbc->sys_size = sys_size * pow(2, 20);
	sbc->block_size = block_size * pow(2, 10);
	sbc->num_blocks = num_blocks;
	sbc->num_files = 0;

	used = (int *) (blocks + sizeof(sbc));

	for (int i = 0; i < num_blocks; i++)
		used[i] = 0;

	seek = (int *) (used + num_blocks);

	for (int i = 0; i < num_blocks; i++)
		seek[i] = 0;

	filename_map = (vol_info *) (seek + num_blocks);
	for (int i = 0; i < num_blocks; i++)
	{
		strcpy(filename_map[i].filename, "");
		filename_map[i].index = -1;
	}

	fat = (int*) (filename_map + num_blocks);

	for (int i = 0; i < num_blocks; i++)
		fat[i] = -1;

	other_blocks = (char *) (blocks + (sizeof(sbc) + (3 * sizeof(int) + sizeof(vol_info)) * (num_blocks)) / sizeof(void *));
	// cout<<"Offset: "<<sizeof(sbc) + (3 * sizeof(int) + sizeof(vol_info)) * (num_blocks - 2)<<endl;


	for (int i = 0; i < num_blocks ; i++)
	{
		// printf("%d\n%d\n", blocks, other_blocks);
		// cout<<i<<endl;
		// printf("Pos now: %d\n", other_blocks + (i * sbc->block_size));
		strcpy(other_blocks + (i * sbc->block_size), "\0");
	}



	// API testing

	int file = my_open("hello");
	// cout<<"file is "<<file<<endl;
	my_write(file, "uerhfuerhfuihrfuhrukfhkfhskhfkshfksdhfkdshkdjcdjkckdcjkdbckddbc", 61, 'w');
	my_cat("hello");
	my_write(file, "hello", 5, 'w');
	my_cat("hello");
	my_write(file, " ja gelo", 8, 'a');
	my_cat("hello");
	int file2 = my_copy("test", "test2");
	// cout<<"file2 is "<<file2<<endl;
	int file3 = my_open("test2");
	// cout<<"file3 is "<<file3<<endl;
	// cout<<"file file2 file3 "<<file<<" "<<file2<<" "<<file3<<endl;
	my_cat("test2");
	char txt_here[100];
	my_read(txt_here, file3, 7);
	cout<<"Text is: "<<txt_here<<endl;
	my_read(txt_here, file3, 3);
	cout<<"Text is: "<<txt_here<<endl;
	my_close(file2);
	my_read(txt_here, file2, 7);
	cout<<"Done."<<endl;
}