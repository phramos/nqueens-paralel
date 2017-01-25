#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define QUEEN 'Q'
#define TRUE 1
#define FALSE 666
#define NOT_SET -1

int numberOfQueens = 0;
int numberOfThreads = 0;
int boardSize = 0;
int solutioNumber = 0;
int *solution;
pthread_t* threads;//char *board;

struct timespec begin, end;

struct safePositionThreadParams {
    int i;
    int newQueenCol;
    int newQueenRow;
    int *board;
};

/*imprime a solucao. Formato da solucao e um array em que cada posicao do array indica a uma coluna no tabuleiro, e o
 * valor nessa coluna indica a linha onde a rainha esta
*/
void printSolution(int solution[], int solutionNumber) {
    int i = 0;
    int j = 0;

    printf("\n Solution number: %d", solutionNumber);
    printf("\n");
    for (i=0; i<boardSize; i++) {
        for (j=0; j<boardSize; j++) {
            if (j == solution[i]) {
                printf("%c ", QUEEN);
            } else {
                printf("%c ", '-');
            }
        }
        printf("\n");
    }
}

/*
 * Verifica se eh seguro colocar uma nova rainha na coluna definida na devida linha
 */
int isSafePositionSerial(int newQueenCol, int newQueenRow, int *board) {
    int i = 0;
    for (i=0; i<newQueenCol; i++) {
        if (board[i] == newQueenRow) {
            return FALSE;
        } else {
            //checa as diagonais
            int difference = abs(newQueenCol-i);
            int notSafe1 = board[i]-difference;
            int notSafe2 = board[i]+difference;
            if (newQueenRow == notSafe1 || newQueenRow == notSafe2) {
                return FALSE;
            }
        }
    }
    return TRUE;
}


void *isSafe(void *args) {
    struct safePositionThreadParams *threadParams = args;
//    printf("\ni=%d",threadParams->i);
//    printf("\nrol=%d",threadParams->newQueenCol);
//    printSolution(threadParams->board, 1);
    int safe = FALSE;
    if (threadParams->board[threadParams->i] == threadParams->newQueenRow) {
        safe = FALSE;
    } else {
        //checa as diagonais
        int difference = abs(threadParams->newQueenCol-threadParams->i);
        int notSafe1 = threadParams->board[threadParams->i]-difference;
        int notSafe2 = threadParams->board[threadParams->i]+difference;
        if (threadParams->newQueenRow == notSafe1 || threadParams->newQueenRow == notSafe2) {
            safe = FALSE;
        } else {
            safe = TRUE;
        }
    }
    free(args);
    return (void*) safe;
}


int isSafePositionParalel(int newQueenCol, int newQueenRow, int *board) {
    int i = 0;

    //parte que divide a quantidade de threads para poder paralelizar a verificacao de posicao segura para a nova rainha
    if (newQueenCol > numberOfThreads) {
        int colsProcessed = 0;
//        while (colsProcessed <= newQueenCol) {
        while (colsProcessed < newQueenCol) {
            int threadsToOpen;
            //verifica a quantidade de threads a serem abertas para processamento
            if (numberOfThreads > (newQueenCol) - colsProcessed) {
                threadsToOpen = newQueenCol-colsProcessed;
            } else {
                threadsToOpen = numberOfThreads;
            }

            threads = malloc(threadsToOpen * sizeof(pthread_t));
            int *returnFromIsSafe = malloc(threadsToOpen * sizeof(int));
            for (i=0; i<threadsToOpen; i++) {
                struct safePositionThreadParams *args = malloc(sizeof(struct safePositionThreadParams));
                args->i = i + colsProcessed;
                args->newQueenCol = newQueenCol;
                args->newQueenRow = newQueenRow;
                args->board = board;

                pthread_create(&threads[i], NULL, isSafe, (void *)args);

            }

            for(i = 0; i < threadsToOpen; ++i){
                pthread_join(threads[i], (void**) &returnFromIsSafe[i]);
            }
            for (i = 0; i < threadsToOpen; i++) {
//                printf("\nretorno: %d", returnFromIsSafe[i]);
                if (returnFromIsSafe[i] == FALSE){
                    free(threads);
                    free(returnFromIsSafe);
                    return FALSE;
                }
            }
            free(threads);
            free(returnFromIsSafe);
            colsProcessed += threadsToOpen;
        }
    } else {
        threads = malloc(newQueenCol * sizeof(pthread_t));
        int *returnFromIsSafe = malloc(newQueenCol * sizeof(int));
        for (i=0; i<newQueenCol; i++) {
            struct safePositionThreadParams *args = malloc(sizeof(struct safePositionThreadParams));
            args->i = i;
            args->newQueenCol = newQueenCol;
            args->newQueenRow = newQueenRow;
            args->board = board;

            pthread_create(&threads[i], NULL, isSafe, (void *)args);

        }
        for(i = 0; i < newQueenCol; ++i)
            pthread_join(threads[i], (void**) &returnFromIsSafe[i]);
        for (i = 0; i < newQueenCol; i++) {
//            printf("\nretorno: %d", returnFromIsSafe[i]);
            if (returnFromIsSafe[i] == FALSE) {
                free(threads);
                free(returnFromIsSafe);
                return FALSE;
            }
        }
        free(threads);
        free(returnFromIsSafe);
    }
    return TRUE;
}

//resolve o problema das n rainhas serialmente
void serialSolver(int col) {
    int row = 0;
    for (row=0; row<boardSize; row++){
        int safe = isSafePositionSerial(col, row, solution);
        if (safe == TRUE) {
            solution[col] = row;
            int nextCol = col+1;
            if (nextCol<boardSize) {
                serialSolver(nextCol);
            } else {
                solutioNumber ++;
                printSolution(solution, solutioNumber);
            }
        }
    }
}
//resolve o problema das n rainhas pralelamente
void paralelSolver(int col) {
    int row = 0;
    for (row=0; row<boardSize; row++){
        int safe = isSafePositionParalel(col, row, solution);
        if (safe == TRUE) {
            solution[col] = row;
            int nextCol = col+1;
            if (nextCol<boardSize) {
                paralelSolver(nextCol);
            } else {
                solutioNumber ++;
                printSolution(solution, solutioNumber);
            }
        }
    }
}

int main() {
    printf("Number of queens: ");
    scanf("%d", &numberOfQueens);
    printf("Threads: ");
    scanf("%d", &numberOfThreads);

    //define a dimensao da matriz de acordo com a quantidade de rainhas
    boardSize = numberOfQueens;

    printf("\nQueens: %d", numberOfQueens);
    printf("\nThreads: %d", numberOfThreads);
    solution=malloc(boardSize * sizeof(int));
    int i = 0;

    //Define a solucao como nao definida para todas as colunas
    for (i = 0; i < boardSize; ++i) {
        solution[i] = NOT_SET;
    }
    clock_gettime(CLOCK_MONOTONIC, &begin);

    if (numberOfThreads == 0) {
        serialSolver(0);
    }else {
        paralelSolver(0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double timeSpent = end.tv_sec - begin.tv_sec;
    timeSpent += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    printf("\nTotal solutions: %d", solutioNumber);
    printf("\nTime spent: %.10lf seconds.\n", timeSpent);
    free(solution);
    return 0;
}


