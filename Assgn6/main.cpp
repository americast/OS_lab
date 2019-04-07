#include <bits/stdc++.h>
#include <math.h>
using namespace std;

struct block
{
	char* buf;
	int len;
	int i;
	block* next_ptr;
};

typedef block my_file;

struct FAT
{
	char filename[100];
	block* ptr;
};

struct DIR
{};

struct super_block
{
	int sys_size, num_blocks, block_size, num_files;
	FAT* fat;
	DIR* dir;
	int* free;
};



void* *blocks;

block* my_open(char *file_name)
{
	int num_blocks = ((super_block *) blocks[0])->num_blocks;
	int found = 0, pos;
	// cout<<"num_blocks: "<<num_blocks<<endl;
	for (int i = 0; i < num_blocks - 1; i++)
	{
		if (((super_block *) blocks[0])->free[i] == 0)
		{
			found = 1;
			pos = i;
			break;
		}
	}
	if (found)
	{
		int i = pos;
		((super_block *) blocks[0])->free[i] = 1;
		strcpy(((super_block *) blocks[0])->fat[i].filename, file_name);
		block *here = (block *) malloc(sizeof(block));
		here->next_ptr = NULL;
		blocks[i + 1] = here;
		// cout<<"i is: "<<i<<endl;
		((super_block *) blocks[0])->fat[i].ptr = here;
		here->len = 0;
		here->i = i;
		((super_block *) blocks[0])->num_files++;
		return here;
	}
	else
	{
		fprintf(stderr, "Disk is full\n");
		return NULL;
	}
}

int my_write(block *file, char *text, int length)
{
	int block_size = ((super_block *) blocks[0])->block_size;
	int count = 0;
	do
	{
		int len_here, end_flag = 0;
		if (length > block_size - sizeof(void *) - 2 * sizeof(int))
			len_here = block_size - sizeof(void *) - 2 * sizeof(int);
		else
		{
			len_here = length;
			end_flag = 1;
		}
		file->buf = (char *) malloc(sizeof(char) * len_here);
		memcpy(file->buf, text + count, len_here);
		count+=len_here;
		length-=len_here;
		file->len = len_here;
		if (end_flag)
		{
			file->next_ptr = NULL;
			break;
		}
		else
		{
			int i, found_flag = 0;
			for (i = 0; i < ((super_block *)blocks[0])->num_blocks - 1; i++)
			{
				if (((super_block *)blocks[0])->free[i] == 0)
				{
					found_flag = 1;
					break;
				}
			}
			if (found_flag)
			{
				block *here = (block *) malloc(sizeof(block));;
				blocks[i + 1] = here;
				file->next_ptr = here;
				file = here;
				file->i = i;
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

int my_cat(char *str)
{
	int num_files = ((super_block *) blocks[0])->num_files;
	int i, found = 0;
	for (i = 0; i < num_files; i++)
	{
		// cout<<"Name of file is: "<<((super_block *) blocks[0])->fat[i].filename<<endl;
		if (strcmp(((super_block *) blocks[0])->fat[i].filename, str) == 0)
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
	block *here = ((super_block *) blocks[0])->fat[i].ptr;
	while(1)
	{
		int len = here->len;
		// cout<<"len is "<<len<<endl;
		char *now = here->buf;
		for (int j = 0; j < len; j++)
			cout<<now[j];
		if (here->next_ptr != NULL)
			here = here->next_ptr;
		else
		{
			cout<<endl;
			return 0;
		}
	}
}

void my_read(char *text, my_file *file, int len)
{
	int num_files = ((super_block *) blocks[0])->num_files;
	int i = 0;
	block *here = file;
	while(1)
	{
		int len = here->len;
		// cout<<"len is "<<len<<endl;
		char *now = here->buf;
		for (int j = 0; j < len; j++)
		{
			text[i++] = now[j];
			if (i >= len)
				return;
		}
		if (here->next_ptr != NULL)
			here = here->next_ptr;
		else
		{
			text[i] = '\0';
			break;
		}
	}
}

block* my_copy(char *system_file, char *file_here)
{
	my_file *file = my_open(file_here);
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
	int n = my_write(file, txt_here, size);
	if (n >= 0)
		return file;
	else
		return NULL;

}

int main()
{
	int sys_size, block_size;
	cout<<"Enter size of file system in MB: ";
	cin>>sys_size;
	cout<<"\nEnter size of one block in KB: ";
	cin>>block_size;
	int num_blocks = (sys_size * pow(2, 20) - (sizeof(super_block) - sizeof(int*) - sizeof(FAT*)) ) / (block_size * pow(2, 10) + (sizeof(FAT) + sizeof(int)));
	cout<<"Num blocks in main: "<<num_blocks<<endl;
	blocks = (void **) malloc(sizeof(void *) * sizeof(num_blocks));

	super_block *sb = (super_block *) malloc(sizeof(super_block));

	blocks[0] = sb;

	sb->fat = (FAT *) malloc(sizeof(FAT) * (num_blocks - 1));
	sb->free = (int *) malloc(sizeof(int) * (num_blocks- 1));
	sb->sys_size = sys_size;
	sb->block_size = block_size;
	sb->num_blocks = num_blocks;
	sb->num_files = 0;

	for (int i = 0; i < num_blocks; i++)
		sb->free[i] = 0;

	my_file *file = my_open("hello");
	my_write(file, "uerhfuerhfuihrfuhrukfhkfhskhfkshfksdhfkdshkdjcdjkckdcjkdbckddbc", 61);
	my_cat("hello");
	my_file *file2 = my_copy("test", "test2");
	char txt_here[100];
	my_read(txt_here, file2, 10);
	cout<<"Text is: "<<txt_here<<endl;
	cout<<"Done."<<endl;
}