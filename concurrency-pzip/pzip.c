///////////////////Libraries///////////////////////
#include <fcntl.h>      // open, O_* constants
#include <stdio.h>      // fwrite
#include <stdlib.h>     // exit, malloc
#include <string.h>
#include <sys/mman.h>   // Library for mmap
#include <pthread.h>
#include <sys/stat.h>   // Library for struct stat,fstat
#include <sys/sysinfo.h>
#include <unistd.h>
///////////////////////////////////////////////////

//////////////////////NOTES/////////////////////////////
/*void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
addr is the beginning of virtual address that will be mapped to and if it is NULL
then the kernel chooses the (page-aligned) address at which to create the mapping,

the length specify the length of the mapping ,

The prot argument describes the desired memory protection of the mapping

The flags argument determines whether updates to the mapping are
visible to other processes mapping the same region

fd is the opend  file descriptor

The offset argument specifies the first byte of the file to be mapped.
*/

/*
Struct stat contains:
dev_t     st_dev     Device ID of device containing file.
ino_t     st_ino     File serial number.
mode_t    st_mode    Mode of protection for the  file.
nlink_t   st_nlink   Number of hard links to the file.
uid_t     st_uid     User ID of file.
gid_t     st_gid     Group ID of file.
off_t     st_size    The file size in bytes.
*/
//////////////////////////////////////////////////////////

/////////////////GLOBAL VARIABLES////////////////////////
int total_threads; //Total number of threads that will be created for consumer.
long page_size;
int num_files; //Total number of the files passed as the arguments.
int producer_isComplete = 0; //Flag needed to tell the consumer that the producer is done and  wakeup any sleeping threads at the end of program.
int total_pages; // total number of pages for all the files ,required for the compressed output
int q_head = 0; //Circular queue head.
int q_tail = 0; //Circular queue tail.
#define q_capacity 10 //Circular queue capacity.
int q_size = 0; //Circular queue current size
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER, fill = PTHREAD_COND_INITIALIZER;
int* pages_per_file;
int rc=0;
/////////////////////////////////////////////////////////

/////////////////STRUCTURES///////////////////////////////

//Contains the compressed output
struct output {
	char* data;
	int* count;
	int size;
}*out;

//Contains page specific data of a specific file.
struct buffer {
	char* address;      //Mapping address of the file_number file + page_number page
	int file_number;    //File Number
	int page_number;    //Page number
	int last_page_size;      //Page sized or (size_of_file)%page_size
}buf[q_capacity];       //array of size q_capacity contains strucu buffer

////////////////////////////////////////////////////////

/////////////////QUEUE Functions////////////////////////

//buf is the Buffer queue. Queue capacity by default = 10
//Add at q_head index of the circular queue. 
void put(struct buffer b) {
	buf[q_head] = b; //Enqueue the buffer
	q_head = (q_head + 1) % q_capacity;
	q_size++;
}

//Remove from q_tail index of the circular queue.
struct buffer get() {
	struct buffer b = buf[q_tail]; //Dequeue the buffer.
	q_tail = (q_tail + 1) % q_capacity;
	q_size--;
	return b;
}

