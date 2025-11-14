#include <iostream>
#include <string>
using namespace std;


// Sort the words in ascending order
void bubbleSort(string &word) {
    int n = word.length();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (word[j] > word[j + 1]) {
                swap(word[j], word[j + 1]);
            }
        }
    }
}


string anagramChecker (string wordOne, string wordTwo) {
    
    // If they are not the same length, break early
    if (wordOne.length() != wordTwo.length()){
        return "The words are not anagrams"; }

    // Match each letter in both strings
    for (int i = 0 ; i < (int)wordOne.length() ; i++ ){
        if (wordOne[i] != wordTwo[i]) {
            return "The words are not anagrams"; }
    }

    return "The two words are anagrams";
};


int main () {

    string wordOne = "ejwwfe";
    string wordTwo = "fejwwe";
    bubbleSort(wordOne);
    bubbleSort(wordTwo);
    cout << anagramChecker(wordOne, wordTwo) << endl;

}