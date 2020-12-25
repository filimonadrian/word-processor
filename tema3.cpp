#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream> 
#include <bits/stdc++.h>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SCIFI 4
#define MAX_THREADS sysconf(_SC_NPROCESSORS_CONF)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


using namespace std;

int P = 2;

struct paragraph {
    std::string type;
    std::string buffer;
} Paragraph;



/* calculate number of threads for workers */ 
int checkNumberOfThreads(int numberOfLines) {

  int nrThreads = numberOfLines / 20;
  P = nrThreads <= MAX_THREADS ? nrThreads : MAX_THREADS;

  return P;
}

/* read text from file */
void *readText(void *arg) {
  int id = *(int *)arg;
  
  pthread_exit(NULL);
}

bool isConsonant(char c) {
    c = tolower(c);
    /* if c is letter and is not vowel */
    return (c >= 'a' && c <= 'z') && 
            !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

/* return new buffer with consonants duplicated */
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
        printf("_%s_\n", token);


        do {
            ++p;
        } while (*p && isDelim(*p));
    }

    free(mutableString);
    return result;
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


void processScifi(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        if ((i + 1) % 6 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        result += words[i];
        result += " ";
    }
}

/* duplicate even position of character */
string duplicateEvenPosition(string buffer) {
    
    // define empty string and construct a new buffer
    string res = "";

    int buffer_len = buffer.length();

    for (int i = 0; i < buffer_len; i++) {
        if (i % 2 == 0) {
            res += buffer[i];
        }
    
        res += buffer[i];
    }

    return res;
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
    // int id = *(int *)arg;
    vector<string> words = *(vector<string>)arg;

    // print vector to test if it works
    for (int i = 0; i < words.size(); i++) {
        cout << words[i];
    }
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

    // How many numbers will be sent.
    int send_numbers = 10;
    int value = 0, ret = 0;

    if (rank == MASTER) {
        

        // Generate the random numbers.
        // Generate the random tags.
        // Sends the numbers with the tags to the second process.
        // Generate a random number.
        for (int i = 0; i < send_numbers; i++) {

            value = 10;

            printf("Process [%d] send %d with tag %d.\n", rank, value, i);
            
            // Sends the value to the MASTER process.
            ret = MPI_Send(&value, 1, MPI_INT, 1, i, MPI_COMM_WORLD);
            if (ret != 0) {
              printf("Can't send data. I am process %d", rank);
            }
        }

    } else if (rank == HORROR) {

        // Receives the information from the first process.
        // Prints the numbers with their corresponding tags.

        MPI_Status status;
        int recv_msg = 0;
        while (recv_msg < send_numbers) {
          ret = MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          
          if (ret != 0) {
            printf("Can't receive data. I am process %d", rank);
          }

          printf("Received from process %d number %d with tag %d\n",
                status.MPI_SOURCE, value, status.MPI_TAG);
          recv_msg++;
        }
    } else if (rank == COMEDY) {
    
    } else if (rank == FANTASY) {

    } else if (rank == SCIFI) {

    }

    MPI_Finalize();

}
