/**
 * NodeScape - Dynamic DSA Visualizer Engine
 * Recreates algorithm execution frames with glowing gradients,
 * real-time pseudocode synchronization, and C++ trace compatibility.
 */

// Global State
const state = {
  currentCategory: 'sorting', // sorting, list, tree, queue, graph, hash, json
  currentAlgo: 'bubble',
  array: [45, 12, 89, 34, 67, 23, 90, 15, 52, 38],
  arraySize: 10,
  frames: [],
  currentFrameIndex: 0,
  isPlaying: false,
  playbackSpeed: 1.0,
  timerId: null,
  activeCode: [],
  algorithmInfo: {}
};

// Pseudocode definitions
const PSEUDOCODE = {
  bubble: {
    title: "Bubble Sort",
    complexity: { time: "O(n²)", best: "O(n)", space: "O(1)" },
    desc: "Repeatedly steps through the list, compares adjacent elements and swaps them if they are in the wrong order. Passes continue until no swaps are needed.",
    code: [
      "for i = 0 to n - 2 do",
      "  swapped = false",
      "  for j = 0 to n - 2 - i do",
      "    if a[j] > a[j + 1] then",
      "      swap(a[j], a[j + 1])",
      "      swapped = true",
      "  if not swapped then break"
    ]
  },
  selection: {
    title: "Selection Sort",
    complexity: { time: "O(n²)", best: "O(n²)", space: "O(1)" },
    desc: "Divides the list into a sorted and unsorted region. Repeatedly finds the minimum element from the unsorted region and swaps it to the front.",
    code: [
      "for i = 0 to n - 2 do",
      "  minIndex = i",
      "  for j = i + 1 to n - 1 do",
      "    if a[j] < a[minIndex] then minIndex = j",
      "  if minIndex != i then",
      "    swap(a[i], a[minIndex])"
    ]
  },
  insertion: {
    title: "Insertion Sort",
    complexity: { time: "O(n²)", best: "O(n)", space: "O(1)" },
    desc: "Builds the sorted array one item at a time by repeatedly taking the next element and shifting larger elements to the right to make room.",
    code: [
      "for i = 1 to n - 1 do",
      "  key = a[i]",
      "  j = i - 1",
      "  while j >= 0 and a[j] > key do",
      "    a[j + 1] = a[j]; j = j - 1",
      "  a[j + 1] = key"
    ]
  },
  merge: {
    title: "Merge Sort",
    complexity: { time: "O(n log n)", best: "O(n log n)", space: "O(n)" },
    desc: "Divide and conquer algorithm that splits the array into two halves, recursively sorts them, and merges the two sorted halves back together.",
    code: [
      "mergeSort(a, left, right):",
      "  if left >= right then return",
      "  mid = (left + right) / 2",
      "  mergeSort(a, left, mid)",
      "  mergeSort(a, mid + 1, right)",
      "  merge(a, left, mid, right)"
    ]
  },
  quick: {
    title: "Quick Sort",
    complexity: { time: "O(n log n)", best: "O(n log n)", space: "O(log n)" },
    desc: "Selects a pivot element and partitions the array into values smaller than the pivot and values greater than the pivot, then sorts recursively.",
    code: [
      "quickSort(a, low, high):",
      "  if low >= high then return",
      "  p = partition(a, low, high)",
      "  quickSort(a, low, p - 1)",
      "  quickSort(a, p + 1, high)"
    ]
  },
  heap: {
    title: "Heap Sort",
    complexity: { time: "O(n log n)", best: "O(n log n)", space: "O(1)" },
    desc: "Builds a Max-Heap in-place from the array, then repeatedly moves the maximum element to the end and sifts down the new root.",
    code: [
      "buildMaxHeap(a, n)",
      "for i = n - 1 down to 1 do",
      "  swap(a[0], a[i])",
      "  siftDown(a, 0, i)"
    ]
  },
  singlyList: {
    title: "Singly Linked List",
    complexity: { time: "O(1) - O(n)", best: "O(1)", space: "O(n)" },
    desc: "Nodes chained together via next pointers. Demonstrating insertions, deletions, search, and in-place pointer reversal.",
    code: [
      "Node* n = new Node(val)",
      "n->next = head",
      "head = n"
    ]
  },
  doublyList: {
    title: "Doubly Linked List",
    complexity: { time: "O(1) - O(n)", best: "O(1)", space: "O(n)" },
    desc: "Nodes have both next and prev pointers with head and tail references, allowing O(1) push and pop at both ends.",
    code: [
      "DNode* n = new DNode(val)",
      "n->prev = tail; tail->next = n",
      "tail = n"
    ]
  },
  circularList: {
    title: "Circular Linked List",
    complexity: { time: "O(1) - O(n)", best: "O(1)", space: "O(n)" },
    desc: "The tail node points back to the head node. Single tail pointer allows O(1) insertions at both front and back.",
    code: [
      "CNode* n = new CNode(val)",
      "n->next = tail->next",
      "tail->next = n; tail = n"
    ]
  },
  bst: {
    title: "Binary Search Tree",
    complexity: { time: "O(log n)", best: "O(1)", space: "O(n)" },
    desc: "Binary tree maintaining the invariant that left subtree < node < right subtree. Supports insert, search, and 3-case deletion.",
    code: [
      "insert(node, val):",
      "  if node == null then return new Node(val)",
      "  if val < node.val then node.left = insert(node.left, val)",
      "  else if val > node.val then node.right = insert(node.right, val)",
      "  return node"
    ]
  },
  avl: {
    title: "AVL Tree",
    complexity: { time: "O(log n)", best: "O(log n)", space: "O(n)" },
    desc: "Self-balancing BST. Balance factor = height(left) - height(right). Any imbalance > 1 or < -1 is immediately restored with rotations (LL, RR, LR, RL).",
    code: [
      "balance = height(node.left) - height(node.right)",
      "if balance > 1 and balance(node.left) >= 0: rotateRight(node)",
      "if balance < -1 and balance(node.right) <= 0: rotateLeft(node)",
      "if balance > 1 and balance(node.left) < 0: rotateLeft(left); rotateRight(node)",
      "if balance < -1 and balance(node.right) > 0: rotateRight(right); rotateLeft(node)"
    ]
  },
  stackQueue: {
    title: "Stack & Circular Queue",
    complexity: { time: "O(1)", best: "O(1)", space: "O(n)" },
    desc: "LIFO (Last In First Out) Stack vs FIFO (First In First Out) Circular Queue with wrap-around pointers front & rear.",
    code: [
      "Stack push: top++; arr[top] = val",
      "Stack pop:  val = arr[top]; top--",
      "Queue enq:  arr[rear] = val; rear = (rear + 1) % MAX",
      "Queue deq:  val = arr[front]; front = (front + 1) % MAX"
    ]
  },
  graphs: {
    title: "Graph Traversal & MST",
    complexity: { time: "O(V + E)", best: "O(1)", space: "O(V)" },
    desc: "BFS explores layer by layer; DFS searches down branches. Prim and Kruskal find the Minimum Spanning Tree.",
    code: [
      "BFS: queue.push(start); visited[start] = true",
      "while not queue.empty():",
      "  u = queue.pop()",
      "  for v in adj[u]:",
      "    if not visited[v]: visit(v); queue.push(v)"
    ]
  },
  hashing: {
    title: "Hash Tables",
    complexity: { time: "O(1) avg", best: "O(1)", space: "O(n)" },
    desc: "Key-value indexing with hash functions (division, folding, mid-square) and collision resolution (chaining, linear probing, quadratic probing).",
    code: [
      "slot = hash(key) % tableSize",
      "while slot occupied and key not found:",
      "  slot = probe(slot, attempt++)",
      "insertAt(slot, key)"
    ]
  }
};

