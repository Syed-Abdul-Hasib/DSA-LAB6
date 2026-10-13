// Task 3: Print Server Request Handling System
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

struct PrintRequest {
    int id;
    int arrival;       // arrival time
    int processing;    // time needed to print
};

class CircularQueue {
    static const int MIN_CAP = 4;
    PrintRequest* data;
    int cap;      // current capacity
    int head;     // index of the front element
    int count;    // number of stored elements

public:
    explicit CircularQueue(int c = MIN_CAP) : data(nullptr), cap(c < 1 ? MIN_CAP : c), head(0), count(0) {
        data = new PrintRequest[cap];
    }
    ~CircularQueue() { delete[] data; }
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    bool isEmpty() const { return count == 0; }
    bool isFull()  const { return count == cap; }
    int  size()    const { return count; }
    int  capacity() const { return cap; }

    // Copys all elements in order into a new array; head restarts at 0.
    void resize(int newCap) {
        if (newCap < count) throw invalid_argument("resize: new capacity smaller than size");
        if (newCap < 1)     throw invalid_argument("resize: capacity must be >= 1");
        PrintRequest* nd = new PrintRequest[newCap];
        for (int i = 0; i < count; i++) nd[i] = data[(head + i) % cap];
        delete[] data;
        data = nd;
        cout << "    [resize] capacity " << cap << " -> " << newCap << " (" << count << " request(s) copied)\n";
        cap = newCap;
        head = 0;
    }

    void enqueue(const PrintRequest& x) {
        if (isFull()) resize(cap * 2);                       // auto-grow
        data[(head + count) % cap] = x;
        count++;
    }

    PrintRequest dequeue() {
        if (isEmpty()) throw underflow_error("dequeue from empty queue");
        PrintRequest x = data[head];
        head = (head + 1) % cap;
        count--;
        if (cap > MIN_CAP && count <= cap / 4) {             // auto-shrink
            int nc = cap / 2;
            resize(nc < MIN_CAP ? MIN_CAP : nc);
        }
        return x;
    }

    PrintRequest front() const {
        if (isEmpty()) throw underflow_error("front of empty queue");
        return data[head];
    }

    // Show the physical array: [id] = occupied slot, [ _ ] = free slot
    void display() const {
        cout << "    Queue: size=" << count << " cap=" << cap
             << " head=" << head << " tail=" << (head + count) % cap << "  ";
        for (int i = 0; i < cap; i++) {
            bool used = ((i - head + cap) % cap) < count;
            if (used) cout << "[P" << data[i].id << "]";
            else      cout << "[ _ ]";
        }
        cout << "\n";
    }
};

// Simulation
class PrintServer {
    CircularQueue q;
    int  clock;          // time at which the printer becomes free
    long totalWait;
    int  processed;
    int  submitted;
public:
    PrintServer() : clock(0), totalWait(0), processed(0), submitted(0) {}

    void submit(int id, int arrival, int processing) {
        PrintRequest r = { id, arrival, processing };
        cout << "ENQUEUE P" << id << " (arrival=" << arrival << ", processing=" << processing << ")\n";
        q.enqueue(r);
        submitted++;
        q.display();
    }

    void processNext() {
        PrintRequest r = q.dequeue();
        int start = clock > r.arrival ? clock : r.arrival;
        int wait  = start - r.arrival;
        clock = start + r.processing;
        totalWait += wait;
        processed++;
        cout << "DEQUEUE P" << r.id << " -> starts at t=" << start << ", waited " << wait
             << ", finishes at t=" << clock << "\n";
        q.display();
    }

    bool hasWork() const { return !q.isEmpty(); }

    void report() const {
        cout << "\n===== Summary =====\n"
             << "Submitted : " << submitted << "\n"
             << "Processed : " << processed << (submitted == processed ? "  (no request lost)" : "  (REQUESTS LOST!)") << "\n"
             << "Total waiting time   : " << totalWait << "\n"
             << "Average waiting time : " << fixed << setprecision(2)
             << (processed ? (double)totalWait / processed : 0.0) << "\n";
    }
};

int main() {
    PrintServer server;

    // id, arrival, processing
    server.submit(1, 0, 5);
    server.submit(2, 1, 3);
    server.submit(3, 2, 4);
    server.submit(4, 3, 2);
    server.submit(5, 4, 6);          // queue full at capacity 4 -> grows to 8

    server.processNext();
    server.processNext();

    server.submit(6, 5, 1);
    server.submit(7, 6, 3);
    server.submit(8, 7, 2);

    while (server.hasWork())         // draining makes the queue sparse -> shrinks
        server.processNext();

    server.report();
    return 0;
}