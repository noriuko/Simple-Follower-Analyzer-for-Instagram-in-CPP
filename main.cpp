#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream> 
#include <set>

using namespace std;
using json = nlohmann::json;

// --- DATA EXTRACTION LOGIC ---
vector<string> parseJsonFile(string filePath, bool isFollowers) {
    vector<string> usernames;
    try {
        ifstream file(filePath);
        if (!file.is_open()) return usernames;

        json data = json::parse(file);

        if (isFollowers) {
            // Logic for followers_1.json
            for (auto& item : data) {
                if (item.contains("string_list_data") && !item["string_list_data"].empty()) {
                    usernames.push_back(item["string_list_data"][0]["value"]);
                }
            }
        }
        else {
            // Logic for following.json
            auto& followingList = data["relationships_following"];
            for (auto& item : followingList) {
                usernames.push_back(item["title"]);
            }
        }
    }
    catch (json::parse_error& e) {
        cout << "[ERROR] JSON Parse Error: " << e.what() << endl;
    }
    return usernames;
}

// --- PATH & INPUT UTILITIES ---

string cleanPath(string path) {
    // Remove quotes often added by Windows drag-and-drop
    if (!path.empty() && path.front() == '"' && path.back() == '"') {
        path = path.substr(1, path.length() - 2);
    }
    return path;
}

string getFileName(string path) {
    size_t lastSlash = path.find_last_of("\\/");
    return (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);
}

set<string> user_ignoreList() {
    vector<string> usernames;
    string path;
    string line;
    cout << "Drag and drop your ignore.txt list here and press Enter: ";
    getline(cin, path);
    path = cleanPath(path);
    ifstream file(path);
    while (getline(file, line)) {
        usernames.push_back(line);
      }
int size = usernames.size();
cout << "You have " << size << " people in your ignore list!";
return set<string>(usernames.begin(), usernames.end());
}

set<string> acquireDataSet(string targetName, bool isFollowers) {
    string path;
    while (true) {
        cout << "Drag and drop [" << targetName << "] here and press Enter: ";
        getline(cin, path);
        path = cleanPath(path);

        if (getFileName(path) == targetName) {
            vector<string> results = parseJsonFile(path, isFollowers);
            if (!results.empty()) {
                cout << ">> Found " << results.size() << " users.\n" << endl;
                return set<string>(results.begin(), results.end());
            }
            cout << "[ERROR] File was empty or incorrectly formatted." << endl;
        }
        else {
            cout << "[ERROR] Wrong file. Expected: " << targetName << " | Got: " << getFileName(path) << endl;
        }
    }
}

// --- MAIN CONTROL FLOW ---

int main() {
    cout << "--- Instagram Follower Analyzer (C++ Edition) ---\n" << endl;

    // 1. Acquire Data (Stored in main's scope)
    set<string> followerSet = acquireDataSet("followers_1.json", true);
    set<string> followingSet = acquireDataSet("following.json", false);
    set<string> ignroe;
    // 2. Interaction Loop
    while (true) {
        cout << "----------------------------------------" << endl;
        cout << "Choose an option:\n1. Show Non-Followers \n2. Show Fans (You don't follow back)\n3. Turn ignore list on\n4. Exit\nEnter choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            cout << "\n[!] USERS NOT FOLLOWING YOU BACK:" << endl;
            int count = 0;
            for (const string& user : followingSet) {
                if (followerSet.find(user) == followerSet.end() && ignroe.find(user) == ignroe.end()) {
                    cout << " - " << user << endl;
                    count++;
                }
            }
            cout << "Total: " << count << endl;
        }
        else if (choice == "2") {
            cout << "\n[!] FANS (PEOPLE YOU DON'T FOLLOW):" << endl;
            int count = 0;
            for (const string& user : followerSet) {
                if (followingSet.find(user) == followingSet.end()) {
                    cout << " + " << user << endl;
                    count++;
                }
            }
            cout << "Total: " << count << endl;
        }
        else if (choice == "3") {
            ignroe = user_ignoreList();
        }
        else if (choice == "4") {
            cout << "Exiting..." << endl;
            break;
        }
        else {
            cout << "[!] Invalid selection. Try again." << endl;
        }
    }

    return 0;
}
