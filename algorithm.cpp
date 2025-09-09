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
string uc_explist(string const initialState, string const goalState, int &pathLength, int &numOfStateExpansions, int &maxQLength,
                  float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions)
{
    // init UC
    pathLength = 0;
    numOfStateExpansions = 0;
    maxQLength = 0;
    actualRunningTime = 0.0f;
    numOfDeletionsFromMiddleOfHeap = 0;
    numOfLocalLoopsAvoided = 0;
    numOfAttemptedNodeReExpansions = 0;
    string path;
    clock_t startTime = clock();
    actualRunningTime = 0.0;

    // corner case
    if (initialState == goalState)
    {
        actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);
        return ""; // empty path
    }

    // MINI-HEAP STL
    vector<Node *> openHeap;                // Big Q
    unordered_map<string, Node *> inOpen;   // Open Queue
    unordered_map<string, int> bestClosedG; // Extened Queue

    // start node
    Node *start = new Node();
    start->state = initialState;
    start->path = "";
    start->g = 0;
    start->h = 0;
    start->f = 0;
    start->alive = true;

    // init heap
    openHeap.push_back(start);
    make_heap(openHeap.begin(), openHeap.end(), CmpUC{}); // self-defined MINI-HEAP based on vector
    inOpen[start->state] = start;
    maxQLength = (int)openHeap.size();

    while (!openHeap.empty())
    {
        pop_heap(openHeap.begin(), openHeap.end(), CmpUC{}); // pop and keep mini-heap
        Node *cur = openHeap.back();
        openHeap.pop_back(); // remove it

        // realse space of closed node
        if (!cur->alive)
        {
            numOfDeletionsFromMiddleOfHeap++;
            delete cur;
            continue;
        }

        if (cur->state == goalState)
        {
            string path = cur->path;
            pathLength = (int)path.size();
            actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);

            delete cur;
            for (Node *nd : openHeap)
                delete nd;
            return path;
        }

        // Expanded finsished
        numOfStateExpansions++;

        auto itC = bestClosedG.find(cur->state);
        if (itC == bestClosedG.end() || cur->g < itC->second)
        {
            bestClosedG[cur->state] = cur->g;
        }

        delete cur;
    }
    
    // no result fund
    actualRunningTime = actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);
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
