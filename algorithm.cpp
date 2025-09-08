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

    string path;
    clock_t startTime;

    numOfDeletionsFromMiddleOfHeap = 0;
    numOfLocalLoopsAvoided = 0;
    numOfAttemptedNodeReExpansions = 0;
    // Big Q
    vector<Node *> openHeap;

    // Open Queue
    unordered_map<string, Node *> inOpen;

    // Extened Queue
    unordered_map<string, int> bestClosedG;

    // cout << "------------------------------" << endl;
    // cout << "<<uc_explist>>" << endl;
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
