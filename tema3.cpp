#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <iostream> 
#include <cstring>
#include <vector>
#include <bits/stdc++.h>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SCIFI 4
#define HORROR_THREAD 0
#define COMEDY_THREAD 1
#define FANTASY_THREAD 2
#define SCIFI_THREAD 3

#define MAX_THREADS sysconf(_SC_NPROCESSORS_CONF)
#define MIN_THREADS 2

#define MAX_LINES_THREAD 20
#define MASTER_THREADS 4

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define BUFMAX 500

const char COMEDY_NAME[7] = "comedy";
const char HORROR_NAME[7] = "horror";
const char FANTASY_NAME[8] = "fantasy";
const char SCIFI_NAME[16] = "science-fiction";
const char newline[2] = "\n";

using namespace std;

pthread_barrier_t barrier;

/* this is a line from a paragraph */
typedef struct __attribute__((__packed__)) line {
    // string data;
    char data[500];
    int NO;
} paragraph_line;

/* thread arguments */
typedef struct t_arguments {
    int id;
    int nr_threads;
    /* the result of a thread */
    string result;
    /* splitted sentence and size */
    string *words;
    int size;

} t_arguments;
/* arguments for thread function that read the file */
typedef struct read_arguments {
    int id;
    /* vector of paragraph lines and the size */
    line *lines;
    int size;
    char genre[20];

} read_arguments;

/* calculate number of threads for workers */ 
int checkNumberOfThreads(int numberOfLines) {

    /* at least 1 thread for reading and 1 for processing*/
    int nr_threads = 1;
    
    int proc = ceil((double)numberOfLines / MAX_LINES_THREAD);
    if (!proc) {
        return MIN_THREADS;
    }

    nr_threads += proc;
    nr_threads = nr_threads <= MAX_THREADS ? nr_threads : MAX_THREADS;

  return nr_threads;
}

// http://www.joshbarczak.com/blog/?p=970
// manual solution works faster
static bool isDelim(char c) {
    const char* DELIMS = " \n\t";

    do {
        if (c == *DELIMS) {
            return true;
        }
        ++DELIMS;
    } while (*DELIMS);

    return false;
}

vector<string> tokenize(std::string &str, vector<string> &result) {
    char *mutableString = (char*) malloc (str.size() + 1);
    strcpy(mutableString, str.c_str());

    char *p = mutableString;
    
    while (*p && isDelim(*p)) {
        ++p;
    }

    while (*p) {
        
        // start of token
        char* token = p;

        do {
            ++p;
        } while (!isDelim(*p) && *p);

        *p = 0;
       // cout << token;
        result.push_back(token);
        // printf("_%s_\n", token);

        do {
            ++p;
        } while (*p && isDelim(*p));
    }

    free(mutableString);
    return result;
}

bool isConsonant(char c) {
    c = tolower(c);
    /* if c is letter and is not vowel */
    return (c >= 'a' && c <= 'z') && 
            !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

string duplicateConsonants(string buffer) {
    
    // define empty string and construct a new buffer
    string res = "";

    int buffer_len = buffer.length();

    for (int i = 0; i < buffer_len; i++) {
        res += buffer[i];

        if (isConsonant(buffer[i])) {
            res += tolower(buffer[i]);
        }
    }

    return res;
}


void processComedyString(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        for (int j = 0; j < words[i].length(); j++) {
            // cout << (words[i])[j] << " ";
            result.push_back((words[i])[j]);
            if ((j + 1) % 2 == 0) {
                result.push_back((words[i])[j]);
            }
        }
        result.push_back(' ');
        // cout << "\n";
    }
}

void *processHorrorThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        args->result += duplicateConsonants(words[i]);
        args->result += " ";
    }

	pthread_exit(NULL);
}


void processComedy(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        for (int j = 0; j < words[i].length(); j++) {
            result.push_back((words[i])[j]);
            if ((j + 1) % 2 == 0) {
                result.push_back((words[i])[j]);
            }
        }
        result.push_back(' ');
        // cout << "\n";
    }
}

void *processComedyThreads(void *arg) {
    
    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < words[i].length(); j++) {
            // cout << (words[i])[j] << " ";
            if ((j + 1) % 2 == 0) {
                args->result.push_back(toupper((words[i])[j]));
            } else {
                args->result.push_back((words[i])[j]);
            }
        }
        args->result.push_back(' ');
        // cout << "\n";
    }

	pthread_exit(NULL);
}

void processScifi(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        if ((i + 1) % 6 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        result += words[i];
        result += " ";
    }
}

void *processScifiThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        if ((i + 1) % 7 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        args->result += words[i];
        args->result += " ";
    }

	pthread_exit(NULL);
}


void processFantasy(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        if (islower((words[i])[0])) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        result += words[i];
        result += " ";
    }
}