////////////////////////////////////////////////////////
////////////////////////PRODUCER/////////////////////////
//Producer function to memory map the files passed as arguments.
void* producer(void *arg){
	//Step 1: Get the file.
	char** filenames = (char **)arg;
	struct stat sb;
	char* map; //mmap address
	int file;
	
	//Step 2: Open the file
	for(int i=0;i<num_files;i++){
		//printf("filename %s\n",filenames[i]);
		file = open(filenames[i], O_RDONLY);
		int pages_in_file=0; //Calculates the number of pages in the file. Number of pages = Size of file / Page size.
		int last_page_size=0; //Variable required if the file is not page-alligned ie Size of File % Page size !=0
		
		if(file == -1){ // possible due to file not found?
			printf("Error: File didn't open\n");
			exit(1);
		}

		//Step 3: Get the file info.
		if(fstat(file,&sb) == -1){ 
			close(file);
			printf("Error: Couldn't retrieve file stats");
			exit(1);
		}
		//Empty files - Test 5
        	if(sb.st_size==0){
               		continue;
        	}
		//Step 4: Calculate the number of pages and last page size.
		//st_size contains the size offset. 
		pages_in_file=(sb.st_size/page_size);
		//In case file is not page alligned, we'll need to assign an extra page.
		if(((double)sb.st_size/page_size)>pages_in_file){ 
			pages_in_file+=1;
			last_page_size=sb.st_size%page_size;
		}
		else{ //Page alligned
			last_page_size=page_size;
		}
		total_pages+=pages_in_file;
		pages_per_file[i]=pages_in_file;
		
		//Step 5: Map the entire file.
		map = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, file, 0); 											  
		if (map == MAP_FAILED) { //possibly due to no memory . unneeded unmapp
			close(file);
			printf("Error mmapping the file\n");
			exit(1);
    	}	
    	
		//Step 6: For all the pages in file, create a Buffer type data with the relevant information for the consumer.
		for (int j = 0; j < pages_in_file; j++) {
			pthread_mutex_lock(&lock);            // Critical section so the thread will hold a lock
			while (q_size == q_capacity) {        // Check if the queue is full
				pthread_cond_broadcast(&fill);    // Wake-up all the sleeping consumer threads.
				pthread_cond_wait(&empty, &lock); // Put this thread to sleep and release the lock and  waits for the consumer threads to signal it .
			}
			pthread_mutex_unlock(&lock);          // Release the lock
			struct buffer temp;                   // Contains page specific data of specific file
			if (j == pages_in_file - 1) {         // Last page, might not be page-alligned
				temp.last_page_size = last_page_size;
			}
			else {
				temp.last_page_size = page_size;
			}
			// let the first temp (sized page) address points to the mapped address
			//returned from mmap and then sum the page size to the map  to make the map points to the next page
			temp.address = map;
			temp.page_number = j;
			temp.file_number = i;
			map += page_size; //Go to next page in the memory.
			//possible race condition while changing size.
			pthread_mutex_lock(&lock);     // Hold the lock
			put(temp);
			rc++;
			pthread_mutex_unlock(&lock);   // Release th lock
			pthread_cond_signal(&fill);    // Wake up at least one thread waiting for the buffer to be filled
		}
		//Step 7: Close the file.
		close(file);
	}
	producer_isComplete = 1; //When producer is done mapping.
	// Program wasn't ending during some runtimes as consumer threads were waiting for work.
	pthread_cond_broadcast(&fill); //Wake-up all the sleeping consumer threads.
	return 0;
}
/////////////////////////////////////////////////////////////////////////

///////////////////////////CONSUMER/////////////////////////////////////

//Compresses the buffer object.
struct output RLECompress(struct buffer temp){
	struct output compressed;
	compressed.count=malloc(temp.last_page_size*sizeof(int));// Allocate memory for compressed.count which contain the character frequency
	if (!compressed.count) {
		printf("Error: malloc failed \n");
		exit(1);
	}
	char* tempString=malloc(temp.last_page_size);
	if (!tempString) {
		printf("Error: malloc failed \n");
		exit(1);
	}
	int countIndex=0;
	for(int i=0;i<temp.last_page_size;i++){
		tempString[countIndex]=temp.address[i];
		compressed.count[countIndex]=1;
		while(i+1<temp.last_page_size && temp.address[i]==temp.address[i+1]){
			compressed.count[countIndex]++;
			i++;
		}
		countIndex++;
	}
	compressed.size=countIndex;
	compressed.data=realloc(tempString,countIndex);
	return compressed;
}

