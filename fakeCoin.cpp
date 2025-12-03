#include <iostream>
#include <vector>
#include <string>
using namespace std;

// two pan balance scale, as the problem states: returns -1 if left pan lighter, 0 if balanced, 1 if left pan heavier
int weigh(int left, int right) {
    if (left == right) return 0;
    return (left > right ? 1 : -1);
}

string threeCoins(const vector<int>& coins) {
    int a = coins[0], b = coins[1], c = coins[2];

    // compare a and b
    int balanceOne = weigh(a, b);

    // if a and b are equal, the genuine weight is a, and the fake is c OR none found
    if (balanceOne == 0) {  
        int balanceTwo = weigh(c, a);
        if (balanceTwo == 0) return "No fake coin found";          
        return "Fake coin is " + string(balanceTwo > 0 ? "HEAVIER" : "LIGHTER");
    } 
    // if a and b are not equal, one of them is the fake, so compare to c 
    else {  
        int balanceTwo = weigh(c, a);
        // if c is the same as a, b is the fake
        if (balanceTwo == 0) {
            return "Fake coin is " + string(balanceOne > 0 ? "HEAVIER" : "LIGHTER");
        } else {
            // if c is not the same as a, a is the fake (lighter and heavier are switched because a was on the left pan)
            return "Fake coin is " + string(balanceOne > 0 ? "LIGHTER" : "HEAVIER");
        }
    }
}

string fakeFinder(const vector<int>& coins) {

    int a = coins[0], b = coins[1], c = coins[2];
    int rcw;  // real coin weight

    // 1. Find a real coin from first 3 coins using scale function
    int balanceOne = weigh(a, b);
    if (balanceOne == 0) {
        rcw = a;    // a and b equal, so rcw is a
    } else {
        int balanceTwo = weigh(a, c);  // assuming b is fake, compare a and c
        if (balanceTwo == 0) {
            rcw = a;  // a and c are equal, so a is genuine
        } else {
            int balanceThree = weigh(b, c);  // a was not equal to b or c, so check b with c
            if (balanceThree == 0) {
                rcw = b;  // b is genuine
            } else {
                return "There can only be one fake coin."; // all three different
            }
        }
    }


    int fcw = -1;       
    int countDiff = 0;  

    // 2. Compare remaining coins to genuine coin using scale
    for (int i = 3; i < (int)coins.size(); i++) {
        int res = weigh(coins[i], rcw);
        if (res != 0) {
            countDiff++;
            if (countDiff > 1) return "There can only be one fake coin.";  // multiple fakes
            fcw = i;  // store index of fake coin
        }
    }

    // 3. Check if no fake was found
    if (countDiff == 0) return "No fake coin found.";

    // 4. Determine heavier or lighter
    int finalResult = weigh(coins[fcw], rcw);
    return "Fake coin is " + string(finalResult > 0 ? "HEAVIER" : "LIGHTER");
}


int main() {
    int n;
    cout << "Enter number of coins: ";
    cin >> n;

    if (n < 3) {
            cout << "Must have more than 2 coins.";
            return 0;
    }

    vector<int> coins(n);
    cout << "Enter weights: ";
    for (int i = 0; i < n; i++) cin >> coins[i];

    if (n==3) cout << threeCoins(coins) << endl;
    else cout << fakeFinder(coins) << endl;
}

