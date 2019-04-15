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
	int r_seek;
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

int curr_dir, curr_dir_inode = 0;

int add_to_fdt(int index)
{
	for (int i = 0; i < sbc->num_blocks; i++)
	{
		if (fdt[i].use == 0)
		{
			fdt[i].use = 1;
			fdt[i].r_seek = 0;
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
	int inode_pos, dir_pos;
	for (int i = 0; i < sbc->num_blocks; i++)
	{
		if (inodes[i].valid == -1)
		{
			inodes[i].valid = 1;
			inodes[i].type = 1;
			inodes[i].singly = -1;
			for (int j = 0; j < 5; j++)
				inodes[i].directly[j] = -1;
			inode_pos = i;
			break;
		}
	}

	int dir_found_flag = 0;

	for (int j = 0; j < 5; j++)
	{
		if (dir_found_flag)
			break;
		curr_dir = inodes[curr_dir_inode].directly[j];
		if (curr_dir == -1)
		{
			curr_dir = sbc->free_ptr;
			sbc->free_ptr = ((free_block *)(blocks + curr_dir * sbc->block_size))->next_ptr;
		}
		for (int i = 0; i < block_size / 16; i++)
		{
			if (dir_here[i].i_no == -1)
			{
				dir_pos = i;
				dir_found_flag = 1;
				break;
			}
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


	if (!dir_found_flag)
	{
		// singly
		cout<<"Inside singly"<<endl;
		int ptr_singly;
		int is_free = 0;
		if (inodes[curr_dir_inode].singly == -1)
		{
			inodes[curr_dir_inode].singly = sbc->free_ptr;
			ptr_singly = sbc->free_ptr;
			sbc->free_ptr = ((free_block *)(blocks + ptr_singly * sbc->block_size))->next_ptr;
			is_free = 1;
		}

		inode* inode_at_single = (inode *) (blocks + ptr_singly * sbc->block_size);
		if (is_free)
		{
			for (int i = 0; i < sbc->block_size / sizeof(inode); i++)
			{
				for (int j = 0; j < 5; j++)
					inode_at_single[i].directly[j] = -1;
			}
		}

		for (int k = 0; k < sbc->block_size / sizeof(inode); k++)
		{
			if (dir_found_flag)
				break;
			for (int j = 0; j < 5; j++)
			{
				if (dir_found_flag)
					break;
				curr_dir = inode_at_single[k].directly[j];
				if (curr_dir == -1)
				{
					curr_dir = sbc->free_ptr;
					inode_at_single[k].directly[j] = curr_dir;
					sbc->free_ptr = ((free_block *)(blocks + curr_dir * sbc->block_size))->next_ptr;
				}
				for (int i = 0; i < block_size / 16; i++)
				{
					if (dir_here[i].i_no == -1)
					{
						dir_pos = i;
						dir_found_flag = 1;
						break;
					}
				}		
			}	
		}
	}

	if (!dir_found_flag)
	{
		// doubly
		cout<<"Inside doubly"<<endl;
		int ptr_doubly, ptr_singly;
		int is_free = 0;
		if (inodes[curr_dir_inode].doubly == -1)
		{
			inodes[curr_dir_inode].doubly = sbc->free_ptr;
			ptr_doubly = sbc->free_ptr;
			sbc->free_ptr = ((free_block *)(blocks + ptr_doubly * sbc->block_size))->next_ptr;
			is_free = 1;
		}
		inode* inode_at_double = (inode *) (blocks + ptr_doubly * sbc->block_size);
		for (int l = 0; l < sbc->block_size / sizeof(inode); l++)
		{

			if (inode_at_double[l].singly == -1)
			{
				inode_at_double[l].singly = sbc->free_ptr;
				ptr_singly = sbc->free_ptr;
				sbc->free_ptr = ((free_block *)(blocks + ptr_singly * sbc->block_size))->next_ptr;
				is_free = 1;
			}

			inode* inode_at_single = (inode *) (blocks + inode_at_double[l].singly * sbc->block_size);
			if (is_free)
			{
				for (int i = 0; i < sbc->block_size / sizeof(inode); i++)
				{
					for (int j = 0; j < 5; j++)
						inode_at_single[i].directly[j] = -1;
				}
			}

			for (int k = 0; k < sbc->block_size / sizeof(inode); k++)
			{
				if (dir_found_flag)
					break;
				for (int j = 0; j < 5; j++)
				{
					if (dir_found_flag)
						break;
					curr_dir = inode_at_single[k].directly[j];
					if (curr_dir == -1)
					{
						curr_dir = sbc->free_ptr;
						inode_at_single[k].directly[j] = curr_dir;
						sbc->free_ptr = ((free_block *)(blocks + curr_dir * sbc->block_size))->next_ptr;
					}
					for (int i = 0; i < block_size / 16; i++)
					{
						if (dir_here[i].i_no == -1)
						{
							dir_pos = i;
							dir_found_flag = 1;
							break;
						}
					}		
				}	
			}

		}
	}
}


void set_seekw(int file, int val)
{
	fdt[file].w_seek = val;
}

void set_seekr(int file, int val)
{
	fdt[file].r_seek = val;
}

void add_to_free(int block_num)
{
	// cout<<"block num to free "<<block_num<<endl;
	int curr_free = sbc->free_ptr;
	void* block_here = (blocks + block_num * sbc->block_size);
	memset(block_here, 0, sbc->block_size);
	free_block* free_block_here = (free_block *) block_here;
	free_block_here->next_ptr = curr_free;
	sbc->free_ptr = block_num;
}

int my_erase(int inode_num)				// Erases file contents, but not the file
{
	// cout<<"inode_num: "<<inode_num<<endl;
	inode inode_here = inodes[inode_num];
	// directly
	for (int i = 0; i < 5; i++)
		if (inode_here.directly[i] != -1)
		{
			// cout<<"inode_here.directly[i] "<<inode_here.directly[i]<<endl;
			add_to_free(inode_here.directly[i]);
			inode_here.directly[i] = -1;
		}

	int singly_pos = inode_here.singly;
	// cout<<"Singly: "<<singly_pos<<endl;
	if (singly_pos < 0)
		return 1;

	inode* inodes_there = (inode *)(blocks + singly_pos * sbc->block_size);

	for (int j = 0; j < sbc->num_blocks; j++)
	{
		if (inodes_there[j].valid == -1)
			break;
		for (int i = 0; i < 5; i++)
			if (inodes_there[j].directly[i] != -1)
			{
				add_to_free(inodes_there[j].directly[i]);
				inodes_there[j].directly[i] = -1;
			}
		inodes_there[j].valid = -1;
	}

	return 1;
}

int my_write(int file, char *text_here, int length, char mode)		// Writes to a file
{
	int org_file = file;
	file = index_from_fdt(file);
	inode i_here = inodes[file];
	int	seek = fdt[org_file].w_seek;

	if (mode == 'w')
	{
		my_erase(file);
		seek = 0;
		fdt[org_file].w_seek = 0;
	}
	if (mode == 'a')
	{
		seek = inodes[file].size;
		fdt[org_file].w_seek = seek;
	}
	int count_text = 0;
	int seek_here = 0;
	char text[length + 1];
	strcpy(text, text_here);

	if (mode == 'w' || mode == 'a')
	{
		strcat(text, "\0");
		length++;
	}

	length+=seek;

	// cout<<"text: "<<text<<endl;

	for (int i = 0; i < 5; i++)
	{
		// cout<<"Seek here: "<<seek<<endl;
		if (length <= 0)
			break;
		int free_block_index = sbc->free_ptr;
		if (seek == 0)
			sbc->free_ptr = ((free_block *) (blocks + free_block_index * sbc->block_size))->next_ptr;
		int len_here = sbc->block_size;
		if (length < sbc->block_size)
			len_here = length;
		char *block_here;
		if (seek > 0)
			block_here = (char*) (blocks + inodes[file].directly[i] * sbc->block_size);
		else
			block_here = (char*) (blocks + free_block_index * sbc->block_size);
		if (seek == 0)
			inodes[file].directly[i] = free_block_index;
		for (int i = 0; i < len_here; i++)
		{
			// cout<<"Seek here in: "<<seek<<endl;
			if (seek <= 0)
			{
				// cout<<"i : "<<i<<endl;
				// cout<<"char here: "<<*(text + count_text + i)<<endl;
				memcpy(block_here + i, text + count_text, 1);
				length--;
				count_text++;
				fdt[org_file].w_seek++;
			}
			else
				seek--;
		}
	}

	if (length <= 0)
	{
		if (mode == 'w' || mode == 'a')
			inodes[file].size = fdt[org_file].w_seek - 1;
		else
			inodes[file].size = fdt[org_file].w_seek;
		return count_text;
	}

	int free_block_index;
	void *block_single;
	if (seek == 0)
	{
		free_block_index = sbc->free_ptr;
		sbc->free_ptr = ((free_block *) (blocks + free_block_index * sbc->block_size))->next_ptr;
		block_single = (blocks + free_block_index * sbc->block_size);
		inodes[file].singly = free_block_index;
	}
	else
		block_single = (blocks + inodes[file].singly * sbc->block_size);

	inode* inode_at_single = (inode*) block_single;

	if (seek == 0)
		for (int i = 0; i < (sbc->block_size) / sizeof(inode); i++)
			inode_at_single[i].valid = -1;

	int fin = 0;
	for (int j = 0; j < (sbc->block_size) / sizeof(inode); j++)
	{
		if (fin)
			break;
		if (inode_at_single[j].valid == -1)
		{
			inode_at_single[j].valid == 1;
			for (int i = 0; i < 5; i++)
			{
				if (length <= 0)
				{
					fin = 1;
					break;
				}
				int free_block_index = sbc->free_ptr;
				if (seek == 0)
					sbc->free_ptr = ((free_block *) (blocks + free_block_index * sbc->block_size))->next_ptr;
				int len_here = sbc->block_size;
				if (length < sbc->block_size)
					len_here = length;
				char *block_here;
				if (seek > 0)
					block_here = (char*) (blocks + inode_at_single[j].directly[i] * sbc->block_size);
				else
					block_here = (char*) (blocks + free_block_index * sbc->block_size);
				if (seek == 0)
					inode_at_single[j].directly[i] = free_block_index;
				for (int i = 0; i < len_here; i++)
				{
					// cout<<"Seek here in: "<<seek<<endl;
					if (seek <= 0)
					{
						// cout<<"i : "<<i<<endl;
						// cout<<"char here: "<<*(text + count_text + i)<<endl;
						memcpy(block_here + i, text + count_text, 1);
						length--;
						count_text++;
						fdt[org_file].w_seek++;
					}
					else
						seek--;
				}
			}
		}

	}

	if (length <= 0)
	{
		if (mode == 'w' || mode == 'a')
			inodes[file].size = fdt[org_file].w_seek - 1;
		else
			inodes[file].size = fdt[org_file].w_seek;
		return count_text;
	}


	free_block_index = -1;
	void *block_double;
	if (seek == 0)
	{
		free_block_index = sbc->free_ptr;
		sbc->free_ptr = ((free_block *) (blocks + free_block_index * sbc->block_size))->next_ptr;
		block_double = (blocks + free_block_index * sbc->block_size);
		inodes[file].doubly = free_block_index;
	}
	else
		block_double = (blocks + inodes[file].doubly * sbc->block_size);

	inode* inode_at_double = (inode*) block_single;

	if (seek == 0)
		for (int i = 0; i < (sbc->block_size) / sizeof(inode); i++)
			inode_at_double[i].valid = -1;

	fin = 0;
	for (int j = 0; j < (sbc->block_size) / sizeof(inode); j++)
	{
		if (fin)
			break;
		if (inode_at_double[j].valid == -1)
		{

			inode* inode_at_single = (inode*) (blocks + inode_at_double[j].directly[0]);

			for (int j = 0; j < (sbc->block_size) / sizeof(inode); j++)
			{
				inode_at_single[j].valid == 1;
				for (int i = 0; i < 5; i++)
				{
					if (length <= 0)
					{
						fin = 1;
						break;
					}
					int free_block_index = sbc->free_ptr;
					if (seek == 0)
						sbc->free_ptr = ((free_block *) (blocks + free_block_index * sbc->block_size))->next_ptr;
					int len_here = sbc->block_size;
					if (length < sbc->block_size)
						len_here = length;
					char *block_here;
					if (seek > 0)
						block_here = (char*) (blocks + inode_at_single[j].directly[i] * sbc->block_size);
					else
						block_here = (char*) (blocks + free_block_index * sbc->block_size);
					if (seek == 0)
						inode_at_single[j].directly[i] = free_block_index;
					for (int i = 0; i < len_here; i++)
					{
						// cout<<"Seek here in: "<<seek<<endl;
						if (seek <= 0)
						{
							// cout<<"i : "<<i<<endl;
							// cout<<"char here: "<<*(text + count_text + i)<<endl;
							memcpy(block_here + i, text + count_text, 1);
							length--;
							count_text++;
							fdt[org_file].w_seek++;
						}
						else
							seek--;
					}
				}

			}
		}

	}

	if (length <= 0)
	{
		if (mode == 'w' || mode == 'a')
			inodes[file].size = fdt[org_file].w_seek - 1;
		else
			inodes[file].size = fdt[org_file].w_seek;
		return count_text;
	}
}

int my_close(int file)		// Closes the descriptor to a file
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



void my_read(int file, char *text, int len)
{
	int org_file = file;
	int seek = fdt[org_file].r_seek;
	memset(text, 0, len + 1);
	file = index_from_fdt(file);
	inode i_here = inodes[file];
	int count_text = 0;
	int count_before_seek = 0;

	
	for (int i = 0; i < 5; i++)
	{
		int block_here_index = i_here.directly[i];
		char* block_here = (char*) (blocks + block_here_index * sbc->block_size);
		int len_here = strlen(block_here);
		if (len_here >= sbc->block_size)
		{
			len_here = sbc->block_size;
			for (int i = 0; i < len_here; i++)
			{
				if (count_before_seek >= seek)
				{
					memcpy(text + count_text, block_here + i, 1);
					count_text++;
					fdt[org_file].r_seek++;
				}
				else
					count_before_seek++;
				if (count_text >=len)
					return;
			}
		}
		else
		{
			len_here++;
			for (int i = 0; i < len_here; i++)
			{
				if (count_before_seek >= seek)
				{
					memcpy(text + count_text, block_here + i, 1);
					count_text++;
					fdt[org_file].r_seek++;
				}
				else
					count_before_seek++;
				if (count_text >=len)
					return;
			}
			return;
		}
	}


	int single_block_here_index = inodes[file].singly;
	inode* inode_at_single = (inode*) (blocks + single_block_here_index * sbc->block_size);

	for (int j = 0; j < (sbc->block_size) / sizeof(inode); j++)
	{
		if (inode_at_single[j].valid == -1)
			return;
		else
		{
			inode_at_single[j].valid == 1;
			for (int i = 0; i < 5; i++)
			{
				int block_here_index = inode_at_single[j].directly[i];
				char* block_here = (char*) (blocks + block_here_index * sbc->block_size);
				int len_here = strlen(block_here);
				if (len_here >= sbc->block_size)
				{
					len_here = sbc->block_size;
					for (int i = 0; i < len_here; i++)
					{
						if (count_before_seek >= seek)
						{
							memcpy(text + count_text, block_here + i, 1);
							count_text++;
							fdt[org_file].r_seek++;
						}
						else
							count_before_seek++;
						if (count_text >=len)
							return;
					}
				}
				else
				{
					len_here++;
					for (int i = 0; i < len_here; i++)
					{
						if (count_before_seek >= seek)
						{
							memcpy(text + count_text, block_here + i, 1);
							count_text++;
							fdt[org_file].r_seek++;
						}
						else
							count_before_seek++;
						if (count_text >=len)
							return;
					}
					return;
				}
			}
		}

	}
}

void my_cat(int file)
{
	int r_seek_org = fdt[file].r_seek;
	fdt[file].r_seek = 0;

	int file_here = index_from_fdt(file);
	char text[inodes[file_here].size];

	my_read(file, text, inodes[file_here].size);
	cout<<text<<endl;

	fdt[file].r_seek = r_seek_org;
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

int my_mkdir(char *dir_name)
{
	int block_size = sbc->block_size;

	int i;
	dir_entry* dir_here = (dir_entry*) (blocks + curr_dir * block_size);
	for (i = 0; i < block_size / 16; i++)
	{
		if (strcmp(dir_here[i].filename, dir_name) == 0 && dir_here[i].i_no != -1 && inodes[dir_here[i].i_no].type == 2)
		{
			fprintf(stderr, "Directory exists\n");
			return 1;
		}
	}

	int num_blocks = sbc->num_blocks;
	int free_pos = sbc->free_ptr;
	if (free_pos)
	{
		void* block_here = (blocks + (free_pos * sbc->block_size));
		sbc->free_ptr = ((free_block*) (blocks + (free_pos * sbc->block_size)))->next_ptr;

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
			strcpy(dir_here[dir_pos].filename, dir_name);
			dir_here[dir_pos].i_no = inode_pos;
			inodes[inode_pos].type = 2;
			inodes[inode_pos].directly[0] = free_pos;
			dir_entry* dir_new = (dir_entry *) (blocks + free_pos * sbc->block_size);
			strcpy(dir_new[0].filename, ".");
			strcpy(dir_new[1].filename, "..");
			dir_new[0].i_no = inode_pos;
			dir_new[1].i_no = curr_dir_inode;
			return 1;
		}
		else
		{
			fprintf(stderr, "Disk full\n");
		}
	}
	else
	{
		fprintf(stderr, "Disk full\n");
	}

}

int my_chdir(char *dir_name)
{
	int block_size = sbc->block_size;

	int i, found = 0;
	// cout<<"ch curr_dir: "<<curr_dir<<endl;
	dir_entry* dir_here = (dir_entry*) (blocks + curr_dir * block_size);
	for (i = 0; i < block_size / 16; i++)
	{
		if (strcmp(dir_here[i].filename, dir_name) == 0 && dir_here[i].i_no != -1 && inodes[dir_here[i].i_no].type == 2)
		{
			// cout<<"ch Found at "<<i<<endl;
			// cout<<"ch dir_here[i].i_no: "<<dir_here[i].i_no<<endl;
			curr_dir = inodes[dir_here[i].i_no].directly[0];
			curr_dir_inode = dir_here[i].i_no;
			return 1;
		}
	}
	fprintf(stderr, "Directory not found\n");
	return -1;
}

int my_rmdir(char *dir_name)
{
	int block_size = sbc->block_size;

	int i, found = 0;
	// cout<<"rm curr_dir: "<<curr_dir<<endl;
	dir_entry* dir_here = (dir_entry*) (blocks + curr_dir * block_size);
	for (i = 0; i < block_size / 16; i++)
	{
		if (strcmp(dir_here[i].filename, dir_name) == 0 && dir_here[i].i_no != -1 && inodes[dir_here[i].i_no].type == 2)
		{
			for (int k = 0; k < 5; k++)
			{
				int block_num = inodes[dir_here[i].i_no].directly[k];
				if (block_num == -1)
					break;
				dir_entry* dir_here_inside = (dir_entry *) (blocks + block_num * sbc->block_size);
				for (int j = 2; j < sbc->block_size / sizeof(dir_entry); j++)
				{
					if (dir_here_inside[j].i_no > 0)
					{
						// cout<<"dir test filenames: "<<dir_here_inside[j].filename<<endl;
						my_erase(dir_here_inside[j].i_no);
					}
				}
				// cout<<"parent block num free: "<<block_num<<endl;
				add_to_free(block_num);
				// memcpy(dir_here[i].filename, 0, 14);
				// cout<<"-1 i "<<i<<endl;
				// cout<<"rm Found at "<<i<<endl;
				dir_here[i].i_no = -1;
				// cout<<"rm dir_here[i].i_no = -1: "<<dir_here[i].i_no<<endl;
				return 1;
			}
			int singly = inodes[dir_here[i].i_no].singly, doubly = inodes[dir_here[i].i_no].doubly;
			if (singly)
			{
				inode *inodes_here = (inode*) (blocks + singly * sbc->block_size);
				for (int j_ = 0; j_ < sbc->block_size / sizeof(inode); j_++)
				{
					for (int k = 0; k < 5; k++)
					{
						if (inodes_here[j_].directly[k] == -1)
							break;
						dir_entry* dir_here_inside = (dir_entry *) (blocks + inodes_here[j_].directly[k] * sbc->block_size);
						for (int j = 2; j < sbc->block_size / sizeof(dir_entry); j++)
						{
							if (dir_here_inside[j].i_no > 0)
							{
								// cout<<"dir test filenames: "<<dir_here_inside[j].filename<<endl;
								my_erase(dir_here_inside[j].i_no);
							}
						}
						// cout<<"parent block num free: "<<block_num<<endl;
						add_to_free(inodes_here[j_].directly[k]);
						// memcpy(dir_here[i].filename, 0, 14);
						// cout<<"-1 i "<<i<<endl;
						// cout<<"rm Found at "<<i<<endl;
						dir_here[i].i_no = -1;
						// cout<<"rm dir_here[i].i_no = -1: "<<dir_here[i].i_no<<endl;
						return 1;

					}
				}
			}

			if (doubly)
			{
				inode *inodes_here = (inode*) (blocks + doubly * sbc->block_size);
				for (int j_1 = 0; j_1 < sbc->block_size / sizeof(inode); j_1++)
				{
					inode *inodes_here = (inode*) (blocks + inodes_here[j_1].singly * sbc->block_size);
					for (int j_ = 0; j_ < sbc->block_size / sizeof(inode); j_++)
					{
						for (int k = 0; k < 5; k++)
						{
							if (inodes_here[j_].directly[k] == -1)
								break;
							dir_entry* dir_here_inside = (dir_entry *) (blocks + inodes_here[j_].directly[k] * sbc->block_size);
							for (int j = 2; j < sbc->block_size / sizeof(dir_entry); j++)
							{
								if (dir_here_inside[j].i_no > 0)
								{
									// cout<<"dir test filenames: "<<dir_here_inside[j].filename<<endl;
									my_erase(dir_here_inside[j].i_no);
								}
							}
							// cout<<"parent block num free: "<<block_num<<endl;
							add_to_free(inodes_here[j_].directly[k]);
							// memcpy(dir_here[i].filename, 0, 14);
							// cout<<"-1 i "<<i<<endl;
							// cout<<"rm Found at "<<i<<endl;
							dir_here[i].i_no = -1;
							// cout<<"rm dir_here[i].i_no = -1: "<<dir_here[i].i_no<<endl;
							return 1;

						}
					}
				}
			}
		}
	}
	fprintf(stderr, "Directory not found\n");
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
	for (int i = 0; i < sbc->num_blocks; i++)
	{
		inodes[i].valid = -1;
		for (int j = 0; j < 5; j++)
			inodes[i].directly[j] = -1;
		inodes[i].singly = -1;
		inodes[i].doubly = -1;
	}

	inodes[0].valid = 1;
	inodes[0].type = 2;
	inodes[0].directly[0] = 3;


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
	my_write(file, "the quick brown fox jumps over the lazy dog", 43, 'w');
	my_cat(file);

	my_mkdir("test");
	printf("%d\n", curr_dir);
	my_chdir("test");
	printf("%d\n", curr_dir);
	int file3 = my_open("hello2");
	my_write(file3, "oh!", 3, 'w');
	my_cat(file3);
	my_chdir("..");
	printf("%d\n", curr_dir);
	cout<<"Del dir"<<endl;
	my_rmdir("test");
	cout<<"Ch to del dir"<<endl;
	my_chdir("test");
	cout<<"Print file 3\n";
	my_cat(file3);	// check this
	// my_chdir("..");

	char text[100];
	my_read(file, text, 3);
	printf("From read: %s\n", text);

	my_read(file, text, 3);
	printf("Second read: %s\n", text);

	my_write(file, "hello", 5, 'w');
	my_cat(file);

	cout<<"Starting copy"<<endl;
	int file2 = my_copy("test", "test_here");
	// cout<<"file 2 no: "<<file2<<endl;
	my_cat(file2);

	my_close(file2);

	set_seekw(file, 2);
	my_write(file, "a", 1, 's');
	my_cat(file);
	my_write(file, "ab", 2, 'a');
	my_cat(file);
	my_mkdir("test");
	my_mkdir("test");
	my_cat(file2);
}