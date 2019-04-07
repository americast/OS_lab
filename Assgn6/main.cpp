#include <bits/stdc++.h>
#include <math.h>
using namespace std;

struct FAT
{
	char filename[100];
	void* ptr;
};

struct DIR
{};

struct super_block
{
	int sys_size, num_blocks, block_size;
	FAT* fat;
	DIR* dir;
	int* free;
};

struct block
{
	char* buf;
	void* next_ptr;
};


void* *blocks;

block* my_open(char *file_name)
{
	int num_blocks = ((super_block *) blocks[0])->num_blocks;
	int found = 0, pos;
	cout<<"num_blocks: "<<num_blocks<<endl;
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
		((super_block *) blocks[0])->fat[i].ptr = here;
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
		if (length > block_size - sizeof(void *))
			len_here = block_size - sizeof(void *);
		else
		{
			len_here = length;
			end_flag = 1;
		}
		file->buf = (char *) malloc(sizeof(char) * len_here);
		memcpy(file->buf, text + count, len_here);
		count+=len_here;
		length-=len_here;
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
				blocks[i] = here;
				file->next_ptr = here;
				file = here;
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

	for (int i = 0; i < num_blocks; i++)
		sb->free[i] = 0;

	block *file = my_open("hello");
	my_write(file, "text", 5);
}