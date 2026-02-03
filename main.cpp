using namespace std;
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream> 
using json = nlohmann::json;

vector<string> followers() {
	fstream file("followers.json");
	json data = json::parse(file);

	vector<string> followers_list;
	for (const auto& follower : data["followers"]) {
		followers_list.push_back(follower.get<string>());
	}
	return followers_list;
}

void add_followers(vector<string>& followers_list, const string& username) {
	followers_list.push_back(username);
}


int main() {
	cout << "Test Follower App Started" << endl;
	vector<string> followers_list = followers();
	cout << "Current Followers:" << endl;
	for (const auto& follower : followers_list) {
		cout << "- " << follower << endl;
	}
}