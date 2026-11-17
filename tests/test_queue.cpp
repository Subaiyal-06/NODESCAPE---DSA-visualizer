// Tests for the four queue types

#include "TestUtils.h"
#include "../src/core/ArrayQueue.h"
#include "../src/core/CircularQueue.h"
#include "../src/core/Deque.h"
#include "../src/core/LinkedQueue.h"

int main() {
    // simple array queue
    {
        ArrayQueue q;
        int v;
        check(q.isEmpty() && !q.dequeue(v) && !q.peekFront(v), "empty array queue");
        for (int i = 1; i <= ArrayQueue::MAX; i++) check(q.enqueue(i), "enqueue until full");
        check(q.isFull() && !q.enqueue(99), "full is reported");
        check(q.dequeue(v) && v == 1 && q.dequeue(v) && v == 2, "first in, first out");
        check(!q.enqueue(99), "freed space at the front is not reused (the weakness of this queue)");
        while (q.dequeue(v)) {}
        check(q.isEmpty() && q.enqueue(5) && q.peekFront(v) && v == 5, "queue works again after it became empty");
        q.clear();
        check(q.isEmpty(), "clear");
    }

    // circular queue
    {
        CircularQueue q;
        int v;
        check(q.isEmpty() && !q.dequeue(v) && !q.peekFront(v) && !q.peekRear(v), "empty circular queue");
        for (int i = 1; i <= CircularQueue::MAX; i++) check(q.enqueue(i), "fill the circular queue");
        check(q.isFull() && !q.enqueue(99), "circular queue full");
        check(q.dequeue(v) && v == 1 && q.dequeue(v) && v == 2 && q.dequeue(v) && v == 3, "dequeue three");
        check(q.enqueue(9) && q.enqueue(10) && q.enqueue(11), "freed space is reused (wrap around)");
        check(q.isFull() && !q.enqueue(12), "full again");
        check(q.peekFront(v) && v == 4 && q.peekRear(v) && v == 11, "front and rear after wrapping");
        int expected[] = {4, 5, 6, 7, 8, 9, 10, 11};
        bool inOrder = true;
        for (int i = 0; i < 8; i++) {
            if (!q.dequeue(v) || v != expected[i]) inOrder = false;
        }
        check(inOrder && q.isEmpty(), "order is kept across the wrap");
    }

    // deque
    {
        Deque d;
        int v;
        check(d.isEmpty() && !d.deleteFront(v) && !d.deleteRear(v), "empty deque");
        check(d.insertRear(2) && d.insertRear(3) && d.insertFront(1) && d.insertFront(0), "insert at both ends"); // 0 1 2 3
        check(d.size() == 4 && d.peekFront(v) && v == 0 && d.peekRear(v) && v == 3, "ends");
        check(d.deleteFront(v) && v == 0 && d.deleteRear(v) && v == 3, "delete at both ends");
        check(d.deleteFront(v) && v == 1 && d.deleteRear(v) && v == 2 && d.isEmpty(), "delete the rest");
        for (int i = 0; i < Deque::MAX; i++) check(i % 2 == 0 ? d.insertFront(i) : d.insertRear(i), "fill from both ends");
        check(d.isFull() && !d.insertFront(1) && !d.insertRear(1), "full deque");
        d.clear();
        check(d.isEmpty(), "clear");
    }

    // linked queue
    {
        LinkedQueue q;
        int v;
        check(q.isEmpty() && !q.dequeue(v) && !q.peekFront(v), "empty linked queue");
        q.enqueue(1); q.enqueue(2); q.enqueue(3);
        check(q.size() == 3 && q.peekFront(v) && v == 1, "linked queue front");
        check(q.dequeue(v) && v == 1 && q.dequeue(v) && v == 2 && q.dequeue(v) && v == 3, "linked queue order");
        check(q.isEmpty() && !q.dequeue(v), "linked queue empty again");
        q.enqueue(8);                                     // tail must be reset after emptying
        q.enqueue(9);
        check(q.dequeue(v) && v == 8 && q.dequeue(v) && v == 9, "works after being emptied");
        q.enqueue(4);
        q.clear();
        check(q.isEmpty(), "clear");
    }

    return finishTests();
}
