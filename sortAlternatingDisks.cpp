#include <iostream>
#include <vector> // dynamic array since n is compiled at runtime (user input)
using namespace std;

// & lets the function directly modify the vector (call by reference)
int sortDisks(vector<char>& disks) {

    // passes = how many passes are needed to move all D to R (every disk must move 2n-1 times)
    int passes = disks.size() - 1;
    int moves = 0;

    // does 1 full check of the whole array every pass & allows every D disk to move 1 step right
    for (int i = 0; i < passes; i++) {
        // checks every neighboring pair to 'bubble' the D disks forward 
        for (int j = 0; j < int(disks.size()) - 1; j++) {
            // Swap D & L if D is immediately on the left of an L disk
            if (disks[j] == 'D' && disks[j + 1] == 'L') {
                swap(disks[j], disks[j + 1]);
                moves++;
            }
        }
    }

    return moves;
}

int main() {
    int n;
    cout << "Enter the number of D/L disks (n): ";
    cin >> n;

    // create the vector with the altenrating disks pattern D, L..
    vector<char> disks(n * 2);
    for (int i = 0; i < n*2; i++) {
        disks[i] = (i % 2 == 0) ? 'D' : 'L';
        // current disk = D if i is even, else (i is odd) it is L
    }

    cout << "Initial state: ";
    for (char i : disks) cout << i << " ";
    cout << endl;

    cout << "Number of moves: " << sortDisks(disks) << endl;

    cout << "Goal state: ";
    for (char i : disks) cout << i << " ";
    cout << endl;

    return 0;
}