//Calculates the relative output position for the buffer object.
int calculateOutputPosition(struct buffer temp){
	int position=0;
	//step 1: Find the relative file position of the buffer object.
	for(int i=0;i<temp.file_number;i++){
		position+=pages_per_file[i];
	}
	//Step 2: Now we're at the index where the particular file begins, we need
	//to find the page relative position
	position+=temp.page_number;
	return position;
}

//Called by consumer threads to start memory compression
//of the files in the queue 'buf'
void *consumer(){
	do{
		pthread_mutex_lock(&lock);
		while(q_size==0 && producer_isComplete ==0){
		    pthread_cond_signal(&empty);     // Wake up the producer 
			pthread_cond_wait(&fill,&lock);  // put the consumer to sleep and release the lock.
		}
		if(producer_isComplete ==1 && q_size==0){ //If producer is done mapping and there's nothing left in the queue.
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		struct buffer temp=get();
		rc--;
		if(producer_isComplete ==0){
		    pthread_cond_signal(&empty);
		}	
		pthread_mutex_unlock(&lock);
		//Output position calculation
		int position=calculateOutputPosition(temp);
		out[position]=RLECompress(temp);
	}while(!(producer_isComplete ==1 && q_size==0));
	return NULL;
}

////////////////////////////////////////////////////////////////////////

///////////////////////////Main/////////////////////////////////////////

//https://piazza.com/class/jcwd4786vss6ky?cid=571
void printOutput(){
	if (rc == 0) {
		char* finalOutput = malloc(total_pages * page_size * (sizeof(int) + sizeof(char)));// pointer points to the compressed file
		if (!finalOutput) {
			printf("Error: malloc failed \n");
			exit(1);
		}
		char* init = finalOutput; //contains the starting point of finalOutput pointer
		for (int i = 0; i < total_pages; i++) {
			if (i < total_pages - 1) {
				if (out[i].data[out[i].size - 1] == out[i + 1].data[0]) { //Compare i'th output's last character with the i+1th output's first character
					out[i + 1].count[0] += out[i].count[out[i].size - 1];
					out[i].size--;
				}
			}

			for (int j = 0; j < out[i].size; j++) {
				int num = out[i].count[j];
				char character = out[i].data[j];
				*((int*)finalOutput) = num;
				finalOutput += sizeof(int);
				*((char*)finalOutput) = character;
				finalOutput += sizeof(char);
			}
		}
		fwrite(init, finalOutput - init, 1, stdout);
	}
	else {
		printf("Error race condition happend \n");
		exit(1);
	}
	
	
}

void freeMemory(){
	free(pages_per_file);
	free(out);

}

int main(int argc, char* argv[]){
	//Check if less than two arguments
	if(argc<2){
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}

	//Initialize all the global Variables.
	page_size = sysconf(_SC_PAGESIZE);
	num_files=argc-1; //Number of files, needed for producer.
	total_threads=get_nprocs(); //Number of processes consumer threads 
	pages_per_file=malloc(sizeof(int)*num_files); //Pages per file.
	if (!pages_per_file) {
		printf("Error: malloc failed \n");
		exit(1);
	}
	
    out=malloc(sizeof(struct output)*( total_pages+1)*page_size); 
	if (!out) {
		printf("Error: malloc failed \n");
		exit(1);
	}
	//Create producer thread to map all the files.
	pthread_t pid,cid[total_threads];
	if (pthread_create(&pid, NULL, producer, argv + 1)!=0) { //argv + 1 to skip argv[0].
		printf("Error: thread create failed \n");
		exit(1);
	}
	

	//Create consumer thread to compress all the pages per file.
	for (int i = 0; i < total_threads; i++) {
		if (pthread_create(&cid[i], NULL, consumer, NULL) != 0) {
			printf("Error: thread create failed \n");
			exit(1);
		}
       
    }

    //Wait for producer-consumers to finish.
    for (int i = 0; i < total_threads; i++) {
        pthread_join(cid[i], NULL);
    }
    pthread_join(pid,NULL);
	printOutput();
	return 0;
	
}
//////////////////////////////////////////////////////////////////////////
