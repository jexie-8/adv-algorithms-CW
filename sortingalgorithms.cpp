#include <iostream>
#include <cstdlib> // to use rand (generates random integers)
#include <ctime> // to use srand (sets the seed so the numbers are random every time the program runs)
#include <chrono> //  to use high_resolution_clock (since its the most precise timer) to get timestamps & measure execution time
#include <fstream> //
using namespace std;
using namespace chrono;

/* Task breakdown:
1. For each size from 1-30, generate 3 arrays: 
    - random, 
    - sorted, 
    - and reverse-sorted 
2. For each of those:
    - Makes 3 separate copies 
    - Runs all 3 sorting algorithms (bubble, selection, insertion)
    - Measures and prints (for each):
        - Number of comparisons
        - Execution time in nanoseconds
*/


// 3 sorting algorithms, with comparison counters:
void bubbleSort(int arr[], int n, int& comparisons) {
    comparisons = 0;
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            comparisons++;  
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void selectionSort(int arr[], int n, int& comparisons) {
    comparisons = 0;  
    for (int i = 0; i < n - 1; i++) {
        int min_index = i;
        for (int j = i + 1; j < n; j++) {
            comparisons++; 
            if (arr[j] < arr[min_index]) {
                min_index = j;
            }
        }
        if (min_index != i) {
            swap(arr[min_index], arr[i]);
        }
    }
}

void insertionSort(int arr[], int n, int& comparisons) {
    comparisons = 0;
    int i, key, j;

    for (i = 1; i < n; i++) {  
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            comparisons++;  
            arr[j + 1] = arr[j];
            j = j - 1;
        }

        if (j >= 0) {
            comparisons++;
        }

        arr[j + 1] = key;
    }
}


// array generators:
int* random_arrays(int size) {
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000 + 1; // Values from 1 to 1000
    }
    return arr;
}

int* sorted_arrays(int size) {
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;
    }
    return arr;
}

int* inverse_arrays(int size) {
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = size - i;
    }
    return arr;
}


// helper function to make copies of every array so the sorting algorithms each get a new copy:
int* copy_array(int* source, int size) {
    int* copy = new int[size];
    for (int i = 0; i < size; i++) {
        copy[i] = source[i];
    }
    return copy;
}

// helper function to measure the execution time, in nanoseconds, and read the number of comparisons:
long long time_sort(void (*sortFunc)(int[], int, int&), int arr[], int size, int& comparisons) {
    auto start = high_resolution_clock::now();
    sortFunc(arr, size, comparisons);
    auto end = high_resolution_clock::now();
    return duration_cast<nanoseconds>(end - start).count();
}

// sorting function that uses the copier function, the time sort function, and the sorting functions - for each type of sorting algorithm - then deallocates memory and logs data.
void sort_array(int* baseArray, int size, const string& arrayType, ofstream& compOut, ofstream& timeOut) {
    int* bubble = copy_array(baseArray, size);
    int* selection = copy_array(baseArray, size);
    int* insertion = copy_array(baseArray, size);

    cout << "  " << arrayType << " array of size " << size << ":\n";

    // Bubble Sort
    int bubbleComparisons = 0;
    long long bubbleTime = time_sort(bubbleSort, bubble, size, bubbleComparisons);
    cout << "    Bubble Sort comparisons: " << bubbleComparisons
         << ", time (ns): " << bubbleTime << endl;

    // Selection Sort
    int selectionComparisons = 0;
    long long selectionTime = time_sort(selectionSort, selection, size, selectionComparisons);
    cout << "    Selection Sort comparisons: " << selectionComparisons
         << ", time (ns): " << selectionTime << endl;

    // Insertion Sort
    int insertionComparisons = 0;
    long long insertionTime = time_sort(insertionSort, insertion, size, insertionComparisons);
    cout << "    Insertion Sort comparisons: " << insertionComparisons
         << ", time (ns): " << insertionTime << endl;

    // Write to comparisons CSV
    compOut << size << "," << arrayType << ","
            << bubbleComparisons << "," << selectionComparisons << "," << insertionComparisons << "\n";

    // Write to timing CSV
    timeOut << size << "," << arrayType << ","
            << bubbleTime << "," << selectionTime << "," << insertionTime << "\n";

    delete[] bubble;
    delete[] selection;
    delete[] insertion;
}

// sorting function that generates each type of array to use in each type of sorting algorithm - then deallocates memory:
void run_all_for_n(int size, ofstream& compOut, ofstream& timeOut) {
    int* random = random_arrays(size);
    int* sorted = sorted_arrays(size);
    int* reversed = inverse_arrays(size);

    cout << "Testing arrays of size " << size << "...\n";

    sort_array(random, size, "Random", compOut, timeOut);
    sort_array(sorted, size, "Sorted", compOut, timeOut);
    sort_array(reversed, size, "Reverse", compOut, timeOut);

    delete[] random;
    delete[] sorted;
    delete[] reversed;
}


int main() {

    // PLant a seed
    srand(time(0)); 

    // Create CSV file
    ofstream compOut("comparisons.csv");
    ofstream timeOut("timings.csv");

    // CSV file headers 
    compOut << "Size,Array Type,Bubble,Selection,Insertion\n";
    timeOut << "Size,Array Type,Bubble (ns),Selection (ns),Insertion (ns)\n";

    // Loop 30 times for the "run all for n" function
    for (int size = 1; size <= 30; size++) {
        run_all_for_n(size, compOut, timeOut);
    }

    // Close CSV file
    compOut.close();
    timeOut.close();

    return 0;
}


