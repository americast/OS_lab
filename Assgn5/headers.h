struct page_entry{
	int page;
	int frame;
	int validity = -1;
};

struct main_mem_frame{
	int frame;
	int free;
	int use = 0;
	int hist = 0;
};

struct mq {
	long type;
	char msg[100];
};

struct map_
{
	int page_no;
	int memory_loc;
};

struct rq { 
	long type;
    char pid[100]; 
}; 

struct pg_num
{
	long type;
	char txt[100];
};