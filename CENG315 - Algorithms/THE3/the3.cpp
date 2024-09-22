#include "the3.h"
#define MAX3(a, b, c) ((a >= b) ? ((a >= c) ? a : c) : ((b >= c) ? b : c))
// do not add extra libraries, but you can define helper functions below.

int stack_limit = 0;

int C(char a, char b, int mismatch, int match){
    if (a == b){
        return match;
    }
    else{
        return -mismatch;
    }
}

int backtrack(std::string sequence_A, std::string sequence_B, int ** memoize, int mismatch, int match, int gap, std::string &possible_alignment){
    int A_length = sequence_A.length();
    int B_length = sequence_B.length();
    int i = A_length;
    int j = B_length;
    std::string aligned_A;
    std::string aligned_B;

    while (i > 0 || j > 0){
        if(i > 0 && j > 0 && memoize[i][j] == (memoize[i - 1][j - 1] + C(sequence_A[i - 1], sequence_B[j - 1], mismatch, match))){
            aligned_A = sequence_A[i - 1] + aligned_A;
            aligned_B = sequence_B[j - 1] + aligned_B;
            i--;
            j--;
        }
        else if (i > 0 && memoize[i][j] == memoize[i - 1][j] - gap){
            aligned_A = sequence_A[i - 1] + aligned_A;
            aligned_B = '.' + aligned_B;
            i--;
        }
        else{
            aligned_A = '_' + aligned_A;
            aligned_B = sequence_B[j - 1] + aligned_B;
            j--;
        }
    }


    int total_length = aligned_A.length();

    possible_alignment.resize(total_length);
    for(int i = 0; i < total_length; i++){
        if(aligned_A[i] == '_'){
            possible_alignment[i] = '_';
        }
        else if(aligned_B[i] == '.'){
            possible_alignment[i] = '.';
        }
        else if (aligned_A[i] == aligned_B[i]){
            possible_alignment[i] = aligned_A[i];
        }
        else{
            possible_alignment[i] = '!';
        }
    }

    return 1;
}

int F(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, int i, int j, int** mem, int* call_count){
    if ((*call_count) >= 1000000){
        stack_limit = 1;
        i = -1;
        j = -1;
        return 0;
    }
    if (i < 0) {
        mem[i + 1][j + 1] = -(j + 1) * gap;
        (*call_count)++;
        return -((j + 1) * gap);
    }
    if (j < 0) {
        mem[i + 1][j + 1] = -(i + 1) * gap;
        (*call_count)++;
        return -((i + 1) * gap);
    }

    else{

        int max_score = MAX3(F(sequence_A, sequence_B, gap, mismatch, match, i - 1, j - 1, mem, call_count) + C(sequence_A[i], sequence_B[j], mismatch, match),
                              F(sequence_A, sequence_B, gap, mismatch, match, i - 1, j, mem, call_count) - gap,
                              F(sequence_A, sequence_B, gap, mismatch, match, i, j - 1, mem, call_count) - gap);

        mem[i + 1][j + 1] = max_score;
        (*call_count)++;
        return max_score;
    }
}

/*
PART 1
you are expected to call recursive_alignment (as the name suggests) recursively to find an alignment.
initial call_count value given to you will be 0.
you should check if call_count >= 1000000, if so, set possible_alignment string to "STACK LIMIT REACHED", return INT_MIN (or anything - it will not be checked).
*/
int recursive_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment, int call_count){
    int highest_alignment_score;
    int A_length = sequence_A.length();
    int B_length = sequence_B.length();

    int** memoize = new int*[A_length + 1];

    for (int i = 0; i < A_length + 1; i++){
        memoize[i] = new int[B_length + 1];
        for(int j = 0; j < B_length + 1; j++){
            memoize[i][j] = -1;
        }
    }

    call_count = 0;
    highest_alignment_score = F(sequence_A, sequence_B, gap, mismatch, match, A_length - 1, B_length - 1, memoize, &call_count);
    if (stack_limit){
        possible_alignment = "STACK LIMIT REACHED";
        return INT_MIN;
    }

    backtrack(sequence_A, sequence_B, memoize, mismatch, match, gap, possible_alignment);
    
    for (int i = 0; i < A_length + 1; i++) {
        delete[] memoize[i];
    }

    delete[] memoize;

    return highest_alignment_score;
}

/*
PART 2
you are expected to create a dynamic programming table to find the highest alignment score.
then you will need to reconstruct a possible alignment string from the table.
*/
int dp_table_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment){
    int highest_alignment_score;

    int A_length = sequence_A.length();
    int B_length = sequence_B.length();

    int** memoize = new int*[A_length + 1];

    for (int i = 0; i < A_length + 1; i++){
        memoize[i] = new int[B_length + 1];
        for(int j = 0; j < B_length + 1; j++){
            if(i == 0){
                memoize[i][j] = -(j * gap);
            }
            else if(j == 0){
                memoize[i][j] = -(i * gap);
            }
            else{
                memoize[i][j] = 0;
            }
        }
    }

    for (int i = 1; i < A_length + 1; i++){
        for(int j = 1; j < B_length + 1; j++){
            int upper = memoize[i - 1][j] - gap;
            int left = memoize[i][j - 1] - gap;
            int diagonal = 0;
            if(sequence_A[i - 1] == sequence_B[j - 1]){
                diagonal = memoize[i - 1][j - 1] + match;
            }
            else{
                diagonal = memoize[i - 1][j - 1] - mismatch;
            }

            memoize[i][j] = MAX3(upper, left, diagonal);
        }
    }

    highest_alignment_score = memoize[A_length][B_length];

    backtrack(sequence_A, sequence_B, memoize, mismatch, match, gap, possible_alignment);
    
    for (int i = 0; i < A_length + 1; i++) {
        delete[] memoize[i];
    }

    delete[] memoize;

    return highest_alignment_score;
}
