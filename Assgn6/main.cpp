#include <bits/stdc++.h>
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
	int sys_size, num_blocks;
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

void* my_open(char *file_name)
{
	int num_blocks = ((super_block *) blocks[0])->num_blocks;
	int found = 0, pos;
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

int main()
{
	int sys_size, block_size;
	cout<<"Enter size of file system in MB: ";
	cin>>sys_size;
	cout<<"\nEnter size of one block in KB: ";
	cin>>block_size;
	int num_blocks = (sys_size - (sizeof(super_block) - sizeof(int*) - sizeof(FAT*)) )* pow(2, 20) / (block_size * pow(2, 10) + (sizeof(FAT) + sizeof(int)));
	blocks = (void **) malloc(sizeof(void *) * sizeof(num_blocks));

	super_block *sb = (super_block *) malloc(sizeof(super_block));

	blocks[0] = sb;

	sb->fat = (FAT *) malloc(sizeof(FAT) * (num_blocks - 1));
	sb->free = (int *) malloc(sizeof(int) * (num_blocks- 1));
	sb->sys_size = sys_size;
	sb->num_blocks = block_size;

	for (int i = 0; i < num_blocks; i++)
		sb->free[i] = 0;

	my_open("hello");
}