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

#define BUFMAX 99999999

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
    // string result;
    char result[99999999];
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
    const char* DELIMS = " ";

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

void *processHorrorThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;
    string rez;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
    }

    for (int i = start; i < end; i++) {
        rez += duplicateConsonants(words[i]);
        rez += " ";
    }
    strcpy(args->result, rez.c_str());

	pthread_exit(NULL);
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
    string rez;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < words[i].length(); j++) {
            // cout << (words[i])[j] << " ";
            if ((j + 1) % 2 == 0) {
                rez.push_back(toupper((words[i])[j]));
            } else {
                rez.push_back((words[i])[j]);
            }
        }
        rez.push_back(' ');
        // cout << "\n";
    }

    strcpy(args->result, rez.c_str());
	pthread_exit(NULL);
}

// void processScifi(vector<string> words, string &result) {
//     for (int i = 0; i < words.size(); i++) {
//         if ((i + 1) % 6 == 0) {
//             reverse(words[i].begin(), words[i].end());
//         }
//         result += words[i];
//         result += " ";
//     }
// }

void *processScifiThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;
    string rez;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        if ((i + 1) % 7 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        rez += words[i];
        rez += " ";
    }

    strcpy(args->result, rez.c_str());
	pthread_exit(NULL);
}


// void processFantasy(vector<string> words, string &result) {
//     for (int i = 0; i < words.size(); i++) {
//         if (islower((words[i])[0])) {
//             (words[i])[0] = toupper((words[i])[0]);
//         }
//         result += words[i];
//         result += " ";
//     }
// }


void *processFantasyThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int nr_threads = args->nr_threads;
    int size = args->size;
    string rez;

    int start = id * (double)size / nr_threads;
    int end = MIN((id + 1) * (double)size / nr_threads, size);

    for (int i = start; i < end; i++) {
        if (islower((words[i])[0])) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        rez += words[i];
        rez += " ";
    }

    strcpy(args->result, rez.c_str());
	pthread_exit(NULL);
}

void write_output(vector<paragraph_line> result, string output_file) {
    FILE *f;
    f = fopen(output_file.c_str(), "w");
    if (f == NULL) {
        printf("Error! Can't open this file.\n");
    } else {
        for (int i = 0; i < result.size(); i++) {
            fprintf(f, "%s\n", result[i].data);
        }
    }

    fclose(f);
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
    int nr_paragraps = 0;

    /* buffer reader */
    char line[501];
    fp = fopen("input1.in", "r");

    if (fp == NULL) {
        printf("Error! Can't open this file.\n");
    } else {
        while (!feof(fp)) {
            memset(lines, 0, sizeof(lines));
            fgets(line, sizeof(line) - 1, (FILE*)fp);
            line_NO++;
            
            /* if this is type of paragraph we want to read */
            if (!strncmp(line, args->genre, strlen(genre)) && isNewLineBefore == true) {

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

                        paragraph_size = 0;
                        nr_paragraps++;

                        break;
                    }

                    strcpy(lines[paragraph_size].data, line);
                    lines[paragraph_size].NO = line_NO;
                    isNewLineBefore = false;
                    paragraph_size++;

                    // printf("%s", line);
                }

            }
            if (!strcmp(line, newline)) {
                isNewLineBefore = true;
            } else {
                isNewLineBefore = false;
            }
            
            // printf("%s", line);
            memset(line, 0, sizeof(line));
        }
    }

    /* notify the worker that there's no more data for it */
    send_to_worker = 0;
    MPI_Send(&send_to_worker, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);
    
    args->size = nr_paragraps++;
    free(lines);
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


