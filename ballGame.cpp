/* Problem definition & breakdown:
Ball sorting game: Moving stacked balls one by one such that balls of the same color are stacked on each other.
INPUT: the number of stacks, number of balls in each stack, number of colors, and number of empty stacks
OUTPUT: step-by-step moves to reach the final goal.
VALID MOVE: only move the top ball from stack A (!= empty) to stack B (!= full && empty OR top ball color = moving ball)
GOAL STACK: if stack is only 1 color OR empty
METHOD: DFS SEARCH: 
    1. generate all possible valid moves from state A (sourse state)
    2. try out each move -> find state B (destination states)
    3. if state B visited, skip
    4. if state B not visited -> recurse
    5. if dead-end -> backtrack (return to previous & check other routes)

LIST to TUPLE:
    - You need visited to avoid loops.
    - visited must be a set.
    - A set only accepts hashable objects.
    - Lists are not hashable → error.
    - Tuples are hashable → works.
    - Therefore you convert your state (list of lists) into a tuple of tuples.
*/

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <random>
using namespace std;

// automatic setup using user parameters
void setupBalls(vector<vector<int>>& state, int numStacks, int ballsPerStack, int numColors, int numEmpty) {
    
    if (numStacks <= 0 || ballsPerStack <= 0 || numColors <= 0 || numEmpty < 0 || numEmpty > numStacks) {
        cout << "Invalid input." << endl;
        exit(1);
    }

    // make numStacks empty elements inside the vector to be filled 
    state.resize(numStacks);

    // create balls
    int totalBalls = numColors * ballsPerStack;
    vector<int> balls;
  

    for (int color = 1; color <= numColors; color++) { // loops over every color
        for (int i = 0; i < ballsPerStack; i++) {      // loops over every ball of that color
            balls.push_back(color);                    // adds every ball to the balls vector
        }
    }

    // create random seed, setup random num generator with seed for reproducability, then shuffle balls)
    random_device rd;
    mt19937 g(42);
    shuffle(balls.begin(), balls.end(), g);

    // fill non empty stacks
    int ballIndex = 0;
    for (int i = 0; i < numStacks - numEmpty ; i++) { // loops over every stack
        for (int j = 0; j < ballsPerStack; j++) {     // loops over how many balls are supposed to be in every stack
            state[i].push_back(balls[ballIndex++]);   // puts the balls in the stack & increments which ball
        }
    }


}

// convert the non-hashable vector to a hashable set of strings so it can be accessed later
string encodeState(const vector<vector<int>>& state) {
    
    string result = "";

    for (int i = 0; i < int(state.size()); i++) { // loops over every stack
        result += "[";  // [] separating each group of balls within a stack

        for (int j = 0; j < int(state[i].size()); j++) { // loops over every ball in the current stack
            result += to_string(state[i][j]) + ","; // appends the ball to the string separated by commas
        }

        result += "]";
    }

    return result;
}

// check if the current state is a goal state (each stack is either empty OR has balls of only one color AND is full)
bool goalState(const vector<vector<int>>& state, int ballsPerStack) {
    for (int i = 0; i < int(state.size()); i++) {

        // 1. stack is empty
        if (state[i].empty()) continue; 

        // 2. stacks must be full if they are not empty
        if (int(state[i].size()) < ballsPerStack) return false;   

        int stackColor = state[i][0];

        for (int j = 0; j < int(state[i].size()); j++) {
            // if the colors are mixed
            if (state[i][j] != stackColor) {
                return false; 
            }
        }
    }

    return true;
}

