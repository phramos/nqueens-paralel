#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define QUEEN 'Q'
#define TRUE 1
#define FALSE 0
#define NOT_SET -1

int queens = 0;
int threads = 0;
int boardSize = 0;
int queensPerThread = 0;
int solutioNumber = 0;
int *solution;
//char *board;


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
int isSafePosition(int newQueenCol, int newQueenRow, int board[]) {
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

//resolve o problema das n rainhas
void solve(int col) {
    int row = 0;
    for (row=0; row<boardSize; row++){
        int safe = isSafePosition(col, row, solution);
        if (safe == TRUE) {
            solution[col] = row;
            int nextCol = col+1;
            if (nextCol<boardSize) {
                solve(nextCol);
            } else {
                solutioNumber ++;
                printSolution(solution, solutioNumber);
            }
        }
    }
}

int main() {
    printf("Number of queens: ");
    scanf("%d", &queens);
    printf("Threads: ");
    scanf("%d", &threads);

    //define a dimensao da matriz de acordo com a quantidade de rainhas
    boardSize = queens;
    queensPerThread = queens/threads;

    printf("\nQueens: %d", queens);
    printf("\nThreads: %d", threads);
    solution=malloc(boardSize * sizeof(int));
    int i = 0;

    //Define a solucao como nao definida para todas as colunas
    for (i = 0; i < boardSize; ++i) {
        solution[i] = NOT_SET;
    }

    solve(0);
    printf("\nTotal solutions: %d", solutioNumber);
    return FALSE;
}


