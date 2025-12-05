/* Shopping optimizer: interactive, user-friendly console program
Input: shopping budget, items they like & their prices. 
Output: min. cost items (vector) so that user can buy max. num. of items. -> maximise items 

1. greedy algorithm (sorts items by cheapest price -> picks cheapest until budget runs out ; maximizes number of items)
2. dynamic programming (value = 1 for all items, maximizes number of items using DP table, backtracks to retrieve chosen items)
3. compare the efficiency and complexity of time and space. */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Struct: used here because each item has 2 variables associated with it & with each other 
// so it groups them into one object instead of using 2 separate arrays for item price and item name.
struct item {
    string name;
    int price;
};

// Helper: comparison function so that the items prices can be sorted from lowest to highest
bool compare(const item &a, const item &b) {
    return a.price < b.price;
}

// 1. Greedy Approach
vector<item> greedy(vector<item> items, int budget) {
    
    // 1. Sort the items by smallest first (first element, last element, comparison)
    sort(items.begin(), items.end(), compare);


    vector<item> chosenItems;        // new vector to add chosen items to 
    int remainingBudget = budget;    // new variable to decrement budget 

    // 2. Loop over all the items starting with the lowest & check if it can be afforded
    for (int i = 0; i < int(items.size()); i++) { 
        if (items[i].price <= remainingBudget) {  
            chosenItems.push_back(items[i]);      // if yes, push onto the vector
            remainingBudget -= items[i].price;    // decrement the budget 
        }
    }

    return chosenItems;
}

// 2. DP Approach (value = 1 for all items)
vector<item> dp(vector<item> items, int budget) {
    

    // 1. create a DP table (vector of vectors / 2D matrix) where:
    // outer vector : the number of rows : items.size() + 1
            // one row for each item, + row 0 for “no items"
    // inner vector : each row has budget + 1 columns
            // columns represent budgets from 0 to budget 'b'
            // initialized with 0 because no items are taken yet, so max count = 0
    // dp[i][b] : cell at row i column b : maximum number of items you can buy using the first i items with budget b
    vector<vector<int>> dp(items.size()+1, vector<int>(budget+1, 0));

    // 2. fill the DP table
    for (int i = 1; i <= int(items.size()); i++) {
        for (int b = 0; b <= budget; b++) {
            if (items[i-1].price > b) { // cannot take this item beause its not <= budget
                dp[i][b] = dp[i-1][b];  // fill the cell with the previous items only
            } else {
                dp[i][b] = max(dp[i-1][b], 1 + dp[i-1][b - items[i-1].price]);
                // either <take previous items> , or take previous items [i-1] AND this item (+ 1) and deduct the price from the budget
                // max : based on the higher number of items
            }
        }
    }

    // 3. backtracking (from last to first item to see which items contributed (like a staircase)) to find which items were chosen 
    vector<item> chosenItems;
    for (int i = items.size(); i > 0; i--) {
        if (budget <= 0) break;                  // stop looping once the budget is used up
        if (dp[i][budget] != dp[i-1][budget]) {  // if this cell & the one before it are equal, the item wasnt included 
            chosenItems.push_back(items[i-1]);   // != -> was included -> add this item to the chosen items vector 
            budget -= items[i-1].price;          // deduct from budget the price of the item & re-iterate 
        }
    }

    return chosenItems;
}

int main() {
    // 1. user input budget
    int budget;
    cout << "Enter the shopping budget: ";
    cin >> budget;
    if (budget < 0) { cout << "Invalid budget"; return 0; }

    vector<item> items;
    cout << "Enter items. Type 'done' when finished." << endl;


    while (true) {
        // 2. user input item name
        string name;
        cout << "Item name: ";
        cin >> name;

        if (name == "done") break;

        // 3. user enter item price
        int price;
        cout << "Item price: ";
        cin >> price;
        if (price < 0) { cout << "Invalid price"; continue; }

        items.push_back({name, price});
    }
    if (items.empty()) { cout << "No items entered."; return 0; }

    // 4. Call both functions for the same list of items & user budget
    vector<item> greedyItems = greedy(items, budget);
    vector<item> dpItems = dp(items, budget);

    // 5. Output the items found by each approach & how many items it collected
    cout << "Results based on greedy approach:" << endl;
    int sumOfGreedy = 0;
    for (int i = 0; i < int(greedyItems.size()); i++) {
        cout << "- " << greedyItems[i].name << " (" << greedyItems[i].price << ")\n";
        sumOfGreedy += greedyItems[i].price;
    }
    cout << "Total items: " << greedyItems.size() << "\n";
    cout << "Total cost: " << sumOfGreedy << "\n";

    

    cout << "Results based on dynamic programming approach: " << endl;
    int sumOfDP = 0;
    for (int i = 0; i < int(dpItems.size()); i++) {
        cout << "- " << dpItems[i].name << " (" << dpItems[i].price << ")\n";
        sumOfDP += dpItems[i].price;
    }
    cout << "Total items: " << dpItems.size() << "\n";
    cout << "Total cost: " << sumOfDP << "\n";


    return 0;
}

