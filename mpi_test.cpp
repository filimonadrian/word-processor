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
const char newline[2] = "\n";

using namespace std;

int P = 2;

pthread_barrier_t barrier;

/* this is a line from a paragraph */
typedef struct line {
    // string data;
    char data[100];
    int NO;
} paragraph_line;

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
    // line *lines = args->lines;
    paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));
    char genre[20];
    strcpy(genre, args->genre);
    bool isNewLineBefore = true;

    int ret = 0;
    int size = 0;
    int line_NO = 0;

    /* buffer which keeps the data */
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
                while (strcmp(line, newline) && !feof(fp)) {
                    memset(line, 0, sizeof(line));
                    fgets(line, 500, (FILE*)fp);
                    line_NO++;

                    /* send line and line_NO */
                    int len = strlen(line);
                    MPI_Send(&len, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD);
                    // cout << line;
                    MPI_Send(line, strlen(line), MPI_CHAR, HORROR, 0, MPI_COMM_WORLD);

                    /* the end of the paragraph */
                    if (!strcmp(line, newline)) {
                        isNewLineBefore = true;
                        // for (int i = 0; i < 3; i++) {
                        //     cout << lines[i].data << "\n";
                        // }
                        // ret = MPI_Send(&size, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD);
                        // ret = MPI_Send(lines, 3 * sizeof(struct line), MPI_BYTE, HORROR, 0, MPI_COMM_WORLD);

                        /* send paragraph to the worker
                        * reset paragraph 
                        */

                        break;
                    }

                    // lines[size].data = line;
                    // strcpy(lines[size].data, line);
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

    int  numTasks, rank, ret = 0;

    // MPI_Init(&argc, &argv);
    int provided;
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

    vector<paragraph_line> lines(2600);


    vector<string> words;
    t_arguments *thread_args;
    read_arguments *read_args;
    string result;
    int r = 0;
    void *thread_status;
    MPI_Status mpi_status;
    char line[600];

    if (rank == MASTER) {
        FILE *fp;
        char genre[20];
        strcpy(genre, "horror");
        bool isNewLineBefore = true;

        int ret = 0;
        int line_NO = 0;
        int size = 0;

        /* buffer which keeps the data */
        char line[501];
        paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));
        // paragraph_line lines[100];

        fp = fopen("input1.in", "r");

        if (fp == NULL) {
            printf("Error! Can't open this file.\n");
        } else {
            while (!feof(fp)) {
                fgets(line, 500, (FILE*)fp);
                line_NO++;
                
                // if this is type of paragraph we need to read
                if (!strncmp(line, genre, 6) && isNewLineBefore == true) {

                    // while paragraph is not finished
                    while (strcmp(line, newline) && !feof(fp)) {
                        memset(line, 0, sizeof(line));
                        fgets(line, 500, (FILE*)fp);
                        line_NO++;

                        /* send line and line_NO */
                        // int len = strlen(line);
                        // MPI_Send(&len, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD);
                        // // cout << line;
                        // MPI_Send(line, strlen(line), MPI_CHAR, HORROR, 0, MPI_COMM_WORLD);

                        /* the end of the paragraph */
                        if (!strcmp(line, newline)) {
                            isNewLineBefore = true;
                            // for (int i = 0; i < 3; i++) {
                            //     cout << lines[i].data << "\n";
                            // }
                            printf("%s\n", lines[0].data);
                            MPI_Send(&size, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD);
                            MPI_Send(lines, size * sizeof(struct line), MPI_BYTE, HORROR, 0, MPI_COMM_WORLD);

                            /* send paragraph to the worker
                            * reset paragraph 
                            */

                            break;
                        }

                        // lines[size].data = line;
                        strcpy(lines[size].data, line);
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
        fclose(fp);

    } else if (rank == HORROR) {
        paragraph_line *lines = (paragraph_line* ) malloc(2600 * sizeof(paragraph_line));
        // paragraph_line lines[100];
        int paragraph_size = 0;
        int len = 0;
        ret = MPI_Recv(&paragraph_size, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
        ret = MPI_Recv(lines, paragraph_size * sizeof(struct line), MPI_BYTE, MASTER, 0, MPI_COMM_WORLD, &mpi_status);

        // ret = MPI_Recv(&len, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
        // ret = MPI_Recv(line, 500, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &mpi_status);
        // printf ("size: %d line: %s\n", len, line);
        // memset(line, 0, 300);

        printf ("Par_size: %d\n", paragraph_size);
        for (int i = 0; i < paragraph_size; i++) {
            printf("%s\n", lines[i].data);
        }
    } else if (rank == COMEDY) {
    
    } else if (rank == FANTASY) {

    } else if (rank == SCIFI) {

    }

    MPI_Finalize();

}
