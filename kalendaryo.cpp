#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>  // For file operations
#include <iomanip>  // For formatting
using namespace std;

// Event structure
struct Event {
    string name;
    string description;
    int date; // Format: YYYYMMDD
    int priority;

    Event(string n, string d, int dt, int p) : name(n), description(d), date(dt), priority(p) {}
};

// Format date as YYYY/MM/DD
string formatDate(int date) {
    int year = date / 10000;
    int month = (date / 100) % 100;
    int day = date % 100;

    char buffer[11];
    sprintf(buffer, "%04d/%02d/%02d", year, month, day);
    return string(buffer);
}

// Save events to file
void saveEventsToFile(const vector<Event>& events) {
    ofstream file("events.txt");
    for (const auto& event : events) {
        file << event.name << "|" << event.description << "|" << event.date << "|" << event.priority << "\n";
    }
    file.close();
}

// Segment Tree for managing events by date
class SegmentTree {
    vector<vector<Event>> tree;
    int size;

    void buildTree(vector<Event>& events, int node, int start, int end) {
        if (start == end) {
            tree[node] = {events[start]};
        } else {
            int mid = (start + end) / 2;
            buildTree(events, 2 * node + 1, start, mid);
            buildTree(events, 2 * node + 2, mid + 1, end);
            merge(tree[2 * node + 1].begin(), tree[2 * node + 1].end(),
                  tree[2 * node + 2].begin(), tree[2 * node + 2].end(),
                  back_inserter(tree[node]),
                  [](Event a, Event b) { return a.date < b.date; });
        }
    }

public:
    SegmentTree(vector<Event>& events) {
        size = events.size();
        tree.resize(4 * size);
        buildTree(events, 0, 0, size - 1);
    }

    vector<Event> query(int node, int start, int end, int l, int r) {
        if (r < start || l > end) return {};
        if (l <= start && r >= end) return tree[node];
        int mid = (start + end) / 2;
        auto left = query(2 * node + 1, start, mid, l, r);
        auto right = query(2 * node + 2, mid + 1, end, l, r);
        vector<Event> result;
        merge(left.begin(), left.end(), right.begin(), right.end(), back_inserter(result),
              [](Event a, Event b) { return a.date < b.date; });
        return result;
    }

    vector<Event> query(int l, int r) {
        return query(0, 0, size - 1, l, r);
    }
};

// Binary Search
Event* binarySearch(vector<Event>& events, int date) {
    int low = 0, high = events.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (events[mid].date == date) return &events[mid];
        else if (events[mid].date < date) low = mid + 1;
        else high = mid - 1;
    }
    return nullptr;
}

// Linear Search
vector<Event> linearSearch(vector<Event>& events, const string& keyword) {
    vector<Event> result;
    for (auto& event : events) {
        if (event.name.find(keyword) != string::npos || event.description.find(keyword) != string::npos) {
            result.push_back(event);
        }
    }
    return result;
}

// Merge Sort by Priority
void merge(vector<Event>& events, int left, int mid, int right) {
    vector<Event> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (events[i].priority < events[j].priority) {
            temp[k++] = events[i++];
        } else {
            temp[k++] = events[j++];
        }
    }

    while (i <= mid) temp[k++] = events[i++];
    while (j <= right) temp[k++] = events[j++];

    for (int p = 0; p < k; ++p) {
        events[left + p] = temp[p];
    }
}

void mergeSortByPriority(vector<Event>& events, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSortByPriority(events, left, mid);
    mergeSortByPriority(events, mid + 1, right);
    merge(events, left, mid, right);
}

// CLI Interface
void displayMenu() {
    cout << "\n🏫 School Event Calendar Organizer\n";
    cout << "1️⃣  Add Event\n";
    cout << "2️⃣  View Events\n";
    cout << "3️⃣  🔍 Search Event by Date\n";
    cout << "4️⃣  🔎 Search Event by Keyword\n";
    cout << "5️⃣  🚪 Exit\n";
    cout << "6️⃣  Sort Events by Priority\n";
    cout << "Enter your choice: ";
}

int main() {
    vector<Event> events;
    SegmentTree* segmentTree = nullptr;

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (choice == 1) {
            string name, description;
            int date, priority;
            cout << "📝 Enter event name: ";
            cin.ignore();
            getline(cin, name);
            cout << "📝 Enter event description: ";
            getline(cin, description);
            cout << "📅 Enter event date (YYYYMMDD): ";
            cin >> date;
            cout << "⭐ Enter event priority (1-10): ";
            cin >> priority;

            events.emplace_back(name, description, date, priority);
            cout << "✅ Event added successfully!\n";

            // Save to file and rebuild segment tree
            saveEventsToFile(events);
            delete segmentTree;
            segmentTree = new SegmentTree(events);
        }
        else if (choice == 2) {
            cout << "\n📋 Events List:\n";
            for (const auto& event : events) {
                cout << "📌 Name: " << event.name
                     << ", 📅 Date: " << formatDate(event.date)
                     << ", ⭐ Priority: " << event.priority << "\n";
            }
        }
        else if (choice == 3) {
            int date;
            cout << "📅 Enter date (YYYYMMDD): ";
            cin >> date;
            sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
                return a.date < b.date;
            });
            Event* event = binarySearch(events, date);
            if (event) {
                cout << "📌 Name: " << event->name
                     << ", 📝 Description: " << event->description
                     << ", 📅 Date: " << formatDate(event->date)
                     << ", ⭐ Priority: " << event->priority << "\n";
            } else {
                cout << "❌ No event found on this date.\n";
            }
        }
        else if (choice == 4) {
            string keyword;
            cout << "🔎 Enter keyword: ";
            cin.ignore();
            getline(cin, keyword);
            auto results = linearSearch(events, keyword);
            if (results.empty()) {
                cout << "❌ No matching events found.\n";
            } else {
                for (const auto& event : results) {
                    cout << "📌 Name: " << event.name
                         << ", 📅 Date: " << formatDate(event.date)
                         << ", ⭐ Priority: " << event.priority << "\n";
                }
            }
        }
        else if (choice == 5) {
            cout << "👋 Goodbye!\n";
            break;
        }
        else if (choice == 6) {
            if (events.empty()) {
                cout << "⚠️ No events to sort.\n";
            } else {
                mergeSortByPriority(events, 0, events.size() - 1);
                cout << "✅ Events sorted by priority!\n";
            }
        }
        else {
            cout << "❌ Invalid choice. Try again.\n";
        }
    }

    delete segmentTree;
    return 0;
}