void *processFantasyThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        if (islower((words[i])[0])) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        args->result += words[i];
        args->result += " ";
    }

	pthread_exit(NULL);
}
/* read file  */
void *read_file(void *arg) {
    FILE *fp;

    read_arguments *args = (read_arguments*)(arg);
    int id = args->id;
    paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));
    char genre[20];
    strcpy(genre, args->genre);
    bool isNewLineBefore = true;

    int ret = 0;
    int paragraph_size = 0;
    int line_NO = 0;
    int send_to_worker;

    /* buffer reader */
    char line[501];
    fp = fopen("input1.in", "r");

    if (fp == NULL) {
        printf("Error! Can't open this file.\n");
    } else {
        while (!feof(fp)) {
            fgets(line, sizeof(line) - 1, (FILE*)fp);
            line_NO++;
            
            /* if this is type of paragraph we need to read */
            if (!strncmp(line, args->genre, strlen(genre)) && isNewLineBefore == true) {
                // read the entire paragraph, keep a vector of lines

                // while paragraph is not finished
                while (strcmp(line, newline) && !feof(fp)) {

                    /* read lines from the paragraph */
                    memset(line, 0, sizeof(line));
                    fgets(line, sizeof(line) - 1, (FILE*)fp);
                    line_NO++;

                    /* the end of the paragraph */
                    if (!strcmp(line, newline)) {

                        /* notify the worker that it must receive more data */
                        send_to_worker = 1;
                        MPI_Send(&send_to_worker, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);

                        isNewLineBefore = true;

                        /* send paragraph to workers and reset paragraph */
                        ret = MPI_Send(&paragraph_size, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);
                        ret = MPI_Send(lines, paragraph_size * sizeof(struct line), MPI_BYTE, id + 1, 0, MPI_COMM_WORLD);
                        memset(lines, 0, sizeof(lines));

                        break;
                    }

                    strcpy(lines[paragraph_size].data, line);
                    lines[paragraph_size].NO = line_NO;
                    isNewLineBefore = false;
                    paragraph_size++;

                    // printf("%s", line);
                }

            }
            // printf("%s", line);
            memset(line, 0, sizeof(line));
        }
    }

    /* notify the worker that there's no more data for it */
    send_to_worker = 0;
    MPI_Send(&send_to_worker, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);
    
    args->size = paragraph_size;
    fclose(fp);
    pthread_exit(NULL);
}

void *receiveFromMaster(void *arg) {
    read_arguments *args = (read_arguments*)(arg);
    int paragraph_size = 0;
    MPI_Status mpi_status;
    
    /* receive size of the paragraph and the paragraph itself */
    MPI_Recv(&paragraph_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
    MPI_Recv(args->lines, paragraph_size * sizeof(struct line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

    args->size = paragraph_size;
    
    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {

    int  numTasks, rank, provided, ret = 0;

    // MPI_Init(&argc, &argv);

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Checks the number of processes allowed.
    if (numTasks != 5) {
        printf("Wrong number of processes. Only 5 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    vector<line> horror_lines(2600);
    vector<line> comedy_lines(2600);
    vector<line> fantasy_lines(2600);
    vector<line> scifi_lines(2600);

    vector<string> words;
    t_arguments *thread_args;
    read_arguments *read_args;
    pthread_t threads[MAX_THREADS];
    string result;
    int r = 0;
    void *thread_status;
    MPI_Status mpi_status;
    char line[600];

    paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));

    int paragraph_size = 0, nr_threads = 0;

    if (rank == MASTER) {
        pthread_t threads[MASTER_THREADS];
        read_args = (read_arguments*) malloc(MASTER_THREADS * sizeof(read_arguments));

        for (int i = 0; i < MASTER_THREADS; i++) {
            read_args[i].id = i;

            switch (i) {
                
                /* genre - 1 because threads start from 0
                * but processes starts from 1
                */
                case (HORROR_THREAD):
                    strcpy(read_args[i].genre, HORROR_NAME);
                    /* get the ref of std::vector as an simple array */ 
                    read_args[i].lines = &horror_lines[0];
                    break;
                case (COMEDY_THREAD):
                    strcpy(read_args[i].genre, COMEDY_NAME);
                    read_args[i].lines = &comedy_lines[0]; 
                    break;
                case (FANTASY_THREAD): 
                    strcpy(read_args[i].genre, FANTASY_NAME);
                    read_args[i].lines = &fantasy_lines[0]; 
                    break;
                case (SCIFI_THREAD): 
                    strcpy(read_args[i].genre, SCIFI_NAME);
                    read_args[i].lines = &scifi_lines[0]; 
                    break;
            }

            r = pthread_create(&threads[i], NULL, read_file, &read_args[i]);

            if (r) {
                printf("Can't create thread %d!\n", i);
                exit(1);
            }
        }

        for (int i = 0; i < MASTER_THREADS; i++) {
            r = pthread_join(threads[i], &thread_status);
            if (r) {
                printf("Can't wait thread %d!\n", i);
                exit(1);
            }
	    }

        free(read_args);

        /* receive paragraphs from workers */

    } else if (rank == HORROR) {

        int receive_flag;
        MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        while (receive_flag) {
        
            memset(lines, 0, sizeof(lines));
            

            /* first thread receives data from master */
            /**************************/
            /*
            exit and RELEASE BARRIER 
            */
            read_args = (read_arguments*) malloc(sizeof(read_arguments));
            read_args[0].id = 0;
            read_args[0].lines = &lines[0]; 
            r = pthread_create(&threads[0], NULL, receiveFromMaster, &read_args[0]);

            pthread_join(threads[0], &thread_status);
            /***********************/
            
            paragraph_size = read_args->size;

            /* calculate number of threads */
            nr_threads = checkNumberOfThreads(paragraph_size);
            
            
            /**OTHER THREADSs*********************/



            /**OTHER THREADSs*********************/

            
            /* print lines that i've received */
            for (int i = 0; i < paragraph_size; i++) {
                printf("%s", lines[i].data);
            }
            
            /* receive signal for reading from master */
            MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        }

    } else if (rank == COMEDY) {
    
    } else if (rank == FANTASY) {

    } else if (rank == SCIFI) {

    }

    MPI_Finalize();

}
