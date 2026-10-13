// Task 4
#include <iostream>
using namespace std;

class WaitingRoom {
    int* q;          // circular array holding patient IDs
    int  cap;
    int  head;       // index of the next patient to serve
    int  count;      // patients currently waiting
    int  nextId;     // next ID to hand out

public:
    explicit WaitingRoom(int capacity = 50) : q(new int[capacity]), cap(capacity), head(0), count(0), nextId(1) {}
    ~WaitingRoom() { delete[] q; }
    WaitingRoom(const WaitingRoom&) = delete;
    WaitingRoom& operator=(const WaitingRoom&) = delete;

    // Assigns the next ID and adds the patient to the back of the queue.
    // Returns the ID, or -1 if the waiting room is full (no ID is consumed).
    int RegisterPatient() {
        if (count == cap) {
            cout << "Waiting room is full (" << cap << " patients). Cannot register.\n";
            return -1;
        }
        int id = nextId++;
        q[(head + count) % cap] = id;
        count++;
        cout << "Patient registered with ID " << id << ".\n";
        return id;
    }

    // Calls the patient at the front (first come, first served).
    // Returns the ID, or -1 if nobody is waiting.
    int ServePatient() {
        if (count == 0) {
            cout << "No patients are waiting.\n";
            return -1;
        }
        int id = q[head];
        head = (head + 1) % cap;
        count--;
        cout << "Now serving patient " << id << ".\n";
        return id;
    }

    // Cancels every waiting appointment (doctor goes to lunch).
    void CancelAll() {
        cout << count << " appointment(s) cancelled.\n";
        head = 0;
        count = 0;                 // nextId is NOT reset -> IDs stay unique
    }

    bool CanDoctorGoHome() const { return count == 0; }

    // Prints waiting IDs in sorted (ascending) order.
    // A FIFO queue of increasing IDs is already sorted; we still sort a copy
    // (insertion sort) so the output is guaranteed sorted.
    void ShowAllPatient() const {
        if (count == 0) { cout << "No patients are waiting.\n"; return; }
        int* tmp = new int[count];
        for (int i = 0; i < count; i++) tmp[i] = q[(head + i) % cap];
        for (int i = 1; i < count; i++) {
            int key = tmp[i], j = i - 1;
            while (j >= 0 && tmp[j] > key) { tmp[j + 1] = tmp[j]; j--; }
            tmp[j + 1] = key;
        }
        cout << "Waiting patients (" << count << "): ";
        for (int i = 0; i < count; i++) cout << tmp[i] << (i + 1 < count ? ", " : "\n");
        delete[] tmp;
    }
};

int main() {
    WaitingRoom room(50);
    int choice;
    do {
        cout << "\n===== Emergency Ward Waiting Room =====\n"
                "1. Register patient\n"
                "2. Serve patient\n"
                "3. Cancel all appointments\n"
                "4. Can doctor go home?\n"
                "5. Show all waiting patients\n"
                "0. Exit\n"
                "Choice: ";
        if (!(cin >> choice)) break;               // EOF / bad input
        switch (choice) {
            case 1: room.RegisterPatient(); break;
            case 2: room.ServePatient(); break;
            case 3: room.CancelAll(); break;
            case 4: cout << (room.CanDoctorGoHome() ? "Yes - nobody is waiting, the doctor can go home.\n"
                                                    : "No - patients are still waiting.\n"); break;
            case 5: room.ShowAllPatient(); break;
            case 0: cout << "Goodbye.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
    return 0;
}