bool paragraphs_order(const paragraph_line& a, const paragraph_line& b){
    /* smallest comes first */
    return a.NO < b.NO;
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

    vector<paragraph_line> result_horror;
    result_horror.reserve(3000);
    vector<paragraph_line> result_comedy;
    result_comedy.reserve(3000);
    vector<paragraph_line> result_fantasy;
    result_fantasy.reserve(3000);
    vector<paragraph_line> result_scifi;
    result_scifi.reserve(3000);

    int result_horror_size = 0;
    int result_comedy_size = 0;
    int result_fantasy_size = 0;
    int result_scifi_size = 0;

    vector<string> words;
    t_arguments *thread_args = (t_arguments*) malloc(MAX_THREADS * sizeof(t_arguments));
    read_arguments *read_args;
    pthread_t threads[MAX_THREADS];
    string result;
    int r = 0;
    void *thread_status;
    MPI_Status mpi_status;
    char line[600];

    paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));
    // paragraph_line *result_paragraphs = (paragraph_line *) malloc (2900 * sizeof(paragraph_line));
    vector<paragraph_line> result_paragraphs;
    result_paragraphs.reserve(3000);
    int result_paragraphs_size = 0;

    int paragraph_size = 0, nr_threads = 0;


    if (rank == MASTER) {
        int nr_paragraphs = 0;
        pthread_t threads[MASTER_THREADS];
        read_args = (read_arguments*) malloc(MASTER_THREADS * sizeof(read_arguments));

        for (int i = 0; i < MASTER_THREADS; i++) {
            read_args[i].id = i;

            switch (i) {
                case (HORROR_THREAD):
                    strcpy(read_args[i].genre, HORROR_NAME);
                    break;
                case (COMEDY_THREAD):
                    strcpy(read_args[i].genre, COMEDY_NAME); 
                    break;
                case (FANTASY_THREAD): 
                    strcpy(read_args[i].genre, FANTASY_NAME);
                    break;
                case (SCIFI_THREAD): 
                    strcpy(read_args[i].genre, SCIFI_NAME);
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
       
        /* read_args->size is used as total number of paragraphs send to every worker */
        for (int i = 0; i < MASTER_THREADS; i++) {
            nr_paragraphs += read_args->size;
        }

        free(read_args);

        /* receive size of the paragraph and the paragraph itself from workers */

        for (int i = 0; i < MASTER_THREADS; i++) {
            switch (i) {
                case (HORROR_THREAD):
                    MPI_Recv(&result_horror_size, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD, &mpi_status);
                    MPI_Recv(&result_horror[0], result_horror_size * sizeof(struct line), MPI_BYTE, HORROR, 0, MPI_COMM_WORLD, &mpi_status);
                    break;
                case (COMEDY_THREAD):
                    MPI_Recv(&result_comedy_size, 1, MPI_INT, COMEDY, 0, MPI_COMM_WORLD, &mpi_status);
                    MPI_Recv(&result_comedy[0], result_comedy_size * sizeof(struct line), MPI_BYTE, COMEDY, 0, MPI_COMM_WORLD, &mpi_status); 
                    break;
                case (FANTASY_THREAD): 
                    MPI_Recv(&result_fantasy_size, 1, MPI_INT, FANTASY, 0, MPI_COMM_WORLD, &mpi_status);
                    MPI_Recv(&result_fantasy[0], result_fantasy_size * sizeof(struct line), MPI_BYTE, FANTASY, 0, MPI_COMM_WORLD, &mpi_status);
                    break;
                case (SCIFI_THREAD): 
                    MPI_Recv(&result_scifi_size, 1, MPI_INT, SCIFI, 0, MPI_COMM_WORLD, &mpi_status);
                    MPI_Recv(&result_scifi[0], result_scifi_size * sizeof(struct line), MPI_BYTE, SCIFI, 0, MPI_COMM_WORLD, &mpi_status);
                    break;
            }    
        }

        vector<paragraph_line> final_result;
        for (int i = 0; i < result_horror_size; i++) {
            final_result.push_back(result_horror[i]);
        }
        for (int i = 0; i < result_comedy_size; i++) {
            final_result.push_back(result_comedy[i]);
        }
        for (int i = 0; i < result_fantasy_size; i++) {
            final_result.push_back(result_fantasy[i]);
        }
        for (int i = 0; i < result_scifi_size; i++) {
            final_result.push_back(result_scifi[i]);
        }

        // final_result.insert(final_result.end(), result_horror.begin(), result_horror.end());
        // final_result.insert(final_result.end(), result_comedy.begin(), result_comedy.end());
        // final_result.insert(final_result.end(), result_fantasy.begin(), result_fantasy.end());
        // final_result.insert(final_result.end(), result_scifi.begin(), result_scifi.end());
        
        sort(final_result.begin(), final_result.end(), paragraphs_order);

        // for (int i = 0; i < nr_paragraphs - 1; i++) {
        //     cout << final_result[i].data << endl;
        // }

        /* write result */
        write_output(final_result, "output.out");

    } else if (rank == HORROR) {

        int receive_flag;
        MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        while (receive_flag) {
            
            memset(lines, 0, sizeof(lines));

            /* last thread receives data from master */
            /**************************/

            read_args = (read_arguments*) malloc(sizeof(read_arguments));
            read_args[0].id = 0;
            read_args[0].lines = &lines[0]; 
            r = pthread_create(&threads[MAX_THREADS - 1], NULL, receiveFromMaster, &read_args[0]);

            pthread_join(threads[MAX_THREADS - 1], &thread_status);
            /***********************/
            
            paragraph_size = read_args->size;

            /* calculate number of threads */
            nr_threads = checkNumberOfThreads(paragraph_size);
            int proc_threads = nr_threads - 1;
            
            /**OTHER THREADSs*********************/

            /* tokenize string  and send to processing threads */
            string paragraph;
            for (int i = 0; i < paragraph_size; i++) {
                paragraph += lines[i].data;
            }

            tokenize(paragraph, words);
            
            /* create the threads and separate the work for every thread */
            for (int i = 0; i < proc_threads; i++) {

                thread_args[i].id = i;
                thread_args[i].nr_threads = proc_threads;
                thread_args[i].words = &words[0]; 
                thread_args[i].size = words.size();

                r = pthread_create(&threads[i], NULL, processHorrorThreads, &thread_args[i]);

                if (r) {
                    printf("Can't create thread %d!\n", i);
                    exit(1);
                }
            }

            /* wait threads to finish processing */
            for (int i = 0; i < proc_threads; i++) {
                r = pthread_join(threads[i], &thread_status);
                if (r) {
                    printf("Can't wait thread %d!\n", i);
                    exit(1);
                }
	        }
            
            result += HORROR_NAME;
            result += "\n";
            /* compose the modified paragraph */
            for (int i = 0; i < proc_threads; i++) {
                string rez = thread_args[i].result;
                result += rez;
            }

            // cout << result << "\n";

            /* keep processed data to send data back to master for writing in file */
            strcpy(result_paragraphs[result_paragraphs_size].data, result.c_str());
            result_paragraphs[result_paragraphs_size].NO = lines[0].NO;
            result_paragraphs_size++;
            // cout << result_paragraphs[result_paragraphs_size - 1].data;
            // cout << result_paragraphs[result_paragraphs_size - 1].NO << endl;
            /**OTHER THREADSs*********************/

            
            /* print lines that i've received */
            for (int i = 0; i < paragraph_size; i++) {
                // printf("%s", lines[i].data);
            }
            
            /* receive signal for reading from master */
            MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
            result.clear();
            memset(thread_args, 0, MAX_THREADS * sizeof(t_arguments));
            memset(read_args, 0, sizeof(read_arguments));
            words.clear();
        }

        // sort(result_paragraphs.begin(), result_paragraphs.end(), paragraphs_order);
        /* these were all paragraphs. Now send to master processed text */
        MPI_Send(&result_paragraphs_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&result_paragraphs[0], result_paragraphs_size * sizeof(paragraph_line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD);

    } else if (rank == COMEDY) {
        int receive_flag;
        MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        while (receive_flag) {
            
            memset(lines, 0, sizeof(lines));

            /* last thread receives data from master */
            /**************************/

            read_args = (read_arguments*) malloc(sizeof(read_arguments));
            read_args[0].id = 0;
            read_args[0].lines = &lines[0]; 
            r = pthread_create(&threads[MAX_THREADS - 1], NULL, receiveFromMaster, &read_args[0]);

            pthread_join(threads[MAX_THREADS - 1], &thread_status);
            /***********************/
            
            paragraph_size = read_args->size;

            /* calculate number of threads */
            nr_threads = checkNumberOfThreads(paragraph_size);
            int proc_threads = nr_threads - 1;
            
            /**OTHER THREADSs*********************/

            /* tokenize string  and send to processing threads */
            string paragraph;
            for (int i = 0; i < paragraph_size; i++) {
                paragraph += lines[i].data;
            }

            tokenize(paragraph, words);
            
            /* create the threads and separate the work for every thread */
            for (int i = 0; i < proc_threads; i++) {

                thread_args[i].id = i;
                thread_args[i].nr_threads = proc_threads;
                thread_args[i].words = &words[0]; 
                thread_args[i].size = words.size();

                r = pthread_create(&threads[i], NULL, processComedyThreads, &thread_args[i]);

                if (r) {
                    printf("Can't create thread %d!\n", i);
                    exit(1);
                }
            }

            /* wait threads to finish processing */
            for (int i = 0; i < proc_threads; i++) {
                r = pthread_join(threads[i], &thread_status);
                if (r) {
                    printf("Can't wait thread %d!\n", i);
                    exit(1);
                }
	        }
            
            result += COMEDY_NAME;
            result += "\n";
            /* compose the modified paragraph */
            for (int i = 0; i < proc_threads; i++) {
                string rez = thread_args[i].result;
                result += rez;
            }

            // cout << result << "\n";

            /* keep processed data to send data back to master for writing in file */
            strcpy(result_paragraphs[result_paragraphs_size].data, result.c_str());
            result_paragraphs[result_paragraphs_size].NO = lines[0].NO;
            result_paragraphs_size++;
            // cout << result_paragraphs[result_paragraphs_size - 1].data;
            // cout << result_paragraphs[result_paragraphs_size - 1].NO << endl;
            /**OTHER THREADSs*********************/

            
            /* print lines that i've received */
            for (int i = 0; i < paragraph_size; i++) {
                // printf("%s", lines[i].data);
            }
            
            /* receive signal for reading from master */
            MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
            result.clear();
            memset(thread_args, 0, MAX_THREADS * sizeof(t_arguments));
            memset(read_args, 0, sizeof(read_arguments));
            words.clear();
        }
        // sort(result_paragraphs.begin(), result_paragraphs.end(), paragraphs_order);
        /* these were all paragraphs. Now send to master processed text */
        MPI_Send(&result_paragraphs_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&result_paragraphs[0], result_paragraphs_size * sizeof(paragraph_line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD);

    } else if (rank == FANTASY) {
        int receive_flag;
        MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        while (receive_flag) {
            
            memset(lines, 0, sizeof(lines));

            /* last thread receives data from master */
            /**************************/

            read_args = (read_arguments*) malloc(sizeof(read_arguments));
            read_args[0].id = 0;
            read_args[0].lines = &lines[0]; 
            r = pthread_create(&threads[MAX_THREADS - 1], NULL, receiveFromMaster, &read_args[0]);

            pthread_join(threads[MAX_THREADS - 1], &thread_status);
            /***********************/
            
            paragraph_size = read_args->size;

            /* calculate number of threads */
            nr_threads = checkNumberOfThreads(paragraph_size);
            int proc_threads = nr_threads - 1;
            
            /**OTHER THREADSs*********************/

            /* tokenize string  and send to processing threads */
            string paragraph;
            for (int i = 0; i < paragraph_size; i++) {
                paragraph += lines[i].data;
            }

            tokenize(paragraph, words);
            
            /* create the threads and separate the work for every thread */
            for (int i = 0; i < proc_threads; i++) {

                thread_args[i].id = i;
                thread_args[i].nr_threads = proc_threads;
                thread_args[i].words = &words[0]; 
                thread_args[i].size = words.size();

                r = pthread_create(&threads[i], NULL, processFantasyThreads, &thread_args[i]);

                if (r) {
                    printf("Can't create thread %d!\n", i);
                    exit(1);
                }
            }

            /* wait threads to finish processing */
            for (int i = 0; i < proc_threads; i++) {
                r = pthread_join(threads[i], &thread_status);
                if (r) {
                    printf("Can't wait thread %d!\n", i);
                    exit(1);
                }
	        }
            
            result += FANTASY_NAME;
            result += "\n";
            /* compose the modified paragraph */
            for (int i = 0; i < proc_threads; i++) {
                string rez = thread_args[i].result;
                result += rez;
            }

            // cout << result << "\n";

            /* keep processed data to send data back to master for writing in file */
            strcpy(result_paragraphs[result_paragraphs_size].data, result.c_str());
            result_paragraphs[result_paragraphs_size].NO = lines[0].NO;
            result_paragraphs_size++;
            // cout << result_paragraphs[result_paragraphs_size - 1].data;
            // cout << result_paragraphs[result_paragraphs_size - 1].NO << endl;
            /**OTHER THREADSs*********************/

            
            /* print lines that i've received */
            for (int i = 0; i < paragraph_size; i++) {
                // printf("%s", lines[i].data);
            }
            
            /* receive signal for reading from master */
            MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
            result.clear();
            memset(thread_args, 0, MAX_THREADS * sizeof(t_arguments));
            memset(read_args, 0, sizeof(read_arguments));
            words.clear();
        }
        // sort(result_paragraphs.begin(), result_paragraphs.end(), paragraphs_order);
        /* these were all paragraphs. Now send to master processed text */
        MPI_Send(&result_paragraphs_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&result_paragraphs[0], result_paragraphs_size * sizeof(paragraph_line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD);

    } else if (rank == SCIFI) {
        int receive_flag;
        MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        while (receive_flag) {
            
            memset(lines, 0, sizeof(lines));

            /* last thread receives data from master */
            /**************************/

            read_args = (read_arguments*) malloc(sizeof(read_arguments));
            read_args[0].id = 0;
            read_args[0].lines = &lines[0]; 
            r = pthread_create(&threads[MAX_THREADS - 1], NULL, receiveFromMaster, &read_args[0]);

            pthread_join(threads[MAX_THREADS - 1], &thread_status);
            /***********************/
            
            paragraph_size = read_args->size;

            /* calculate number of threads */
            nr_threads = checkNumberOfThreads(paragraph_size);
            int proc_threads = nr_threads - 1;
            
            /**OTHER THREADSs*********************/

            /* tokenize string  and send to processing threads */
            string paragraph;
            for (int i = 0; i < paragraph_size; i++) {
                paragraph += lines[i].data;
            }

            tokenize(paragraph, words);
            
            /* create the threads and separate the work for every thread */
            for (int i = 0; i < proc_threads; i++) {

                thread_args[i].id = i;
                thread_args[i].nr_threads = proc_threads;
                thread_args[i].words = &words[0]; 
                thread_args[i].size = words.size();

                r = pthread_create(&threads[i], NULL, processScifiThreads, &thread_args[i]);

                if (r) {
                    printf("Can't create thread %d!\n", i);
                    exit(1);
                }
            }

            /* wait threads to finish processing */
            for (int i = 0; i < proc_threads; i++) {
                r = pthread_join(threads[i], &thread_status);
                if (r) {
                    printf("Can't wait thread %d!\n", i);
                    exit(1);
                }
	        }
            
            result += SCIFI_NAME;
            result += "\n";
            /* compose the modified paragraph */
            for (int i = 0; i < proc_threads; i++) {
                string rez = thread_args[i].result;
                result += rez;
            }

            // cout << result << "\n";

            /* keep processed data to send data back to master for writing in file */
            strcpy(result_paragraphs[result_paragraphs_size].data, result.c_str());
            result_paragraphs[result_paragraphs_size].NO = lines[0].NO;
            result_paragraphs_size++;
            // cout << result_paragraphs[result_paragraphs_size - 1].data;
            // cout << result_paragraphs[result_paragraphs_size - 1].NO << endl;
            /**OTHER THREADSs*********************/

            
            /* print lines that i've received */
            for (int i = 0; i < paragraph_size; i++) {
                // printf("%s", lines[i].data);
            }
            
            /* receive signal for reading from master */
            MPI_Recv(&receive_flag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
            result.clear();
            memset(thread_args, 0, MAX_THREADS * sizeof(t_arguments));
            memset(read_args, 0, sizeof(read_arguments));
            words.clear();
        }
        // sort(result_paragraphs.begin(), result_paragraphs.end(), paragraphs_order);
        /* these were all paragraphs. Now send to master processed text */
        MPI_Send(&result_paragraphs_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&result_paragraphs[0], result_paragraphs_size * sizeof(paragraph_line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}
