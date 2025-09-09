#include "algorithm.h"
#include <unordered_map>
using namespace std;

// struct STL-MINHEAP
struct Node
{
    string state; // Puzzle
    string path;  // cost
    int g = 0;
    int h = 0;
    int f = 0;
    bool alive = true; // Lazy-update
};

// Comparator for Uniform Cost Search (UC):
struct CmpUC
{
    bool operator()(const Node *a, const Node *b) const
    {
        return a->g > b->g; // reverse STL to MIN-HEAP
    }
};

struct CmpAstar
{
    bool operator()(const Node *a, const Node *b) const
    {
        if (a->f != b->f)
            return a->f > b->f;
        return a->g < b->g;
    }
};

// successors list
vector<pair<string, char>> successors_URDL(Puzzle *cur)
{
    std::vector<std::pair<std::string, char>> res;

    // next U
    if (cur->canMoveUp())
    {
        Puzzle *child = cur->moveUp();
        res.push_back({child->toString(), 'u'});
        delete child;
    }

    // R
    if (cur->canMoveRight())
    {
        Puzzle *child = cur->moveRight();
        res.push_back({child->toString(), 'r'});
        delete child;
    }

    // D
    if (cur->canMoveDown())
    {
        Puzzle *child = cur->moveDown();
        res.push_back({child->toString(), 'd'});
        delete child;
    }

    // L
    if (cur->canMoveLeft())
    {
        Puzzle *child = cur->moveLeft();
        res.push_back({child->toString(), 'l'});
        delete child;
    }

    return res;
}

// tools ignore confilct move
inline bool isInverse(char a, char b)
{
    return (a == 'u' && b == 'd') || (a == 'd' && b == 'u') || (a == 'l' && b == 'r') || (a == 'r' && b == 'l');
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  UC with Strict Expanded List
//
// Move Generator:
//
////////////////////////////////////////////////////////////////////////////////////////////
string uc_explist(string const initialState, string const goalState,
                  int &pathLength, int &numOfStateExpansions, int &maxQLength,
                  float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap,
                  int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions)
{
    // reset stats
    pathLength = 0; numOfStateExpansions = 0; maxQLength = 0;
    actualRunningTime = 0.0f;
    numOfDeletionsFromMiddleOfHeap = 0;
    numOfLocalLoopsAvoided = 0;
    numOfAttemptedNodeReExpansions = 0;

    clock_t startTime = clock();

    // start==goal
    if (initialState == goalState) {
        actualRunningTime = float(clock() - startTime) / CLOCKS_PER_SEC;
        return "";
    }

    // OPEN heap + indexes + CLOSED
    vector<Node*> openHeap;
    unordered_map<string, Node*> inOpen;
    unordered_map<string, int>   bestClosedG;

    // start node
    Node* start = new Node();
    start->state = initialState;
    start->path  = "";
    start->g = 0; start->h = 0; start->f = 0; start->alive = true;

    openHeap.push_back(start);
    make_heap(openHeap.begin(), openHeap.end(), CmpUC{});
    inOpen[start->state] = start;
    maxQLength = (int)openHeap.size();

    while (!openHeap.empty()) {
        // pop min-g
        pop_heap(openHeap.begin(), openHeap.end(), CmpUC{});
        Node* cur = openHeap.back();
        openHeap.pop_back();

        // 弹出就顺手把 inOpen 的这一条清掉（如果还指着它）
        auto itCurOpen = inOpen.find(cur->state);
        if (itCurOpen != inOpen.end() && itCurOpen->second == cur) {
            inOpen.erase(itCurOpen);
        }

        // lazy deletion
        if (!cur->alive) {
            numOfDeletionsFromMiddleOfHeap++;
            delete cur;
            continue;
        }

        // 只有“从堆顶弹出目标”才返回 —— 保证最优
        if (cur->state == goalState) {
            string res = std::move(cur->path);
            pathLength = (int)res.size();
            actualRunningTime = float(clock() - startTime) / CLOCKS_PER_SEC;
            delete cur;
            for (Node* nd : openHeap) delete nd;
            return res;
        }

        // expand
        numOfStateExpansions++;

        // generate URDL successors
        Puzzle curPuzzle(cur->state, goalState);
        auto succs = successors_URDL(&curPuzzle);

        for (auto &pr : succs) {
            const string &ns = pr.first;
            const char mv = pr.second;

            // avoid immediate backtrack
            if (!cur->path.empty() && isInverse(cur->path.back(), mv)) {
                numOfLocalLoopsAvoided++;
                continue;
            }

            int ng = cur->g + 1;

            // CLOSED: worse/equal path -> drop
            auto itCns = bestClosedG.find(ns);
            if (itCns != bestClosedG.end()) {
                numOfAttemptedNodeReExpansions++;
                continue;
            }

            // OPEN: check existing
            auto itOns = inOpen.find(ns);
            if (itOns != inOpen.end()) {
                Node* old = itOns->second;
                if (ng < old->g) {
                    old->alive = false;               // mark old dead
                } else {
                    numOfAttemptedNodeReExpansions++;
                    continue;                         // not better, skip
                }
            }

            // create child and push
            Node* nd = new Node();
            nd->state = ns;
            nd->path  = cur->path; nd->path.push_back(mv);
            nd->g = ng; nd->h = 0; nd->f = ng; nd->alive = true;

            openHeap.push_back(nd);
            push_heap(openHeap.begin(), openHeap.end(), CmpUC{});
            if ((int)openHeap.size() > maxQLength) maxQLength = (int)openHeap.size();
            inOpen[ns] = nd;
        }

        // finished expanding cur -> commit to CLOSED
        auto itCcur = bestClosedG.find(cur->state);
        if (itCcur == bestClosedG.end() || cur->g < itCcur->second) {
            bestClosedG[cur->state] = cur->g;
        }
        delete cur;
    }

    // no solution
    actualRunningTime = float(clock() - startTime) / CLOCKS_PER_SEC;
    for (Node* nd : openHeap) delete nd;
    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  A* with the Strict Expanded List
//
// Move Generator:
//
////////////////////////////////////////////////////////////////////////////////////////////
string aStar_ExpandedList(string const initialState, string const goalState, int &pathLength, int &numOfStateExpansions, int &maxQLength,
                          float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions, heuristicFunction heuristic)
{

    string path;
    clock_t startTime;

    numOfDeletionsFromMiddleOfHeap = 0;
    numOfLocalLoopsAvoided = 0;
    numOfAttemptedNodeReExpansions = 0;

    // cout << "------------------------------" << endl;
    // cout << "<<aStar_ExpandedList>>" << endl;
    // cout << "------------------------------" << endl;
    actualRunningTime = 0.0;
    startTime = clock();
    srand(time(NULL));                   // RANDOM NUMBER GENERATOR - ONLY FOR THIS DEMO.  YOU REALLY DON'T NEED THIS! DISABLE THIS STATEMENT.
    maxQLength = rand() % 200;           // AT THE MOMENT, THIS IS JUST GENERATING SOME DUMMY VALUE.  YOUR ALGORITHM IMPLEMENTATION SHOULD COMPUTE THIS PROPERLY.
    numOfStateExpansions = rand() % 200; // AT THE MOMENT, THIS IS JUST GENERATING SOME DUMMY VALUE.  YOUR ALGORITHM IMPLEMENTATION SHOULD COMPUTE THIS PROPERLY

    //***********************************************************************************************************
    actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);
    path = "DDRRLLLUUURDLUDURDLUU"; // this is just a dummy path for testing the function
    pathLength = path.size();
    return path;
}