// ==========================================
// Algorithm Simulators (Generating Frames)
// ==========================================

function generateFrames() {
  const algo = state.currentAlgo;
  state.frames = [];

  if (state.currentCategory === 'sorting') {
    const a = [...state.array];
    if (algo === 'bubble') simBubbleSort(a);
    else if (algo === 'selection') simSelectionSort(a);
    else if (algo === 'insertion') simInsertionSort(a);
    else if (algo === 'merge') simMergeSort(a);
    else if (algo === 'quick') simQuickSort(a);
    else if (algo === 'heap') simHeapSort(a);
  } else if (state.currentCategory === 'list') {
    simLinkedList(state.currentAlgo);
  } else if (state.currentCategory === 'tree') {
    simTrees(state.currentAlgo);
  } else if (state.currentCategory === 'queue') {
    simStackQueue();
  } else if (state.currentCategory === 'graph') {
    simGraph();
  } else if (state.currentCategory === 'hash') {
    simHashTable();
  }

  state.currentFrameIndex = 0;
  updateUI();
}

// ------------------------------------------
// 1. Bubble Sort Simulation
// ------------------------------------------
function simBubbleSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  // Helper to push frame
  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) {
          role = stateType; // 'checking', 'replaced', 'unswapped', 'done'
        }
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Bubble Sort. We will compare adjacent numbers and swap when out of order.", 0, [], 'normal');

  for (let i = 0; i < n - 1; i++) {
    let swapped = false;
    for (let j = 0; j < n - 1 - i; j++) {
      comparisons++;
      // 1. CHECKING: Pseudocode reaches elements
      pushFrame(
        `Checking elements at index ${j} (${a[j]}) and ${j + 1} (${a[j + 1]}). Comparing if ${a[j]} > ${a[j + 1]}...`,
        3,
        [j, j + 1],
        'checking',
        [{ index: j, label: 'j' }, { index: j + 1, label: 'j+1' }]
      );

      if (a[j] > a[j + 1]) {
        // 2. CONDITION MET: Swapped / Replaced -> Turns Green with Green Glow!
        swaps++;
        const temp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = temp;
        swapped = true;

        pushFrame(
          `Condition met! ${temp} > ${a[j]}. Swapping elements into correct order!`,
          4,
          [j, j + 1],
          'replaced',
          [{ index: j, label: 'swapped' }, { index: j + 1, label: 'swapped' }]
        );
      } else {
        // 3. CONDITION NOT MET: Kept / Not replaced -> Rose / Red Glow
        pushFrame(
          `Condition not met. ${a[j]} <= ${a[j + 1]}, already in correct relative order. No swap.`,
          3,
          [j, j + 1],
          'unswapped',
          [{ index: j, label: 'keep' }, { index: j + 1, label: 'keep' }]
        );
      }
    }

    pushFrame(`Pass ${i + 1} complete. Element ${a[n - 1 - i]} is placed in its final sorted position.`, 1, [n - 1 - i], 'done');
    if (!swapped) {
      pushFrame("No swaps occurred in this entire pass. The array is fully sorted early!", 6, [], 'normal');
      break;
    }
  }

  // Final sorted frame
  state.frames.push({
    kind: 'bars',
    message: `Bubble Sort Complete! Total Comparisons: ${comparisons}, Total Swaps: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 2. Selection Sort Simulation
// ------------------------------------------
function simSelectionSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) role = stateType;
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Selection Sort. We find the smallest item in the remaining unsorted part and swap it to the front.", 0, [], 'normal');

  for (let i = 0; i < n - 1; i++) {
    let minIdx = i;
    pushFrame(`Assuming position ${i} (${a[i]}) holds the current minimum for this pass.`, 1, [i], 'checking', [{ index: i, label: 'min' }]);

    for (let j = i + 1; j < n; j++) {
      comparisons++;
      pushFrame(
        `Checking if a[${j}] (${a[j]}) is smaller than current min a[${minIdx}] (${a[minIdx]})...`,
        3,
        [j, minIdx],
        'checking',
        [{ index: minIdx, label: 'min' }, { index: j, label: 'j' }]
      );

      if (a[j] < a[minIdx]) {
        minIdx = j;
        pushFrame(
          `Condition met! Found new smaller value ${a[j]} at index ${j}!`,
          3,
          [j],
          'pivot',
          [{ index: minIdx, label: 'new min' }]
        );
      } else {
        pushFrame(
          `${a[j]} is not smaller than ${a[minIdx]}. Keeping current minimum.`,
          3,
          [j],
          'unswapped',
          [{ index: minIdx, label: 'min' }]
        );
      }
    }

    if (minIdx !== i) {
      swaps++;
      const t = a[i];
      a[i] = a[minIdx];
      a[minIdx] = t;
      pushFrame(
        `Swapping minimum value ${a[i]} into its final position at index ${i}.`,
        5,
        [i, minIdx],
        'replaced',
        [{ index: i, label: 'sorted' }]
      );
    } else {
      pushFrame(`Element at position ${i} was already the minimum. No swap needed.`, 4, [i], 'done');
    }
  }

  state.frames.push({
    kind: 'bars',
    message: `Selection Sort Complete! Total Comparisons: ${comparisons}, Total Swaps: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 3. Insertion Sort Simulation
// ------------------------------------------
function simInsertionSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) role = stateType;
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Insertion Sort. The first element alone forms a sorted prefix. We insert subsequent elements into place.", 0, [0], 'done');

  for (let i = 1; i < n; i++) {
    const key = a[i];
    let j = i - 1;
    pushFrame(`Selected key ${key} at index ${i} to insert into the sorted subarray on its left.`, 1, [i], 'pivot', [{ index: i, label: 'key' }]);

    while (j >= 0) {
      comparisons++;
      pushFrame(`Checking if a[${j}] (${a[j]}) > key (${key})...`, 3, [j, j + 1], 'checking', [{ index: j, label: 'compare' }]);

      if (a[j] > key) {
        swaps++;
        a[j + 1] = a[j];
        pushFrame(`Condition met! ${a[j]} > ${key}. Shifting ${a[j]} one spot to the right!`, 4, [j + 1], 'replaced', [{ index: j + 1, label: 'shift' }]);
        j--;
      } else {
        pushFrame(`Condition not met. ${a[j]} <= ${key}. Correct insertion spot found.`, 3, [j], 'unswapped');
        break;
      }
    }
    a[j + 1] = key;
    pushFrame(`Inserted key ${key} into position ${j + 1}.`, 5, [j + 1], 'done', [{ index: j + 1, label: 'inserted' }]);
  }

  state.frames.push({
    kind: 'bars',
    message: `Insertion Sort Complete! Total Comparisons: ${comparisons}, Total Shifts/Swaps: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 4. Merge Sort Simulation
// ------------------------------------------
function simMergeSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) role = stateType;
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Merge Sort. Divide and conquer: repeatedly divide array in half, then merge sorted parts.", 0, [], 'normal');

  function merge(left, mid, right) {
    const temp = [];
    let i = left;
    let j = mid + 1;

    pushFrame(`Merging sorted subarrays [${left}..${mid}] and [${mid + 1}..${right}].`, 4, [i, j], 'checking');

    while (i <= mid && j <= right) {
      comparisons++;
      pushFrame(`Comparing left element ${a[i]} with right element ${a[j]}...`, 4, [i, j], 'checking');
      if (a[i] <= a[j]) {
        temp.push(a[i]);
        pushFrame(`Left element ${a[i]} is smaller/equal. Placing into temporary merged buffer.`, 4, [i], 'replaced');
        i++;
      } else {
        temp.push(a[j]);
        pushFrame(`Right element ${a[j]} is smaller. Placing into temporary merged buffer.`, 4, [j], 'replaced');
        j++;
      }
      swaps++;
    }

    while (i <= mid) {
      temp.push(a[i]);
      pushFrame(`Copying remaining left element ${a[i]} into merged buffer.`, 4, [i], 'replaced');
      i++;
      swaps++;
    }
    while (j <= right) {
      temp.push(a[j]);
      pushFrame(`Copying remaining right element ${a[j]} into merged buffer.`, 4, [j], 'replaced');
      j++;
      swaps++;
    }

    for (let k = 0; k < temp.length; k++) {
      a[left + k] = temp[k];
    }

    const range = [];
    for (let k = left; k <= right; k++) range.push(k);
    pushFrame(`Subarray [${left}..${right}] is now merged and sorted.`, 4, range, 'done');
  }

  function sort(left, right) {
    if (left >= right) return;
    const mid = Math.floor((left + right) / 2);
    pushFrame(`Splitting range [${left}..${right}] at midpoint ${mid}.`, 2, [mid], 'pivot');
    sort(left, mid);
    sort(mid + 1, right);
    merge(left, mid, right);
  }

  sort(0, n - 1);

  state.frames.push({
    kind: 'bars',
    message: `Merge Sort Complete! Total Comparisons: ${comparisons}, Total Copies: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 5. Quick Sort Simulation
// ------------------------------------------
function simQuickSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) role = stateType;
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Quick Sort. Pick a pivot, partition elements smaller to the left and larger to the right.", 0, [], 'normal');

  function partition(low, high) {
    const pivot = a[high];
    pushFrame(`Chosen pivot ${pivot} at index ${high}.`, 2, [high], 'pivot', [{ index: high, label: 'pivot' }]);
    let i = low - 1;

    for (let j = low; j < high; j++) {
      comparisons++;
      pushFrame(`Checking if a[${j}] (${a[j]}) < pivot (${pivot})...`, 2, [j, high], 'checking', [{ index: j, label: 'j' }, { index: high, label: 'pivot' }]);

      if (a[j] < pivot) {
        i++;
        if (i !== j) {
          swaps++;
          const t = a[i];
          a[i] = a[j];
          a[j] = t;
          pushFrame(`Condition met! ${a[i]} < ${pivot}. Swapping to left partition at index ${i}.`, 2, [i, j], 'replaced', [{ index: i, label: '< pivot' }]);
        } else {
          pushFrame(`${a[j]} < ${pivot}, already in left partition.`, 2, [j], 'replaced');
        }
      } else {
        pushFrame(`${a[j]} >= ${pivot}. Remains on right side of partition.`, 2, [j], 'unswapped');
      }
    }

    swaps++;
    const t = a[i + 1];
    a[i + 1] = a[high];
    a[high] = t;
    const pIdx = i + 1;
    pushFrame(`Placed pivot ${pivot} into its exact final sorted position at index ${pIdx}.`, 2, [pIdx], 'done', [{ index: pIdx, label: 'pivot fixed' }]);
    return pIdx;
  }

  function sort(low, high) {
    if (low >= high) {
      if (low === high) pushFrame(`Single element at index ${low} is sorted.`, 1, [low], 'done');
      return;
    }
    const p = partition(low, high);
    sort(low, p - 1);
    sort(p + 1, high);
  }

  sort(0, n - 1);

  state.frames.push({
    kind: 'bars',
    message: `Quick Sort Complete! Total Comparisons: ${comparisons}, Total Swaps: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 6. Heap Sort Simulation
// ------------------------------------------
function simHeapSort(arr) {
  const n = arr.length;
  let comparisons = 0;
  let swaps = 0;
  const a = [...arr];

  function pushFrame(msg, line, activeIndices, stateType, markers = []) {
    state.frames.push({
      kind: 'bars',
      message: msg,
      codeLine: line,
      comparisons,
      swaps,
      cells: a.map((val, idx) => {
        let role = 'normal';
        if (activeIndices.includes(idx)) role = stateType;
        return { value: val, role };
      }),
      markers
    });
  }

  pushFrame("Starting Heap Sort. Step 1: Bottom-up Heapify to build a Max-Heap in O(n).", 0, [], 'normal');

  function siftDown(limit, idx) {
    while (true) {
      const left = 2 * idx + 1;
      const right = 2 * idx + 2;
      let largest = idx;

      if (left < limit) {
        comparisons++;
        if (a[left] > a[largest]) largest = left;
      }
      if (right < limit) {
        comparisons++;
        if (a[right] > a[largest]) largest = right;
      }

      if (largest === idx) break;

      swaps++;
      const t = a[idx];
      a[idx] = a[largest];
      a[largest] = t;
      pushFrame(`Sift down: Swapping ${t} with larger child ${a[idx]}.`, 3, [idx, largest], 'replaced');
      idx = largest;
    }
  }

  // Build max heap
  for (let i = Math.floor(n / 2) - 1; i >= 0; i--) {
    pushFrame(`Heapifying subtree at index ${i}...`, 0, [i], 'checking');
    siftDown(n, i);
  }
  pushFrame("Max-Heap built! The largest element is guaranteed at index 0.", 0, [0], 'pivot', [{ index: 0, label: 'MAX' }]);

  // Extract from heap
  for (let i = n - 1; i > 0; i--) {
    swaps++;
    const t = a[0];
    a[0] = a[i];
    a[i] = t;
    pushFrame(`Extracting max root ${t} to sorted end index ${i}.`, 2, [0, i], 'replaced', [{ index: i, label: 'final' }]);
    siftDown(i, 0);
  }

  state.frames.push({
    kind: 'bars',
    message: `Heap Sort Complete! Total Comparisons: ${comparisons}, Total Swaps: ${swaps}.`,
    codeLine: -1,
    comparisons,
    swaps,
    cells: a.map(val => ({ value: val, role: 'done' })),
    markers: []
  });
}

// ------------------------------------------
// 7. Linked List Simulators
// ------------------------------------------
function simLinkedList(algo) {
  state.frames = [];
  const initial = [10, 25, 40, 55, 70];

  function pushListFrame(msg, line, cells, markers, floating = null) {
    state.frames.push({
      kind: 'list',
      message: msg,
      codeLine: line,
      comparisons: 0,
      swaps: 0,
      cells: cells.map(c => typeof c === 'number' ? { value: c, role: 'normal' } : c),
      markers,
      floating
    });
  }

  if (algo === 'singlyList') {
    pushListFrame("Current singly linked list with head pointer.", 0, initial, [{ index: 0, label: 'head' }]);
    // Insert at beginning: 5
    pushListFrame("Create new node with value [5] (floating).", 0, initial, [{ index: 0, label: 'head' }], { value: 5, role: 'new' });
    pushListFrame("Set newNode->next = head.", 1, initial, [{ index: 0, label: 'head' }], { value: 5, role: 'new' });
    pushListFrame("Reassign head = newNode. Inserted in O(1) time!", 2, [{ value: 5, role: 'new' }, ...initial], [{ index: 0, label: 'head' }]);
    
    // Reverse list
    const currentList = [5, ...initial];
    pushListFrame("Reversing the linked list in-place using 3 pointers (prev, cur, next).", 0, currentList, [{ index: 0, label: 'head/cur' }]);
    for (let k = 0; k < currentList.length; k++) {
      pushListFrame(`Reversing link at node [${currentList[k]}].`, 1, 
        currentList.map((v, idx) => ({ value: v, role: idx === k ? 'replaced' : (idx < k ? 'done' : 'normal') })),
        [{ index: k, label: 'cur' }]
      );
    }
    const reversed = [...currentList].reverse();
    pushListFrame("List reversed successfully! Head now points to the previous tail.", 2, reversed.map(v => ({ value: v, role: 'done' })), [{ index: 0, label: 'head' }]);
  } else if (algo === 'doublyList') {
    pushListFrame("Doubly linked list maintains both head and tail pointers with next & prev links.", 0, initial, [{ index: 0, label: 'head' }, { index: initial.length - 1, label: 'tail' }]);
    pushListFrame("Inserting [85] at end: tail->next = newNode, newNode->prev = tail, tail = newNode. O(1) operation!", 1, 
      [...initial, { value: 85, role: 'new' }], 
      [{ index: 0, label: 'head' }, { index: initial.length, label: 'tail' }]
    );
  } else if (algo === 'circularList') {
    pushListFrame("Circular linked list: Tail node points directly back to the head (tail->next == head).", 0, initial, [{ index: initial.length - 1, label: 'tail' }]);
    pushListFrame("Inserting [99] at end: Linked through tail->next and advancing tail. O(1) without full traversal!", 1,
      [...initial, { value: 99, role: 'new' }],
      [{ index: initial.length, label: 'tail' }]
    );
  }
}

// ------------------------------------------
// 8. Trees Simulators (BST & AVL)
// ------------------------------------------
function simTrees(algo) {
  state.frames = [];

  function pushTreeFrame(msg, line, nodes, rootId = 1) {
    state.frames.push({
      kind: 'tree',
      message: msg,
      codeLine: line,
      comparisons: 0,
      swaps: 0,
      nodes,
      rootId,
      cells: []
    });
  }

  if (algo === 'bst') {
    // Initial BST: root 50, left 30, right 70
    const tree1 = [
      { id: 1, value: 50, left: 2, right: 3, role: 'normal' },
      { id: 2, value: 30, left: 4, right: 5, role: 'normal' },
      { id: 3, value: 70, left: -1, right: -1, role: 'normal' },
      { id: 4, value: 20, left: -1, right: -1, role: 'normal' },
      { id: 5, value: 40, left: -1, right: -1, role: 'normal' }
    ];
    pushTreeFrame("Binary Search Tree: Invariant: left < root < right.", 0, tree1);

    // Insert 35: check 50
    pushTreeFrame("Inserting 35: Compare with root 50. 35 < 50, go left.", 2, [
      { id: 1, value: 50, left: 2, right: 3, role: 'checking' },
      { id: 2, value: 30, left: 4, right: 5, role: 'normal' },
      { id: 3, value: 70, left: -1, right: -1, role: 'normal' },
      { id: 4, value: 20, left: -1, right: -1, role: 'normal' },
      { id: 5, value: 40, left: -1, right: -1, role: 'normal' }
    ]);

    // Check 30
    pushTreeFrame("Compare 35 with node 30. 35 > 30, go right.", 3, [
      { id: 1, value: 50, left: 2, right: 3, role: 'normal' },
      { id: 2, value: 30, left: 4, right: 5, role: 'checking' },
      { id: 3, value: 70, left: -1, right: -1, role: 'normal' },
      { id: 4, value: 20, left: -1, right: -1, role: 'normal' },
      { id: 5, value: 40, left: -1, right: -1, role: 'normal' }
    ]);

    // Check 40
    pushTreeFrame("Compare 35 with node 40. 35 < 40, go left. Left is empty -> Insert!", 2, [
      { id: 1, value: 50, left: 2, right: 3, role: 'normal' },
      { id: 2, value: 30, left: 4, right: 5, role: 'normal' },
      { id: 3, value: 70, left: -1, right: -1, role: 'normal' },
      { id: 4, value: 20, left: -1, right: -1, role: 'normal' },
      { id: 5, value: 40, left: 6, right: -1, role: 'checking' },
      { id: 6, value: 35, left: -1, right: -1, role: 'replaced' }
    ]);

    pushTreeFrame("Node 35 inserted successfully in O(log n) time.", 4, [
      { id: 1, value: 50, left: 2, right: 3, role: 'normal' },
      { id: 2, value: 30, left: 4, right: 5, role: 'normal' },
      { id: 3, value: 70, left: -1, right: -1, role: 'normal' },
      { id: 4, value: 20, left: -1, right: -1, role: 'normal' },
      { id: 5, value: 40, left: 6, right: -1, role: 'normal' },
      { id: 6, value: 35, left: -1, right: -1, role: 'done' }
    ]);
  } else if (algo === 'avl') {
    // Show AVL Left-Left case and Right Rotation
    const unbal = [
      { id: 1, value: 30, left: 2, right: -1, role: 'unswapped' },
      { id: 2, value: 20, left: 3, right: -1, role: 'checking' },
      { id: 3, value: 10, left: -1, right: -1, role: 'checking' }
    ];
    pushTreeFrame("AVL Tree imbalance detected! Node 30 has Balance Factor = +2 (Left-Heavy LL case).", 1, unbal);
    pushTreeFrame("Executing Single Right Rotation around Node 30: Node 20 moves up to become root.", 1, [
      { id: 2, value: 20, left: 3, right: 1, role: 'replaced' },
      { id: 3, value: 10, left: -1, right: -1, role: 'normal' },
      { id: 1, value: 30, left: -1, right: -1, role: 'normal' }
    ], 2);
    pushTreeFrame("Tree rebalanced! All nodes have Balance Factor in {-1, 0, 1}. Guaranteed O(log n).", 0, [
      { id: 2, value: 20, left: 3, right: 1, role: 'done' },
      { id: 3, value: 10, left: -1, right: -1, role: 'done' },
      { id: 1, value: 30, left: -1, right: -1, role: 'done' }
    ], 2);
  }
}

// ------------------------------------------
// 9. Stack & Queue Simulation
// ------------------------------------------
function simStackQueue() {
  state.frames = [];
  const q = [10, 20, 30, 40];
  state.frames.push({
    kind: 'bars',
    message: "Circular Queue in memory: Front = 0, Rear = 3. Count = 4 / MAX.",
    codeLine: 2,
    comparisons: 0,
    swaps: 0,
    cells: q.map((v, i) => ({ value: v, role: 'normal' })),
    markers: [{ index: 0, label: 'front' }, { index: 3, label: 'rear' }]
  });

  // Enqueue 50
  state.frames.push({
    kind: 'bars',
    message: "Enqueueing 50: Rear advances with modulo wrap-around (rear = (rear + 1) % MAX).",
    codeLine: 2,
    comparisons: 0,
    swaps: 0,
    cells: [...q, { value: 50, role: 'replaced' }],
    markers: [{ index: 0, label: 'front' }, { index: 4, label: 'rear' }]
  });

  // Dequeue 10
  state.frames.push({
    kind: 'bars',
    message: "Dequeuing element 10 from Front: Front advances (front = (front + 1) % MAX). Freed space is reused!",
    codeLine: 3,
    comparisons: 0,
    swaps: 0,
    cells: [20, 30, 40, 50].map((v, i) => ({ value: v, role: i === 0 ? 'checking' : 'normal' })),
    markers: [{ index: 0, label: 'front' }, { index: 3, label: 'rear' }]
  });
}

// ------------------------------------------
// 10. Graph & Hash Simulation
// ------------------------------------------
function simGraph() {
  state.frames = [];
  const vertices = [0, 1, 2, 3, 4, 5];
  state.frames.push({
    kind: 'bars',
    message: "Graph BFS Traversal starting at vertex 0: Queue initialized with [0].",
    codeLine: 0,
    comparisons: 0,
    swaps: 0,
    cells: vertices.map(v => ({ value: v, role: v === 0 ? 'checking' : 'normal' })),
    markers: [{ index: 0, label: 'start' }]
  });

  state.frames.push({
    kind: 'bars',
    message: "Visiting neighbors of 0: vertices 1 and 2 enqueued.",
    codeLine: 4,
    comparisons: 0,
    swaps: 0,
    cells: vertices.map(v => ({ value: v, role: (v === 1 || v === 2) ? 'replaced' : (v === 0 ? 'done' : 'normal') })),
    markers: [{ index: 1, label: 'visited' }, { index: 2, label: 'visited' }]
  });

  state.frames.push({
    kind: 'bars',
    message: "BFS complete! All vertices reached in minimum hop order.",
    codeLine: -1,
    comparisons: 0,
    swaps: 0,
    cells: vertices.map(v => ({ value: v, role: 'done' })),
    markers: []
  });
}

function simHashTable() {
  state.frames = [];
  const slots = [14, 0, 26, 0, 42, 0, 0, 35];
  state.frames.push({
    kind: 'bars',
    message: "Hash Table (Division Method: key % 8). Slot 0, 2, 4, 7 occupied.",
    codeLine: 0,
    comparisons: 0,
    swaps: 0,
    cells: slots.map(v => ({ value: v, role: v > 0 ? 'normal' : 'unswapped' })),
    markers: []
  });

  state.frames.push({
    kind: 'bars',
    message: "Inserting key 50: hash(50) = 50 % 8 = 2. Slot 2 occupied (collision with 26)!",
    codeLine: 1,
    comparisons: 1,
    swaps: 0,
    cells: slots.map((v, i) => ({ value: v, role: i === 2 ? 'checking' : (v > 0 ? 'normal' : 'unswapped') })),
    markers: [{ index: 2, label: 'collision' }]
  });

  state.frames.push({
    kind: 'bars',
    message: "Linear Probing: Probe next slot 3 -> Empty! Key 50 inserted with 1 collision.",
    codeLine: 3,
    comparisons: 2,
    swaps: 0,
    cells: [14, 0, 26, 50, 42, 0, 0, 35].map((v, i) => ({ value: v, role: i === 3 ? 'replaced' : (v > 0 ? 'normal' : 'unswapped') })),
    markers: [{ index: 3, label: 'inserted' }]
  });
}

// ==========================================
// Rendering Engine
// ==========================================

function renderFrame(frame) {
  if (!frame) return;

  // 1. Update text narrative & badges
  const explanationEl = document.getElementById('explanationText');
  if (explanationEl) explanationEl.textContent = frame.message || '';

  const comparisonsBadge = document.getElementById('metricComparisons');
  if (comparisonsBadge) comparisonsBadge.textContent = frame.comparisons || 0;

  const swapsBadge = document.getElementById('metricSwaps');
  if (swapsBadge) swapsBadge.textContent = frame.swaps || 0;

  const stepCountBadge = document.getElementById('stepCounter');
  if (stepCountBadge) {
    stepCountBadge.textContent = `Step ${state.currentFrameIndex + 1} of ${state.frames.length}`;
  }

  const slider = document.getElementById('timelineSlider');
  if (slider) {
    slider.max = Math.max(0, state.frames.length - 1);
    slider.value = state.currentFrameIndex;
  }

  // 2. Highlight pseudocode line
  highlightCodeLine(frame.codeLine);

  // 3. Render Viewport based on kind
  const viewport = document.getElementById('stageViewport');
  if (!viewport) return;
  viewport.innerHTML = '';

  if (frame.kind === 'bars') {
    renderBars(viewport, frame);
  } else if (frame.kind === 'list') {
    renderList(viewport, frame);
  } else if (frame.kind === 'tree') {
    renderTree(viewport, frame);
  }
}

// Render Sorting Bars
function renderBars(container, frame) {
  const barsWrap = document.createElement('div');
  barsWrap.className = 'bars-container';

  const cells = frame.cells || [];
  const maxVal = Math.max(10, ...cells.map(c => Math.abs(c.value)));

  cells.forEach((cell, idx) => {
    const barCol = document.createElement('div');
    barCol.className = 'v-bar-wrapper';

    // Markers on top (i, j, min)
    if (frame.markers) {
      const markers = frame.markers.filter(m => m.index === idx);
      if (markers.length > 0) {
        const markerBadge = document.createElement('div');
        markerBadge.className = 'v-bar-marker';
        markerBadge.textContent = markers.map(m => m.label).join('/');
        barCol.appendChild(markerBadge);
      }
    }

    const barEl = document.createElement('div');
    barEl.className = `v-bar role-${cell.role || 'normal'}`;
    const heightPercent = Math.max(8, Math.round((cell.value / maxVal) * 88));
    barEl.style.height = `${heightPercent}%`;

    const valLabel = document.createElement('span');
    valLabel.className = 'v-bar-label';
    valLabel.textContent = cell.value;
    barEl.appendChild(valLabel);

    const idxLabel = document.createElement('span');
    idxLabel.className = 'v-bar-index';
    idxLabel.textContent = idx;
    barEl.appendChild(idxLabel);

    barCol.appendChild(barEl);
    barsWrap.appendChild(barCol);
  });

  container.appendChild(barsWrap);
}

// Render Linked List Nodes
function renderList(container, frame) {
  if (frame.floating) {
    const floatEl = document.createElement('div');
    floatEl.className = 'floating-node-card';
    floatEl.innerHTML = `<span>New Unlinked Node:</span> <strong>[${frame.floating.value}]</strong>`;
    container.appendChild(floatEl);
  }

  const nodesStage = document.createElement('div');
  nodesStage.className = 'nodes-stage';

  (frame.cells || []).forEach((cell, idx) => {
    const item = document.createElement('div');
    item.className = 'list-node-item';

    if (frame.markers) {
      const markers = frame.markers.filter(m => m.index === idx);
      if (markers.length > 0) {
        const ptr = document.createElement('div');
        ptr.className = 'node-pointer';
        ptr.textContent = markers.map(m => m.label).join('/');
        item.appendChild(ptr);
      }
    }

    const box = document.createElement('div');
    box.className = `node-box role-${cell.role || 'normal'}`;
    box.textContent = cell.value;
    item.appendChild(box);

    if (idx < frame.cells.length - 1) {
      const arrow = document.createElement('div');
      arrow.className = 'node-arrow';
      arrow.textContent = '➔';
      item.appendChild(arrow);
    } else {
      const nullArrow = document.createElement('div');
      nullArrow.className = 'node-arrow';
      nullArrow.textContent = '➔ NULL';
      item.appendChild(nullArrow);
    }

    nodesStage.appendChild(item);
  });

  container.appendChild(nodesStage);
}

// Render Tree (BST / AVL) in SVG
function renderTree(container, frame) {
  const treeContainer = document.createElement('div');
  treeContainer.className = 'tree-svg-container';

  const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
  svg.setAttribute('viewBox', '0 0 800 360');

  const nodes = frame.nodes || [];
  if (nodes.length === 0) {
    container.appendChild(treeContainer);
    return;
  }

  // Calculate coordinates
  const positions = {};
  function layout(nodeId, x, y, dx) {
    const node = nodes.find(n => n.id === nodeId);
    if (!node) return;
    positions[nodeId] = { x, y, node };
    if (node.left !== -1) layout(node.left, x - dx, y + 70, dx / 2);
    if (node.right !== -1) layout(node.right, x + dx, y + 70, dx / 2);
  }

  layout(frame.rootId, 400, 50, 180);

  // Draw Links
  nodes.forEach(node => {
    const parentPos = positions[node.id];
    if (!parentPos) return;

    [node.left, node.right].forEach(childId => {
      if (childId !== -1 && positions[childId]) {
        const childPos = positions[childId];
        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        line.setAttribute('x1', parentPos.x);
        line.setAttribute('y1', parentPos.y);
        line.setAttribute('x2', childPos.x);
        line.setAttribute('y2', childPos.y);
        line.setAttribute('stroke', 'rgba(255, 255, 255, 0.2)');
        line.setAttribute('stroke-width', '2');
        svg.appendChild(line);
      }
    });
  });

  // Draw Glowing Nodes
  nodes.forEach(node => {
    const pos = positions[node.id];
    if (!pos) return;

    const g = document.createElementNS('http://www.w3.org/2000/svg', 'g');

    // Outer Glow Circle
    const circle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
    circle.setAttribute('cx', pos.x);
    circle.setAttribute('cy', pos.y);
    circle.setAttribute('r', '22');

    let fill = '#1d4ed8';
    let filter = 'drop-shadow(0 0 10px rgba(56, 189, 248, 0.5))';
    if (node.role === 'checking') {
      fill = '#d97706';
      filter = 'drop-shadow(0 0 16px rgba(251, 191, 36, 0.8))';
    } else if (node.role === 'replaced') {
      fill = '#059669';
      filter = 'drop-shadow(0 0 18px rgba(52, 211, 153, 0.9))';
    } else if (node.role === 'done') {
      fill = '#0d9488';
      filter = 'drop-shadow(0 0 12px rgba(45, 212, 191, 0.6))';
    }

    circle.setAttribute('fill', fill);
    circle.setAttribute('style', `filter: ${filter}; transition: all 0.3s ease;`);
    g.appendChild(circle);

    // Text Label
    const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
    text.setAttribute('x', pos.x);
    text.setAttribute('y', pos.y + 5);
    text.setAttribute('text-anchor', 'middle');
    text.setAttribute('fill', '#ffffff');
    text.setAttribute('font-family', 'var(--font-mono)');
    text.setAttribute('font-size', '14');
    text.setAttribute('font-weight', 'bold');
    text.textContent = node.value;
    g.appendChild(text);

    svg.appendChild(g);
  });

  treeContainer.appendChild(svg);
  container.appendChild(treeContainer);
}

// Highlight pseudocode line
function highlightCodeLine(activeLine) {
  const lines = document.querySelectorAll('.code-line');
  lines.forEach((lineEl, idx) => {
    if (idx === activeLine) {
      lineEl.classList.add('active');
      lineEl.scrollIntoView({ block: 'nearest', behavior: 'smooth' });
    } else {
      lineEl.classList.remove('active');
    }
  });
}

// Update Pseudocode and Theory Panel
function updateTheoryAndCode() {
  const info = PSEUDOCODE[state.currentAlgo] || PSEUDOCODE.bubble;
  state.algorithmInfo = info;

  const titleEl = document.getElementById('stageTitle');
  if (titleEl) titleEl.textContent = info.title;

  const descEl = document.getElementById('theoryDescription');
  if (descEl) descEl.textContent = info.desc;

  const compTimeEl = document.getElementById('compTime');
  if (compTimeEl) compTimeEl.textContent = info.complexity.time;

  const compBestEl = document.getElementById('compBest');
  if (compBestEl) compBestEl.textContent = info.complexity.best;

  const compSpaceEl = document.getElementById('compSpace');
  if (compSpaceEl) compSpaceEl.textContent = info.complexity.space;

  const codeContainer = document.getElementById('codeViewport');
  if (codeContainer) {
    codeContainer.innerHTML = '';
    info.code.forEach((lineText, idx) => {
      const lineDiv = document.createElement('div');
      lineDiv.className = 'code-line';
      lineDiv.innerHTML = `
        <span class="code-line-num">${idx + 1}</span>
        <span class="code-line-text">${escapeHtml(lineText)}</span>
      `;
      codeContainer.appendChild(lineDiv);
    });
  }
}

function escapeHtml(str) {
  return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

// ==========================================
// Playback Controller
// ==========================================

function play() {
  if (state.isPlaying) return;
  state.isPlaying = true;
  updatePlayButtonIcon();

  const delay = Math.round(500 / state.playbackSpeed);
  state.timerId = setInterval(() => {
    if (state.currentFrameIndex < state.frames.length - 1) {
      state.currentFrameIndex++;
      renderFrame(state.frames[state.currentFrameIndex]);
    } else {
      pause();
    }
  }, delay);
}

function pause() {
  state.isPlaying = false;
  if (state.timerId) {
    clearInterval(state.timerId);
    state.timerId = null;
  }
  updatePlayButtonIcon();
}

function togglePlay() {
  if (state.isPlaying) pause();
  else {
    if (state.currentFrameIndex >= state.frames.length - 1) {
      state.currentFrameIndex = 0;
    }
    play();
  }
}

function stepForward() {
  pause();
  if (state.currentFrameIndex < state.frames.length - 1) {
    state.currentFrameIndex++;
    renderFrame(state.frames[state.currentFrameIndex]);
  }
}

function stepBackward() {
  pause();
  if (state.currentFrameIndex > 0) {
    state.currentFrameIndex--;
    renderFrame(state.frames[state.currentFrameIndex]);
  }
}

function jumpToStart() {
  pause();
  state.currentFrameIndex = 0;
  renderFrame(state.frames[0]);
}

function jumpToEnd() {
  pause();
  state.currentFrameIndex = state.frames.length - 1;
  renderFrame(state.frames[state.currentFrameIndex]);
}

function updatePlayButtonIcon() {
  const btn = document.getElementById('playPauseBtn');
  if (!btn) return;
  if (state.isPlaying) {
    btn.innerHTML = `<svg viewBox="0 0 24 24"><path d="M6 19h4V5H6v14zm8-14v14h4V5h-4z"/></svg>`;
  } else {
    btn.innerHTML = `<svg viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></svg>`;
  }
}

// ==========================================
// Array Generation Helpers
// ==========================================

function randomizeArray() {
  const size = state.arraySize;
  state.array = [];
  for (let i = 0; i < size; i++) {
    state.array.push(Math.floor(Math.random() * 85) + 12);
  }
  generateFrames();
}

function reverseArray() {
  state.array.sort((a, b) => b - a);
  generateFrames();
}

function nearlySortedArray() {
  state.array.sort((a, b) => a - b);
  if (state.array.length >= 4) {
    const i = Math.floor(Math.random() * (state.array.length - 2));
    const t = state.array[i];
    state.array[i] = state.array[i + 1];
    state.array[i + 1] = t;
  }
  generateFrames();
}

// ==========================================
// C++ Trace JSON Importer
// ==========================================

function loadTraceJSON(jsonText) {
  try {
    const data = JSON.parse(jsonText);
    if (!data.frames || !Array.isArray(data.frames)) {
      alert("Invalid trace JSON: missing 'frames' array.");
      return;
    }

    pause();
    state.frames = data.frames.map(f => {
      // Convert C++ frame structure if needed
      return {
        kind: f.kind === 2 ? 'bars' : (f.kind === 0 ? 'list' : (f.kind === 3 ? 'tree' : 'bars')),
        message: f.message || '',
        codeLine: f.codeLine !== undefined ? f.codeLine : -1,
        comparisons: f.comparisons || 0,
        swaps: f.swaps || 0,
        cells: (f.cells || []).map(c => {
          let role = 'normal';
          if (c.role === 1) role = 'checking';
          else if (c.role === 2) role = 'pivot';
          else if (c.role === 3) role = 'replaced';
          else if (c.role === 4) role = 'unswapped';
          else if (c.role === 5) role = 'done';
          return { value: c.value, role };
        }),
        markers: f.markers || [],
        nodes: f.nodes || [],
        rootId: f.rootId || 1
      };
    });

    state.currentFrameIndex = 0;
    const stageTitle = document.getElementById('stageTitle');
    if (stageTitle) stageTitle.textContent = data.title || "Imported C++ Execution Trace";

    if (data.code && Array.isArray(data.code)) {
      const codeContainer = document.getElementById('codeViewport');
      codeContainer.innerHTML = '';
      data.code.forEach((lineText, idx) => {
        const lineDiv = document.createElement('div');
        lineDiv.className = 'code-line';
        lineDiv.innerHTML = `<span class="code-line-num">${idx + 1}</span><span class="code-line-text">${escapeHtml(lineText)}</span>`;
        codeContainer.appendChild(lineDiv);
      });
    }

    renderFrame(state.frames[0]);
    alert(`Loaded C++ recording "${data.title || 'Trace'}" with ${state.frames.length} steps!`);
  } catch (err) {
    alert("Error parsing JSON file: " + err.message);
  }
}

// ==========================================
// UI Updates & Event Listeners
// ==========================================

function updateUI() {
  updateTheoryAndCode();
  if (state.frames.length > 0) {
    renderFrame(state.frames[state.currentFrameIndex]);
  }
}

function setupEventListeners() {
  // Navigation Tabs
  const navTabs = document.querySelectorAll('.nav-tab');
  navTabs.forEach(tab => {
    tab.addEventListener('click', () => {
      navTabs.forEach(t => t.classList.remove('active'));
      tab.classList.add('active');
      const cat = tab.getAttribute('data-cat');
      state.currentCategory = cat;

      // Update pills for category
      updatePillsForCategory(cat);
    });
  });

  // Playback Controls
  document.getElementById('playPauseBtn')?.addEventListener('click', togglePlay);
  document.getElementById('stepFwdBtn')?.addEventListener('click', stepForward);
  document.getElementById('stepBackBtn')?.addEventListener('click', stepBackward);
  document.getElementById('startBtn')?.addEventListener('click', jumpToStart);
  document.getElementById('endBtn')?.addEventListener('click', jumpToEnd);

  // Timeline Slider
  const slider = document.getElementById('timelineSlider');
  slider?.addEventListener('input', (e) => {
    pause();
    state.currentFrameIndex = parseInt(e.target.value, 10);
    renderFrame(state.frames[state.currentFrameIndex]);
  });

  // Speed Selector
  const speedSelect = document.getElementById('speedSelect');
  speedSelect?.addEventListener('change', (e) => {
    state.playbackSpeed = parseFloat(e.target.value);
    if (state.isPlaying) {
      pause();
      play();
    }
  });

  // Array Size Slider
  const sizeSlider = document.getElementById('arraySizeSlider');
  sizeSlider?.addEventListener('input', (e) => {
    state.arraySize = parseInt(e.target.value, 10);
    document.getElementById('sizeLabel').textContent = state.arraySize;
    randomizeArray();
  });

  // Quick Action Buttons
  document.getElementById('randomizeBtn')?.addEventListener('click', randomizeArray);
  document.getElementById('reversedBtn')?.addEventListener('click', reverseArray);
  document.getElementById('nearlySortedBtn')?.addEventListener('click', nearlySortedArray);

  // Custom Input Modal
  const customModal = document.getElementById('customInputModal');
  document.getElementById('customInputBtn')?.addEventListener('click', () => {
    customModal?.classList.add('open');
  });
  document.getElementById('modalCancelBtn')?.addEventListener('click', () => {
    customModal?.classList.remove('open');
  });
  document.getElementById('modalApplyBtn')?.addEventListener('click', () => {
    const inputStr = document.getElementById('customArrayInput').value;
    const parts = inputStr.split(',').map(s => parseInt(s.trim(), 10)).filter(n => !isNaN(n));
    if (parts.length > 0) {
      state.array = parts.slice(0, 30);
      state.arraySize = state.array.length;
      document.getElementById('sizeLabel').textContent = state.arraySize;
      customModal?.classList.remove('open');
      generateFrames();
    }
  });

  // C++ File Importer
  const fileInput = document.getElementById('traceFileInput');
  document.getElementById('importTraceBtn')?.addEventListener('click', () => {
    fileInput?.click();
  });
  fileInput?.addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onload = (evt) => loadTraceJSON(evt.target.result);
      reader.readAsText(file);
    }
  });

  // Drag and Drop
  window.addEventListener('dragover', (e) => e.preventDefault());
  window.addEventListener('drop', (e) => {
    e.preventDefault();
    if (e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      const file = e.dataTransfer.files[0];
      if (file.name.endsWith('.json')) {
        const reader = new FileReader();
        reader.onload = (evt) => loadTraceJSON(evt.target.result);
        reader.readAsText(file);
      }
    }
  });

  // Keyboard Shortcuts
  window.addEventListener('keydown', (e) => {
    if (e.target.tagName === 'INPUT') return;
    if (e.code === 'Space') {
      e.preventDefault();
      togglePlay();
    } else if (e.code === 'ArrowRight') {
      e.preventDefault();
      stepForward();
    } else if (e.code === 'ArrowLeft') {
      e.preventDefault();
      stepBackward();
    } else if (e.code === 'Home') {
      jumpToStart();
    } else if (e.code === 'End') {
      jumpToEnd();
    }
  });
}

function updatePillsForCategory(cat) {
  const container = document.getElementById('algoSelectors');
  container.innerHTML = '';

  const pillDefs = {
    sorting: [
      { id: 'bubble', label: 'Bubble Sort' },
      { id: 'selection', label: 'Selection Sort' },
      { id: 'insertion', label: 'Insertion Sort' },
      { id: 'merge', label: 'Merge Sort' },
      { id: 'quick', label: 'Quick Sort' },
      { id: 'heap', label: 'Heap Sort' }
    ],
    list: [
      { id: 'singlyList', label: 'Singly Linked' },
      { id: 'doublyList', label: 'Doubly Linked' },
      { id: 'circularList', label: 'Circular Linked' }
    ],
    tree: [
      { id: 'bst', label: 'Binary Search Tree' },
      { id: 'avl', label: 'AVL Rotations' }
    ],
    queue: [
      { id: 'stackQueue', label: 'Stack & Queue' }
    ],
    graph: [
      { id: 'graphs', label: 'BFS & Traversal' }
    ],
    hash: [
      { id: 'hashing', label: 'Hash Table Probing' }
    ]
  };

  const pills = pillDefs[cat] || pillDefs.sorting;
  state.currentAlgo = pills[0].id;

  pills.forEach((p, idx) => {
    const btn = document.createElement('button');
    btn.className = `pill-btn ${idx === 0 ? 'active' : ''}`;
    btn.textContent = p.label;
    btn.addEventListener('click', () => {
      document.querySelectorAll('.pill-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      state.currentAlgo = p.id;
      pause();
      generateFrames();
    });
    container.appendChild(btn);
  });

  generateFrames();
}

// Initialize Application on DOM Ready
window.addEventListener('DOMContentLoaded', () => {
  updatePillsForCategory('sorting');
  setupEventListeners();
});
