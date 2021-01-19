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

#define MASTER_THREADS 4

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define BUFMAX 500

const char COMEDY_NAME[7] = "comedy";
const char HORROR_NAME[7] = "horror";
const char FANTASY_NAME[8] = "fantasy";
const char SCIFI_NAME[16] = "science-fiction";


using namespace std;

int P = 2;

pthread_barrier_t barrier;

/* this is a line from a paragraph */
typedef struct line {
    int NO;
    string data;
} line;

/* thread arguments */
typedef struct t_arguments {
    int id; 
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

  int nrThreads = numberOfLines / 20;
  P = nrThreads <= MAX_THREADS ? nrThreads : MAX_THREADS;

  return P;
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
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

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
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

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
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

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
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

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
    line *lines = args->lines;
    char genre[20];
    strcpy(genre, args->genre);
    bool isNewLineBefore = true;

    // int size = args->size;
    int size = 0;
    int line_NO = 0;

    // buffer which keeps the data
    char line[501];
    fp = fopen("input1.in", "r");

    if (fp == NULL) {
        printf("Error! Can't open this file.\n");
    } else {
        while (!feof(fp)) {
            fgets(line, 500, (FILE*)fp);
            line_NO++;

            
            // if this is type of paragraph we need to read
            if (!strncmp(line, genre, 6) && isNewLineBefore == true) {
                // read the entire paragraph, keep a vector of lines(strings)

                // memset(line, 0, sizeof(line));
                // while paragraph is not finished
                while (strcmp(line, "\n") && !feof(fp)) {
                    memset(line, 0, sizeof(line));
                    fgets(line, 500, (FILE*)fp);
                    line_NO++;

                    if (!strcmp(line, "\n")) {
                        isNewLineBefore = true;
                        break;
                    }
                    lines[size].data = line;
                    lines[size].NO = line_NO;
                    isNewLineBefore = false;

                    size++;

                    // printf("%s", line);
                }

            }
            // printf("%s", line);
            memset(line, 0, sizeof(line));
        }
    }
    args->size = size;
    fclose(fp);
    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {

    int  numTasks, rank;

    MPI_Init(&argc, &argv);
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
    string result;
    int r = 0;
    void *status;


    if (rank == MASTER) {
        pthread_t threads[MASTER_THREADS];
        read_args = (read_arguments*) malloc(MASTER_THREADS * sizeof(read_arguments));

        for (int i = 0; i < MASTER_THREADS; i++) {
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

            read_args[i].id = i;
            // arguments[i].lines = &lines[0]; 
            // arguments[i].size = lines.size();

            r = pthread_create(&threads[i], NULL, read_file, &read_args[i]);

            if (r) {
                printf("Can't create thread %d!\n", i);
                exit(-1);
            }
        }

        for (int i = 0; i < P; i++) {
            r = pthread_join(threads[i], &status);
            if (r) {
                printf("Can't wait thread %d!\n", i);
                exit(-1);
            }
	    }

        free(read_args);

    } else if (rank == HORROR) {

    } else if (rank == COMEDY) {
    
    } else if (rank == FANTASY) {

    } else if (rank == SCIFI) {

    }

    MPI_Finalize();

}
