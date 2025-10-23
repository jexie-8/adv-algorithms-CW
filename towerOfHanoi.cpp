#include <iostream>
using namespace std;

void towerOfHanoi(int n, char from_rod, char to_rod, char aux_rod) {
    if (n == 0) return;

    if ((from_rod == 'A' && to_rod == 'C') || (from_rod == 'C' && to_rod == 'A')) {
        towerOfHanoi(n, from_rod, 'B', to_rod);
        towerOfHanoi(n, 'B', to_rod, from_rod);
        return;
    }

    towerOfHanoi(n - 1, from_rod, aux_rod, to_rod);
    cout << from_rod << " " << to_rod << endl;
    cout.flush();
    towerOfHanoi(n - 1, aux_rod, to_rod, from_rod);
}

int main() {
    int N;
    cout << "Enter the number of disks: ";
    cin >> N;

    towerOfHanoi(N, 'A', 'C', 'B');

    return 0;
}