// validate moves to ensure proper pruning & reduce inefficient moves
bool validMove(const vector<vector<int>>& state, int from, int to, int maxHeight, const vector<pair<int,int>>& solutionMoves) {
    if (state[from].empty()) return false;                // cant move from an empty stack
    if (from == to) return false;                         // cant move to the same stack
    if (int(state[to].size()) >= maxHeight) return false; // cant move to a full stack
    
    // 4. destination isnt empty & top color is not matching
    if (!state[to].empty() && state[to].back() != state[from].back()) return false;
    
    
    // 5. do not undo a move (A→B , B→A)
    if (!solutionMoves.empty()) {
        pair<int,int> lastMove = solutionMoves.back();
        // if past source = new destination and new destination = past source
        if (lastMove.first == to && lastMove.second == from)
            return false;
    }

    // 6. if the stack is solved (full with 1 color) dont move the balls
    if (int(state[from].size()) == maxHeight) { //the stack is full
        for (int i = 1; i < int(state[from].size()); i++) { 
            if (state[from][i] != state[from][0]) // ball[i] doesnt match ball[0]
                goto notSolved; // the stack isnt solved, allow the move, skip to notSolved
        }
        return false; // the stack is solved, dont allow the move (its invalid)
    notSolved:;
    }

    return true; // passed all the checks.
}

// move ball from source (pop) to destination (push)
void applyMove(vector<vector<int>>& state, int from, int to) {
    int topBall = state[from].back(); 
    state[from].pop_back();
    state[to].push_back(topBall);
}

// undo a move where a dead end was reached
void undoMove(vector<vector<int>>& state, int from, int to) {
    int topBall = state[to].back();
    // pop from destination & push to source
    state[to].pop_back();  
    state[from].push_back(topBall);
}

// Depth-First Search:
bool DFS(vector<vector<int>>& state, unordered_set<string>& visited, vector<pair<int,int>>& solutionMoves, int ballsPerStack) {
    // Base case - keep checking if the game is solved
    if (goalState(state, ballsPerStack)) return true;

    // Encode the current state & append to visited so that it isn't checked again
    string currentState = encodeState(state);
    visited.insert(currentState);

    // Outer loop for source stacks & inner loop for destination stacks (try every possible pair)
    for (int i = 0; i < int(state.size()); i++) {
        for (int j = 0; j < int(state.size()); j++) {

            // if the move is invalid, skip it
            if (!validMove(state, i, j, ballsPerStack, solutionMoves)) continue;

            // if its valid, apply the move, and encode the new game state
            applyMove(state, i, j);
            string newState = encodeState(state);

            // .count() returns how many times the key is found in an unordered set
            // if the new state hasnt been visited, append it to the solution moves
            if (visited.count(newState) == 0) {
                solutionMoves.push_back(make_pair(i, j));

                // recursively call DFS from the new state, return true as soon as a goal state is found
                if (DFS(state, visited, solutionMoves, ballsPerStack)) return true;

                // if this path didn't find a solution, start undoing / backtracking
                solutionMoves.pop_back(); // delete from solution moves
            }

            undoMove(state, i, j); // undo from actual game state
        }
    }

    return false; // all moves from this state have failed -> backtrack further
}

int main() {

    int numOfStacks, ballsPerStack, numOfColors, numOfEmptyStacks;
    cin >> numOfStacks >> ballsPerStack >> numOfColors >> numOfEmptyStacks;

    vector<vector<int>> initialState(numOfStacks);
    setupBalls(initialState, numOfStacks, ballsPerStack, numOfColors, numOfEmptyStacks);

    unordered_set<string> visited;
    vector<pair<int, int>> moves;

    // print initial state
    cout << "STACKS:\n" << flush;
    for (int i = 0; i < int(initialState.size()); i++) {
        for (int j = 0; j < int(initialState[i].size()); j++) {
            cout << initialState[i][j] << " " << flush;
        }
        cout << "\n"; 
    }

    bool solved = DFS(initialState, visited, moves, ballsPerStack);

    if (solved) {
        // print moves
        cout << "MOVES:\n" << flush;
        for (int i = 0; i < int(moves.size()); i++) {
            cout << moves[i].first << " " << moves[i].second << "\n" << flush;
        }
    } else {
        cout << "No solution could be found." << endl << flush;
    }

    return 0;
}
