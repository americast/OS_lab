#include <bits/stdc++.h>

struct super_block
{
	int sys_size, block_size;
	FAT* fat;
	DIR* dir;
	int* free;
};

struct block
{
	char* buf;
	void* next_ptr;
}

struct FAT
{
	char filename[100];
	void* ptr;
}

int main()
{
	int sys_size, block_size;
	cout<<"Enter size of file system in MB: ";
	cin>>sys_size;
	cout<<"\nEnter size of one block in KB: ";
	cin>>block_size;
	int num_blocks = (sys_size - (sizeof(super_block) - sizeof(int*) - sizeof(FAT*)) )* pow(2, 20) / (block_size * pow(2, 10) + (sizeof(FAT) + sizeof(int)));
	void* blocks[num_blocks];

	super_block *sb = (super_block *) malloc(sizeof(super_block));

	blocks[0] = sb;

	sb->fat = (FAT *) malloc(sizeof(FAT) * num_blocks);
	sb->free = (int *) malloc(sizeof(int) * num_blocks);
	sb->sys_size = sys_size;
	sb->block_size = block_size;

	for (int i = 0; i < num_blocks; i++)
		sb->free[i] = 0;


}