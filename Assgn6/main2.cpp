/***************************************************

                    ASSIGNMENT 6 - 2

    Group No.: 42

    Members:    Swastika Dutta  (16CS10060)
                Sayan Sinha     (16CS10048)

****************************************************/


#include <bits/stdc++.h>
#include <math.h>
using namespace std;

struct FDT
{
	int index;
	int seek;
	int w_seek;
	int use;
};

FDT* fdt;		// Pointer to FDT which is in RAM

struct super_block_constants
{
	char vol_name[100];
	int vol_size, block_size, num_blocks;
	int free_ptr;
};

struct free_block
{
	int next_ptr;
};

struct inode
{
	int valid;	// -1 or 1
	int type;	// 1 means file, 2 means dir
	int size;
	int directly[5];
	int singly;
	int doubly;
};

struct vol_info
{
	char filename[100];
	int index;
};

struct dir_entry
{
	char filename[14];
	short i_no;
};

super_block_constants *sbc;
void* blocks;
vol_info* filename_map;
inode* inodes;

int curr_dir;

int add_to_fdt(int index)
{
	for (int i = 0; i < sbc->num_blocks; i++)
	{
		if (fdt[i].use == 0)
		{
			fdt[i].use = 1;
			fdt[i].seek = 0;
			fdt[i].w_seek = 0;
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

int my_open(char *file_name)		// To reopen a file or create a new one
{
	int block_size = sbc->block_size;

	int i;
	dir_entry* dir_here = (dir_entry*) (blocks + curr_dir * block_size);
	for (i = 0; i < block_size / 16; i++)
	{
		if (strcmp(dir_here[i].filename, file_name) == 0 && dir_here[i].i_no != -1)
			return add_to_fdt(dir_here[i].i_no);
	}

	int num_blocks = sbc->num_blocks;
	int pos = sbc->free_ptr;
	if (pos)
	{
		void* block_here = (blocks + (pos * sbc->block_size));
		sbc->free_ptr = ((free_block*) (blocks + (pos * sbc->block_size)))->next_ptr;

		int inode_pos, dir_pos;
		for (int i = 0; i < sbc->num_blocks; i++)
		{
			if (inodes[i].valid == -1)
			{
				inodes[i].valid = 1;
				inode_pos = i;
				break;
			}
		}

		int dir_found_flag = 0;

		for (int i = 0; i < block_size / 16; i++)
		{
			if (dir_here[i].i_no == -1)
			{
				dir_pos = i;
				dir_found_flag = 1;
				break;
			}
		}

		if (dir_found_flag)
		{
			strcpy(dir_here[dir_pos].filename, file_name);
			dir_here[dir_pos].i_no = inode_pos;
			return add_to_fdt(inode_pos);
		}
		else
		{
			fprintf(stderr, "Disk is full 1\n");
			return -1;			
		}
	}
	else
	{
		fprintf(stderr, "Disk is full 2\n");
		return -1;
	}
}

int my_erase(int file)				// Erases file contents, but not the file
{
	// file = index_from_fdt(file);
	// if (used[file] == 0)
	// 	return -1;
	// int file_org = file;
	// while(1)
	// {
	// 	strcpy(other_blocks + (file * sbc->block_size), "\0");
	// 	if (fat[file] == -1)
	// 		break;
	// 	file = fat[file];
	// 	used[file] = 0;
	// }
	// fat[file_org] = -1;
	// return 1;
}

// void set_seekw(int file, int val)
// {
// 	fdt[file].w_seek = val;
// }

// void set_seekr(int file, int val)
// {
// 	fdt[file].seek = val;
// }

int my_write(int file, char *text, int length, char mode)		// Writes to a file
{
	// int org_file = file;
	// int w_seek = fdt[org_file].w_seek;
	// file = index_from_fdt(file);
	// if (mode == 'w')
	// {
	// 	w_seek = 0;
	// 	my_erase(org_file);
	// }
	// else if (mode == 'a')
	// {
	// 	w_seek = 0;
	// 	while(1)
	// 		if (fat[file] == -1)
	// 			break;
	// 		else
	// 			file = fat[file];

	// }
	// else
	// {
	// 	int reached_end = 0;
	// 	while(w_seek > sbc->block_size)
	// 	{
	// 		if (fat[file] == -1)
	// 		{
	// 			reached_end = 1;
	// 			break;
	// 		}
	// 		file = fat[file];
	// 		w_seek -= sbc->block_size;
	// 	}
	// 	if (!reached_end)
	// 		*(other_blocks + ((sbc->block_size * file) + w_seek)) = '\0';
	// }
	// int block_size = sbc->block_size;
	// int count = 0;
	// do
	// {
	// 	int len_here, end_flag = 0;
	// 	if (length > block_size - 1 - strlen(other_blocks + sbc->block_size * file))
	// 		len_here = block_size - 1 - strlen(other_blocks + sbc->block_size * file);
	// 	else
	// 	{
	// 		len_here = length;
	// 		end_flag = 1;
	// 	}
	// 	char *write_here;
	// 	if (strlen(other_blocks + sbc->block_size * file) == 0)
	// 		write_here = other_blocks + sbc->block_size * file;
	// 	else
	// 		write_here = (other_blocks + sbc->block_size * file + strlen(other_blocks + sbc->block_size * file));
	// 	memcpy(write_here, text + count, len_here);
	// 	if (mode != 's')
	// 		write_here[len_here] = '\0';
	// 	count+=len_here;
	// 	length-=len_here;
	// 	// file->len = file->len + len_here;
	// 	if (end_flag)
	// 	{
	// 		fat[file] = -1;
	// 		break;
	// 	}
	// 	else
	// 	{
	// 		int i, found_flag = 0;
	// 		for (i = 0; i < sbc->num_blocks; i++)
	// 		{
	// 			if (used[i] == 0)
	// 			{
	// 				found_flag = 1;
	// 				break;
	// 			}
	// 		}
	// 		if (found_flag)
	// 		{
	// 			fat[file] = i;
	// 			used[i] = 1;
	// 			file = i;
	// 		}
	// 		else
	// 		{
	// 			fprintf(stderr, "Out of disk space\n");
	// 			return -1;
	// 		}
	// 	}
	// }while (length > 0);
	// return length;
}

int my_close(int file)		// Closes the descriptor to a file
{
	// if (fdt[file].use)
	// {
	// 	fdt[file].use = 0;
	// 	return 1;
	// }
	// else
	// {
	// 	fprintf(stderr, "File already closed\n");
	// 	return -1;
	// }
}


int my_cat(char *str)
{
	// int num_files = sbc->num_files;
	// int i, found = 0;
	// for (i = 0; i < num_files; i++)
	// {
	// 	// cout<<"Name of file is: "<<((FAT *) blocks[1])[i].filename<<endl;
	// 	if (strcmp(filename_map[i].filename, str) == 0)
	// 	{
	// 		// cout<<"Found :D\n";
	// 		found = 1;
	// 		break;
	// 	}
	// }

	// if (!found)
	// {
	// 	fprintf(stderr, "File not found\n");
	// 	return -1;
	// }

	// int index = filename_map[i].index;
	// while(1)
	// {
	// 	char *now = other_blocks + sbc->block_size * index;
	// 	cout<<now;
	// 	if (fat[index] != -1)
	// 		index = fat[index];
	// 	else
	// 	{
	// 		cout<<endl;
	// 		return 0;
	// 	}
	// }
}

void my_read(char *text, int file, int len)
{
	// // int num_files = sbc->num_files;
	// int fdt_file = file;
	// file = index_from_fdt(file);
	// int i = 0;
	// int here = file;
	// int seek_here = fdt[fdt_file].seek;
	// while(1)
	// {
	// 	char *now = other_blocks + (here * sbc->block_size);
	// 	if (seek_here < (sbc->block_size))
	// 	{
	// 		now+=seek_here;
	// 		for (int j = 0; j < strlen(now); j++)
	// 		{
	// 			text[i++] = now[j];
	// 			fdt[fdt_file].seek++;
	// 			if (i >= len)
	// 				return;
	// 		}	
	// 	}
	// 	else
	// 		seek_here -= sbc->block_size;
	// 	if (fat[here] != -1)
	// 		here = fat[here];
	// 	else
	// 	{
	// 		text[i] = '\0';
	// 		break;
	// 	}
	// }
}

int my_copy(char *system_file, char *file_here)
{
	// int file = my_open(file_here);
	// FILE *s_file;
	// s_file = fopen(system_file,"rb");
	// fseek(s_file,0,SEEK_END);
	// int size = ftell(s_file);
	// // cout<<"size is: "<<size<<endl;
	// char txt_here[size];
	// fseek(s_file, 0, SEEK_SET);
	// fread(txt_here, size, 1, s_file);
	// txt_here[size - 1] = '\0';
	// // cout<<"txt here is: "<<txt_here<<"\nDone."<<endl;
	// fclose(s_file);
	// int n = my_write(file, txt_here, size, 'w');
	// if (n >= 0)
	// 	return file;
	// else
	// 	return -1;
}

int main()
{
	int sys_size, block_size;
	cout<<"Enter size of file system in MB: ";
	cin>>sys_size;
	cout<<"\nEnter size of one block in KB: ";
	cin>>block_size;
	int num_blocks = ((sys_size * pow(2, 20))) / ((block_size * pow(2, 10)));
	cout<<"Num blocks in main: "<<num_blocks<<endl;
	blocks = (void *) malloc(sys_size * pow(2, 20));
	memset(blocks, 0, sys_size * pow(2, 20));


	fdt = (FDT*) malloc(num_blocks * sizeof(FDT));
	for (int i = 0; i < num_blocks; i++)
		fdt[i].use = 0;

	sbc = (super_block_constants*) blocks;
	strcpy(sbc->vol_name, "my_sda");
	sbc->vol_size = sys_size * pow(2, 20);
	sbc->block_size = block_size * pow(2, 10);
	sbc->num_blocks = num_blocks;
	sbc->free_ptr = 4;

	inodes = (inode*) (blocks + (sbc->block_size));
	inodes[0].valid = 1;
	inodes[0].type = 2;
	inodes[0].directly[0] = 3;

	for (int i = 1; i < sbc->num_blocks; i++)
		inodes[i].valid = -1;

	curr_dir = 3;

	dir_entry* dir_here = (dir_entry*) (blocks + 3 * (sbc->block_size));
	for (int i = 0; i < (sbc->block_size / 16); i++)
		dir_here[i].i_no = -1;
	// Create free blocks
	void* block_here = (blocks + sbc->free_ptr * (sbc->block_size));
	for (int i = 4; i < num_blocks; i++)
	{
		// printf("%d\n", i);
		((free_block*) block_here) -> next_ptr = i + 1;
		if ((i + 1) == num_blocks)
		{
			((free_block*) block_here) -> next_ptr = -1;
			break;
		}
		block_here = (blocks + (i + 1) * (sbc->block_size));

	}

	// API testing

	int file = my_open("hello");
	// my_write(file, "the quick brown fox jumps over the lazy dog", 43, 'w');
	// my_cat("hello");

	// my_write(file, "test1", 5, 'w');
	// my_cat("hello");

	// my_write(file, " test 2 test3", 9, 'a');
	// my_cat("hello");

	// int file2 = my_copy("test", "test2");
	// int file3 = my_open("test2");
	// my_cat("test2");

	// char txt_here[100];
	// my_read(txt_here, file3, 7);
	// cout<<"Text is: "<<txt_here<<endl;
	// my_read(txt_here, file3, 3);
	// cout<<"Text is: "<<txt_here<<endl;
	// set_seekw(file2, 2);
	// my_write(file2, "K", 1, 's');
	// my_cat("test2");
	// my_close(file2);
	// my_read(txt_here, file2, 7);
	// cout<<"Done."<<endl;
}