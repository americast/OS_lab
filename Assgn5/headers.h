struct page_entry{
	int page;
	int frame;
	int validity = -1;
};

struct main_mem_frame{
	int frame;
	int free;
	int use = 0;
};

struct mq {
	char msg[100];
};

struct map_
{
	int page_no;
	int memory_loc;
};

struct rq { 
	short int id;
    pid_t pid; 
}; 