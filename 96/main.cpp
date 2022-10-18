
// Su Doku(Japanese meaning number place) is the name given to a popular puzzle concept.
// Its origin is unclear, but credit must be attributed to Leonhard Euler who invented a similar, 
// and much more difficult, puzzle idea called Latin Squares.The objective of Su Doku puzzles, however, 
// is to replace the blanks(or zeros) in a 9 by 9 grid in such that each row, column, and 3 by 3 box 
// contains each of the digits 1 to 9. Below is an example of a typical starting puzzle grid and its solution grid.
// 
// |-----------------------|     |-----------------------| 
// |     3 |   2   | 6     |     | 4 8 3 | 9 2 1 | 6 5 7 |
// | 9     | 3   5 |     1 |     | 9 6 7 | 3 4 5 | 8 2 1 |
// |     1 | 8   6 | 4     |     | 2 5 1 | 8 7 6 | 4 9 3 |
// |-------+-------+-------|     |-------+-------+-------|
// |     8 | 1   2 | 9     |     | 5 4 8 | 1 3 2 | 9 7 6 |
// | 7     |       |     8 |     | 7 2 9 | 5 6 4 | 1 3 8 |
// |     6 | 7   8 | 2     |     | 1 3 6 | 7 9 8 | 2 4 5 |
// |-------+-------+-------|     |-------+-------+-------|
// |     2 | 6   9 | 5     |     | 3 7 2 | 6 8 9 | 5 1 4 |
// | 8     | 2   3 |     9 |     | 8 1 4 | 2 5 3 | 7 6 9 |
// |     5 |   1   | 3     |     | 6 9 5 | 4 1 7 | 3 8 2 |
// |-----------------------|     |-----------------------|
//
// A well constructed Su Doku puzzle has a unique solution and can be solved by logic, although it may be necessary 
// to employ "guess and test" methods in order to eliminate options(there is much contested opinion over this).
// The complexity of the search determines the difficulty of the puzzle; the example above is considered easy because 
// it can be solved by straight forward direct deduction.
// 
// The 6K text file, sudoku.txt contains fifty different Su Doku puzzles 
// ranging in difficulty, but all with unique solutions(the first puzzle in the file is the example above).
// 
// By solving all fifty puzzles find the sum of the 3 - digit numbers found in the top left corner of each solution grid; 
// for example, 483 is the 3 - digit number found in the top left corner of the solution grid above.

#include <windows.h>
#include <vector>
#include <fstream>
#include <assert.h>
#include <functional>
#include <unordered_map>
#include <algorithm>

static bool s_testTheThing = false;
static constexpr int NUM_ROWS_COLS = 9;
static char s_buf[1024];

#define ITERATE_CELLS()                                       \
        for (int row = 0; row < NUM_ROWS_COLS; row++)         \
        {                                                     \
            for (int col = 0; col < NUM_ROWS_COLS; col++)     \
            {                                                 \
                    GridCell& cell = cells[row][col];

#define END_ITERATE_CELLS()                                   \
            }                                                 \
        }      

#define ITERATE_UNKNOWN_CELLS()                               \
            ITERATE_CELLS()                                   \
            if (cells[row][col].value == 0)                   \
            {                                                          
                                                                 
#define END_ITERATE_UNKNOWN_CELLS()                           \
            }                                                 \
            END_ITERATE_CELLS();
                                             
                                                                       
#define ITERATE_UNKNOWN_CELLS_BY_GROUP()                               \
            ITERATE_UNKNOWN_CELLS()                                    \
            for (int i = 0; i < 3; i++)                                \
            {                                                          \
                int compareRow = ((row / 3) * 3) + i;                  \
                for (int j = 0; j < 3; j++)                            \
                {                                                      \
                    int compareCol = ((col / 3) * 3) + j;              \
                    if (compareRow == row && compareCol == col)        \
                    {                                                  \
                        continue;                                      \
                    }                                                  \
                    if (cells[compareRow][compareCol].value == 0)      \
                    {                                                  \
                        continue;                                      \
                    }                                                  \
                    GridCell& compareCell = cells[compareRow][compareCol]; \

#define END_ITERATE_UNKNOWN_CELLS_BY_GROUP()                           \
                }                                                      \
            }                                                          \
            END_ITERATE_UNKNOWN_CELLS()

//---------------------------------------------------------------------------------------------------------------------
static const bool SameGroup(int row, int col, int otherRow, int otherCol)
{
    return ((row / 3) == (otherRow / 3)) && ((col / 3) == (otherCol / 3));
}
static bool CanBeSeen(int row, int col, int otherRow, int otherCol)
{
    if (row == otherRow)
        return true;
    if (col == otherCol)
        return true;
    if (SameGroup(row, col, otherRow, otherCol))
        return true;
    return false;
}
//---------------------------------------------------------------------------------------------------------------------
struct PossibleValues
{
    std::vector<int> v;
    bool Remove(int toRemove)
    {
        auto it = std::remove_if(v.begin(), v.end(), [toRemove](int val) { return val == toRemove; });
        if (it != v.end())
        {
            v.erase(it, v.end());
            return true;
        }
        return false;
    }
    bool HasValues() const { return v.size() > 0; }
    bool HasNumValues(int num) const { return v.size() == num; }
    bool Contains(int value) { 
        for (int compare : v)
        {
            if(compare == value)
                return true;
        }
        return false;
    }
};
//---------------------------------------------------------------------------------------------------------------------
struct GridCell
{
    int value = 0;
    PossibleValues possibleValues;
};
//---------------------------------------------------------------------------------------------------------------------
struct Grid
{
    GridCell cells[NUM_ROWS_COLS][NUM_ROWS_COLS];
    
    bool Solve();

    bool CheckForSolvedCells();
    bool ExclusiveRows();
    bool ExclusiveColumns();
    bool ExclusiveGroups();
    bool LockingPair();
    bool LockingTriplets();
    bool HiddenSingles();
    bool HiddenPairs();
    bool XWing();
    bool YWing();
    bool PointingPairs();

    bool Validate() const;
    bool IsBoardInvalid() const;

    void DrawBoard(bool completedCells = true) const;
};
//---------------------------------------------------------------------------------------------------------------------
static std::vector<Grid*> s_grids;
//---------------------------------------------------------------------------------------------------------------------
void read_file(const char* file_name)
{
    // This function assumes proper formatting of the file passed in:
    //  - the word 'Grid' on a line before each grid
    //  - exactly 9 characters of integers on exactly 9 rows after the line with the word 'Grid'
    //  - no extra whitespace or lines between each grid
    std::ifstream myReadFile;
    myReadFile.open(file_name);
    char line[512];
    if (myReadFile.is_open())
    {
        int rowIndex = 0;
        while (!myReadFile.eof()) 
        {
            myReadFile.getline(line, 512);
            if (line[0] == 'G' && line[1] == 'r' && line[2] == 'i' && line[3] == 'd')
            {
                s_grids.push_back(new Grid());
                rowIndex = 0;
                continue;
            }

            Grid* grid = s_grids.back();
            for (int i = 0; i < NUM_ROWS_COLS; i++)
            {
                grid->cells[rowIndex][i].value = (int)(line[i] - '0');

                if (grid->cells[rowIndex][i].value == 0)
                {
                    // fill out possible values, all besides itself
                    for (int j = 1; j < 10; j++)
                    {
                        grid->cells[rowIndex][i].possibleValues.v.push_back(j);
                    }
                }

            }
            rowIndex++;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
bool Grid::IsBoardInvalid() const
{
    for (int i = 0; i < NUM_ROWS_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS_COLS; j++)
        {
            if (cells[i][j].value == 0 && !cells[i][j].possibleValues.HasValues())
            {
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------
bool Grid::Validate() const
{
    // no empty entries...
    for (int row = 0; row < NUM_ROWS_COLS; row++)
    {
        for (int col = 0; col < NUM_ROWS_COLS; col++)
        {
            if (cells[row][col].value == 0)
                return false;
        }
    }

    // no duplicate cells in a row
    for (int row = 0; row < NUM_ROWS_COLS; row++)
    {
        bool numFound[10] = { false };
        for (int col = 0; col < NUM_ROWS_COLS; col++)
        {
            int value = cells[row][col].value;
            if (numFound[value])
            {
                return false;
            }
            numFound[value] = true;
        }
    }

    // no duplicate cells in a column
    for (int col = 0; col < NUM_ROWS_COLS; col++)
    {
        bool numFound[10] = { false };
        for (int row = 0; row < NUM_ROWS_COLS; row++)
        {
            int value = cells[row][col].value;
            if (numFound[value])
            {
                return false;
            }
            numFound[value] = true;
        }
    }

    // no duplicate cells in a square
    bool numFound[10] = { false };
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            int value = cells[row][col].value;
            if (numFound[value])
            {
                return false;
            }
            numFound[value] = true;
        }
    }

    // must be valid
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool Grid::Solve()
{
    bool madeProgress = true;
    while (madeProgress)
    {
        madeProgress = false;

        // First, fill in the cells value if there is only one valid choice.
        // this should be the only part of this loop modifying 'value'
        madeProgress |= CheckForSolvedCells();

        // Then, run all our heuristics to see if we can eliminate possible values
        // these should only modify the 'possibleValues' array
        madeProgress |= ExclusiveRows();
        madeProgress |= ExclusiveColumns();
        madeProgress |= ExclusiveGroups();
        if (madeProgress)
            continue;  // bail here because other algorithms get more computational

        madeProgress |= LockingPair();
        madeProgress |= LockingTriplets();
        madeProgress |= HiddenSingles();
        madeProgress |= HiddenPairs();
        madeProgress |= XWing();
        madeProgress |= YWing();
        madeProgress |= PointingPairs();

        if (IsBoardInvalid())
        {
            assert(false);
        }
    }
    bool solved = Validate();

    return solved;
}
//---------------------------------------------------------------------------------------------------------------------
// CheckForSolvedCells
//   If a cell has only one possible pval, then the value must be that.
//---------------------------------------------------------------------------------------------------------------------
bool Grid::CheckForSolvedCells()
{
    bool madeChanges = false;
    ITERATE_UNKNOWN_CELLS()
    {
        if (cell.possibleValues.HasNumValues(1))
        {
            cell.value = cell.possibleValues.v[0];
            madeChanges = true;
        }
    }
    END_ITERATE_UNKNOWN_CELLS()
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// ExclusiveRows
//    If a row already contains a known value, no other cell in that group can also contain that value, so remove
//    it from the pvals of the rest of the row
//---------------------------------------------------------------------------------------------------------------------
bool Grid::ExclusiveRows()
{
    bool madeChanges = false;
    ITERATE_UNKNOWN_CELLS()
    {
        for (int i = 0; i < NUM_ROWS_COLS; i++)
        {
            if (i == row)
            {
                continue;
            }

            int cellValue = cells[i][col].value;
            if (cellValue == 0)
            {
                continue;
            }
            madeChanges |= cell.possibleValues.Remove(cellValue);
        }
    }
    END_ITERATE_UNKNOWN_CELLS();
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// ExclusiveColumns
//    If a column already contains a known value, no other cell in that group can also contain that value, so remove
//    it from the pvals of the rest of the column
//---------------------------------------------------------------------------------------------------------------------
bool Grid::ExclusiveColumns()
{
    bool madeChanges = false;
    ITERATE_UNKNOWN_CELLS()
    {
        for (int i =  0; i < NUM_ROWS_COLS; i++)
        {
            if (i == col)
            {
                continue;
            }

            int cellValue = cells[row][i].value;
            if (cellValue ==  0)
            {
                continue;
            }

            madeChanges |= cell.possibleValues.Remove(cellValue);
        }
    }
    END_ITERATE_UNKNOWN_CELLS()
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// ExclusiveGroups
//    If a group already contains a known value, no other cell in that group can also contain that value, so remove
//    it from the pvals of the rest of the group
//---------------------------------------------------------------------------------------------------------------------
bool Grid::ExclusiveGroups()
{
    bool madeChanges = false;
    ITERATE_UNKNOWN_CELLS_BY_GROUP();
    {
        int cellValue = compareCell.value;
        madeChanges |= cell.possibleValues.Remove(cellValue);
    }
    END_ITERATE_UNKNOWN_CELLS_BY_GROUP();

    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// Locking Pair
//   This heuristic states that if there is a pair of cells that both exclusively contain the same pvals 
//   in the same row, column, or group then those cells will eventually become those values, and 
//   you can eliminate the pair from the pvals of the other cells in the row, column, or group.
// 
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
// |       |   2   |       ||       |       |       ||       |   2   |       |
// |       | 4   6 | 4     ||       | 4     |       ||       |       |       |
// |       | 7 8   | 7     ||       | 7     |       || 7 8   | 7 8   |       |
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//                     ^               ^
//                     +---------------+
//                     locking pair (4,7)
// 
// Because columns 3 and 5 both have exactly a (4,7) as pvals, no other column in the row can have (4,7)... 
// so we can update the pvals to be this:
// 
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
// |       |   2   |       ||       |       |       ||       |   2   |       |
// |       |     6 | 4     ||       | 4     |       ||       |       |       |
// |       |   8   | 7     ||       | 7     |       ||   8   |   8   |       |
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
// 
//---------------------------------------------------------------------------------------------------------------------
bool Grid::LockingPair()
{
    struct RowCol {
        int row;
        int col;
        RowCol(int r, int c) { row = r; col = c; }
    };
    struct IntPair {
        int a; int b;
        IntPair(int _a, int _b) : a(_a), b(_b) {}
    };
    struct IntPairHash {
        size_t operator()(const IntPair& k) const { return k.a << 16 | k.b & 0xFFFF; }
    };
    struct IntPairEqual {
        bool operator()(const IntPair& lhs, const IntPair& rhs) const { return lhs.a == rhs.a && lhs.b == rhs.b; }
    };
    std::unordered_map<IntPair, std::vector<RowCol>, IntPairHash, IntPairEqual> mapOfPairs;
    ITERATE_UNKNOWN_CELLS()
    {
        if(cell.possibleValues.HasNumValues(2))
        {
           mapOfPairs[IntPair(cell.possibleValues.v[0], cell.possibleValues.v[1])].push_back(RowCol(row,col));
        }
    }
    END_ITERATE_UNKNOWN_CELLS()
    
    bool madeChanges = false;
    for(const auto& pair : mapOfPairs)
    {
        // first is the pair tuple
        // second is a vector of all row/cols those pairs were found in
        if(pair.second.size() == 1)
        {
            continue;
        }
       
        // we found the same pair twice, could be a locking pair.. 
        // see if any of the entries have the same row or col or group
        for(auto outerIt = pair.second.begin(); outerIt != pair.second.end(); ++outerIt)
        {
            for(auto innerIt = outerIt + 1; innerIt != pair.second.end(); ++innerIt)
            {
                if(outerIt->row == innerIt->row)
                {
                    // locking pair of row
                    for(int i = 0; i < NUM_ROWS_COLS; i++)
                    {
                        if(i == outerIt->col || i == innerIt->col)
                            continue;
                        
                        auto& cell = cells[outerIt->row][i];
                        if(cell.value != 0)
                            continue;

                        madeChanges |= cell.possibleValues.Remove(pair.first.a);
                        madeChanges |= cell.possibleValues.Remove(pair.first.b);
                    }
                }
                
                if(outerIt->col == innerIt->col)
                {
                    // locking pair of col
                    for (int i = 0; i < NUM_ROWS_COLS; i++)
                    {
                        if (i == outerIt->row || i == innerIt->row)
                            continue;

                        auto& cell = cells[i][outerIt->col];
                        if (cell.value != 0)
                            continue;

                        madeChanges |= cell.possibleValues.Remove(pair.first.a);
                        madeChanges |= cell.possibleValues.Remove(pair.first.b);
                    }
                }
                
                if (SameGroup(outerIt->row, outerIt->col, innerIt->row, innerIt->col))
                {
                    // locking candidates in a group
                    for (int i = 0; i < 3; i++)
                    {
                        int row = ((outerIt->row / 3) * 3) + i;
                        for (int j = 0; j < 3; j++)
                        {
                            int col = ((outerIt->col / 3) * 3) + j;
                            if(row == outerIt->row && col == outerIt->col)
                                continue;
                            if(row == innerIt->row && col == innerIt->col)
                                continue;

                            auto& cell = cells[row][col];
                            if (cell.value != 0)
                                continue;

                            madeChanges |= cell.possibleValues.Remove(pair.first.a);
                            madeChanges |= cell.possibleValues.Remove(pair.first.b);
                        }
                    }
                }
            }
        }
    }
   
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// LockingTriplets
//   Similar to LockingPairs, if any three cells contain exclusively three values (note all cells don't need to contain
//   all values... i.e. one can be (5,3) one can be (5,6) and one can be (5,6,3) and that is a valid locking triplet), 
//   then we can remove those values from the other cells in the row, column or group
// 
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//  |       |   2   |   2   ||       |       |       ||   2   |   2   |       |
//  |       | 4 5   | 4     ||       |       |       || 4 5   | 4 5   |       |
//  |       |       |       ||       |       |       ||       |     9 |       |
//  |-----------------------||-----------------------||-----------------------|
//              ^      ^                                  ^
//              +------+----------------------------------+
//                  locking triplet of (2,4,5)
// 
// In the above example, the three cells can collectively only be (2,4,5) so we can remove those pvals from the
// rest of the row like this:
// 
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//  |       |   2   |   2   ||       |       |       ||   2   |       |       |
//  |       | 4 5   | 4     ||       |       |       || 4 5   |       |       |
//  |       |       |       ||       |       |       ||       |     9 |       |
//  |-----------------------||-----------------------||-----------------------|
//                                                     
//---------------------------------------------------------------------------------------------------------------------
bool Grid::LockingTriplets()
{
    struct RowCol {
        int row;
        int col;
        RowCol(int r, int c) { row = r; col = c; }
    };
    struct IntTriplet {
        int a; int b; int c;
        IntTriplet(int _a, int _b, int _c) {
            std::vector<int> v;
            v.push_back(_a);
            v.push_back(_b);
            v.push_back(_c);
            std::sort(v.begin(), v.end());
            a = v[0];
            b = v[1];
            c = v[2];
        };
    };
    struct IntTripletHash {
        // these values are all between 0-10, so only need 4 bits per
        size_t operator()(const IntTriplet& k) const { return (k.a & 0xF) << 8 | (k.b & 0xF) << 4 | (k.c & 0xF); }
    };
    struct IntTripletEqual {
        bool operator()(const IntTriplet& lhs, const IntTriplet& rhs) const { return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c; }
    };

    std::unordered_map<IntTriplet, std::vector<RowCol>, IntTripletHash, IntTripletEqual> mapOfTriplets;
    ITERATE_UNKNOWN_CELLS()
    {
        if (cell.possibleValues.HasNumValues(2))
        {
            // Insert all possibilities for a third number
            for (int i = 1; i < 10; i++)
            {
                mapOfTriplets[IntTriplet(cell.possibleValues.v[0], cell.possibleValues.v[1], i)].push_back(RowCol(row, col));
            }
        }
        else if (cell.possibleValues.HasNumValues(3))
        {
            mapOfTriplets[IntTriplet(cell.possibleValues.v[0], cell.possibleValues.v[1], cell.possibleValues.v[2])].push_back(RowCol(row, col));
        }
    }
    END_ITERATE_UNKNOWN_CELLS()

    bool madeChanges = false;
    for (const auto& triplet : mapOfTriplets)
    {
        if(triplet.second.size() < 3)
            continue;
        
        for (auto firstIt = triplet.second.begin(); firstIt != triplet.second.end(); ++firstIt)
        {
            for (auto secondIt = firstIt + 1; secondIt != triplet.second.end(); ++secondIt)
            {
                for (auto thirdIt = secondIt + 1; thirdIt != triplet.second.end(); ++thirdIt)
                {
                    if (firstIt->row == secondIt->row && firstIt->row == thirdIt->row)
                    {
                        // locking triplet in the row
                        for (int i = 0; i < NUM_ROWS_COLS; i++)
                        {
                            if (i == firstIt->col || i == secondIt->col || i == thirdIt->col)
                                continue;

                            auto& cell = cells[firstIt->row][i];
                            if (cell.value != 0)
                                continue;

                            madeChanges |= cell.possibleValues.Remove(triplet.first.a);
                            madeChanges |= cell.possibleValues.Remove(triplet.first.b);
                            madeChanges |= cell.possibleValues.Remove(triplet.first.c);
                        }
                    }
                    if (firstIt->col == secondIt->col && firstIt->col == thirdIt->col)
                    {
                        // locking triplet in the column
                        for (int i = 0; i < NUM_ROWS_COLS; i++)
                        {
                            if (i == firstIt->row || i == secondIt->row || i == thirdIt->row)
                                continue;

                            auto& cell = cells[i][firstIt->col];
                            if (cell.value != 0)
                                continue;

                            madeChanges |= cell.possibleValues.Remove(triplet.first.a);
                            madeChanges |= cell.possibleValues.Remove(triplet.first.b);
                            madeChanges |= cell.possibleValues.Remove(triplet.first.c);
                        }

                    }
                    if (SameGroup(firstIt->row, firstIt->col, secondIt->row, secondIt->col) && SameGroup(firstIt->row, firstIt->col, thirdIt->row, thirdIt->col))
                    {
                        // locking candidates in a group
                        for (int i = 0; i < 3; i++)
                        {
                            int row = ((firstIt->row / 3) * 3) + i;
                            for (int j = 0; j < 3; j++)
                            {
                                int col = ((firstIt->col / 3) * 3) + j;
                                if (row == firstIt->row && col == firstIt->col)
                                    continue;
                                if (row == secondIt->row && col == secondIt->col)
                                    continue;
                                if (row == thirdIt->row && col == thirdIt->col)
                                    continue;

                                auto& cell = cells[row][col];
                                if (cell.value != 0)
                                    continue;

                                madeChanges |= cell.possibleValues.Remove(triplet.first.a);
                                madeChanges |= cell.possibleValues.Remove(triplet.first.b);
                                madeChanges |= cell.possibleValues.Remove(triplet.first.c);
                            }
                        }
                    }
                }
            }
        }
    }
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// Hidden Singles
//    If a number can only possibly be in a single location, then it must be the value
// 
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//  | 1     |       |       ||       |       | 1     ||       | 1     |       |
//  |       |       |       ||       |       | 4     ||       |       |       |
//  |   8   |       | 7 8   ||       |       |       ||       | 7     |       |
//  |-----------------------||-----------------------||-----------------------|
//                                              ^
//                                             ----
//                                        hidden single (4)
// 
//   In the above example, because the highlighed cell is the only one that can be a 4
//   we can eliminate other possibilities; it must be a 4
//---------------------------------------------------------------------------------------------------------------------
bool Grid::HiddenSingles()
{
    bool madeChanges = false;
    for (int i = 1; i < 10; i++)
    {
        // rows
        for (int row = 0; row < NUM_ROWS_COLS; row++)
        {
            bool valueIsSet = false;
            for (int col = 0; col < NUM_ROWS_COLS; col++)
            {
                if (cells[row][col].value == i)
                {
                    valueIsSet = true;
                    break;
                }
            }
            if (valueIsSet)
                continue;


            std::vector<std::pair<int, int>> withValue;
            for (int col = 0; col < NUM_ROWS_COLS; col++)
            {
                if (cells[row][col].value != 0)
                    continue;

                if (cells[row][col].possibleValues.Contains(i))
                {
                    withValue.push_back(std::make_pair(row, col));
                }
            }
            if (withValue.size() == 1)
            {
                // we can remove all other candidates from these two cells: they must be these values
                for (int v = 1; v < 10; v++)
                {
                    if (v == i)
                        continue;

                    madeChanges |= cells[withValue[0].first][withValue[0].second].possibleValues.Remove(v);
                }
            }
        }


        // columns
        for (int col = 0; col < NUM_ROWS_COLS; col++)
        {
            bool valueIsSet = false;
            for (int row = 0; row < NUM_ROWS_COLS; row++)
            {
                if (cells[row][col].value == i)
                {
                    valueIsSet = true;
                    break;
                }
            }
            if (valueIsSet)
                continue;


            std::vector<std::pair<int, int>> withValue;
            for (int row = 0; row < NUM_ROWS_COLS; row++)
            {
                if (cells[row][col].value != 0)
                    continue;

                if (cells[row][col].possibleValues.Contains(i))
                {
                    withValue.push_back(std::make_pair(row, col));
                }
            }
            if (withValue.size() == 1)
            {
                // we can remove all other candidates from these two cells: they must be these values
                for (int v = 1; v < 10; v++)
                {
                    if (v == i)
                        continue;

                    madeChanges |= cells[withValue[0].first][withValue[0].second].possibleValues.Remove(v);
                }
            }
        }

        // groups
        for (int group = 0; group < 9; group++)
        {
            bool valueIsSet = false;
            for (int row = ((group / 3) * 3); row < (((group / 3) * 3) + 3); row++)
            {
                for (int col = ((group % 3) * 3); col < (((group % 3) * 3) + 3); col++)
                {
                    if (cells[row][col].value == i)
                    {
                        valueIsSet = true;
                        break;
                    }
                }
            }
            if (valueIsSet)
                continue;


            std::vector<std::pair<int, int>> withValue;
            for (int row = ((group / 3) * 3); row < (((group / 3) * 3) + 3); row++)
            {
                for (int col = ((group % 3) * 3); col < (((group % 3) * 3) + 3); col++)
                {
                    if (cells[row][col].value != 0)
                        continue;

                    if (cells[row][col].possibleValues.Contains(i))
                    {
                        withValue.push_back(std::make_pair(row, col));
                    }
                }
            }
            if (withValue.size() == 1)
            {
                // we can remove all other candidates from these two cells: they must be these values
                for (int v = 1; v < 10; v++)
                {
                    if (v == i)
                        continue;

                    madeChanges |= cells[withValue[0].first][withValue[0].second].possibleValues.Remove(v);
                }
            }
        }
    }
    return madeChanges;
}


//---------------------------------------------------------------------------------------------------------------------
// Hidden Pairs
//   If a pair of numbers exclusively shows up in two cells in a row, column, or group then that pair must be the only
//   valid pvals for those cells, and you can eliminate the other pvals.
// 
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
// |       | 1     |       ||       |       |       ||       | 1     |       |
// |       | 4   6 | 4     ||       |   5   |       ||       | 4   6 |       |
// |       | 7     | 7     ||       | 7     |       || 7     | 7     |       |
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//            ^                                                  ^
//            +--------------------------------------------------+
//                           hidden pair (1,6)
//
// Because the only two cells to have either a 1 or a 6 are index 1 and 7, we can eliminate the other pvals:
//
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
// |       | 1     |       ||       |       |       ||       | 1     |       |
// |       |     6 | 4     ||       |   5   |       ||       |    6  |       |
// |       |       | 7     ||       | 7     |       || 7     |       |       |
// |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//
//---------------------------------------------------------------------------------------------------------------------
bool Grid::HiddenPairs()
{
    bool madeChanges = false;
    for (int i = 1; i < 10; i++)
    {
        for (int j = i + 1; j < 10; j++)
        {
            // pair (i,j)
            
            // check for hidden pair in rows
            for (int row = 0; row < NUM_ROWS_COLS; row++)
            {
                std::vector<std::pair<int, int>> withPair;
                int withEither = 0;
                for (int col = 0; col < NUM_ROWS_COLS; col++)
                {
                    if(cells[row][col].value != 0)
                        continue;
                    bool containsI = cells[row][col].possibleValues.Contains(i);
                    bool containsJ = cells[row][col].possibleValues.Contains(j);

                    if (containsI && containsJ)
                    {
                        withPair.push_back(std::make_pair(row,col));
                    }
                    if (containsI || containsJ)
                    {
                        withEither++;
                    }
                }
                if (withPair.size() == 2 && withEither == 2)
                {
                    // we can remove all other candidates from these two cells: they must be these values
                    for (int v = 1; v < 10; v++)
                    {
                        if(v == i || v == j)
                            continue;
                        
                        madeChanges |= cells[withPair[0].first][withPair[0].second].possibleValues.Remove(v);
                        madeChanges |= cells[withPair[1].first][withPair[1].second].possibleValues.Remove(v);
                    }
                }
            }

            // check for hidden pair in columns
            for (int col = 0; col < NUM_ROWS_COLS; col++)
            {
                std::vector<std::pair<int,int>> withPair;
                int withEither = 0;
                for (int row = 0; row < NUM_ROWS_COLS; row++)
                {
                    if (cells[row][col].value != 0)
                        continue;
                    bool containsI = cells[row][col].possibleValues.Contains(i);
                    bool containsJ = cells[row][col].possibleValues.Contains(j);

                    if (containsI && containsJ)
                    {
                        withPair.push_back(std::make_pair(row,col));
                    }
                    if (containsI || containsJ)
                    {
                        withEither++;
                    }
                }
                if (withPair.size() == 2 && withEither == 2)
                {
                    // we can remove all other candidates from these two cells: they must be these values
                    for (int v = 1; v < 10; v++)
                    {
                        if (v == i || v == j)
                            continue;
                        
                        madeChanges |= cells[withPair[0].first][withPair[0].second].possibleValues.Remove(v);
                        madeChanges |= cells[withPair[1].first][withPair[1].second].possibleValues.Remove(v);
                    }
                }
            }
            
            // check for hidden pair in groups
            for(int group = 0; group < 9; group++)
            {
                std::vector<std::pair<int,int>> withPair;
                int withEither = 0;
                for(int row = ((group / 3) * 3); row < (((group / 3) * 3) + 3); row++)
                {
                    for(int col = ((group % 3) * 3); col < (((group % 3) * 3) + 3); col++)
                    {
                        if (cells[row][col].value != 0)
                            continue;
                        bool containsI = cells[row][col].possibleValues.Contains(i);
                        bool containsJ = cells[row][col].possibleValues.Contains(j);

                        if (containsI && containsJ)
                        {
                            withPair.push_back(std::make_pair(row, col));
                        }
                        if (containsI || containsJ)
                        {
                            withEither++;
                        }
                    }
                }


                if (withPair.size() == 2 && withEither == 2)
                {
                    // we can remove all other candidates from these two cells: they must be these values
                    for (int v = 1; v < 10; v++)
                    {
                        if (v == i || v == j)
                            continue;

                        madeChanges |= cells[withPair[0].first][withPair[0].second].possibleValues.Remove(v);
                        madeChanges |= cells[withPair[1].first][withPair[1].second].possibleValues.Remove(v);
                    }
                }
            }
        }
    }
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// If a value can only exist in two places in a row or column, and a different row or column shares the same trait, then 
// other cells in the column or row can have the value removed from the list of pvals
// 
//  |-----------------------||-----------------------||-----------------------|
//  |       |       |     3 ||       |   2 3 |       ||       |   2 3 |       |
//  |     6 |       |     6 ||       |       |       ||       |       |       |   <---+ 
//  |     9 | 7   9 |       ||       |     9 |       ||       | 7   9 |       |       |
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|       |
//  |       |       |       ||       |     3 |       || 1     | 1   3 | 1     |       |
//  |       |       |       ||       |       |       ||   5   |   5   |   5   |       | these rows have only two places
//  |       |   8   |       ||       |     9 |       ||     9 |     9 |       |       | for a 2 to be...
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|       |
//  | 1     | 1     |     3 ||       |   2 3 |       ||       | 1 2 3 |       |       |
//  |   5 6 |   5   |     6 || 4     |       | 4     ||     6 |   5   |       |  <----+ 
//  |   8 9 | 7 8 9 |       ||   8   |     9 |   8   ||       | 7   9 | 7     |
//  |-------+-------+-------||-------+-------+-------||-------+-------+-------|
//                                        ^                       ^
//                                        +-----------------------+
//                                        ...and they share a column
// 
//  Therefore all other columns in those rows can have 2 removed
//---------------------------------------------------------------------------------------------------------------------
bool Grid::XWing()
{
    bool madeChanges = false;
    for (int i = 1; i < 10; i++)
    {
        // do rows first
        {
            std::vector<int> values[NUM_ROWS_COLS];
            ITERATE_UNKNOWN_CELLS()
            {
                if (cells[row][col].possibleValues.Contains(i))
                {
                    values[row].push_back(col);
                }
            }
            END_ITERATE_UNKNOWN_CELLS()

            for (int firstRow = 0; firstRow < NUM_ROWS_COLS; firstRow++)
            {
                if (values[firstRow].size() != 2)
                    continue;

                for (int secondRow = 0; secondRow < NUM_ROWS_COLS; secondRow++)
                {
                    if (firstRow == secondRow)
                        continue;

                    if (values[secondRow].size() != 2)
                        continue;

                    if (values[firstRow][0] != values[secondRow][0])
                        continue;

                    if (values[firstRow][1] != values[secondRow][1])
                        continue;

                    // we have a set of pairs in two different rows which share a column - this is an XWing
                    // eliminate i from all other cells of the matching columns
                    for (int r = 0; r < NUM_ROWS_COLS; r++)
                    {
                        if (r == firstRow)
                            continue;
                        if (r == secondRow)
                            continue;

                        madeChanges |= cells[r][values[firstRow][0]].possibleValues.Remove(i);
                        madeChanges |= cells[r][values[firstRow][1]].possibleValues.Remove(i);
                    }
                }
            }
        }
        
        // columns
        {
            std::vector<int> values[NUM_ROWS_COLS];
            ITERATE_UNKNOWN_CELLS()
            {
                if (cells[row][col].possibleValues.Contains(i))
                {
                    values[col].push_back(row);
                }
            }
            END_ITERATE_UNKNOWN_CELLS()

            for (int first = 0; first < NUM_ROWS_COLS; first++)
            {
                if (values[first].size() != 2)
                    continue;

                for (int second = 0; second < NUM_ROWS_COLS; second++)
                {
                    if (first == second)
                        continue;

                    if (values[second].size() != 2)
                        continue;

                    if (values[first][0] != values[second][0])
                        continue;

                    if (values[first][1] != values[second][1])
                        continue;

                    // we have a set of pairs in two different rows which share a column - this is an XWing
                    // eliminate i from all other cells of the matching columns
                    for (int c = 0; c < NUM_ROWS_COLS; c++)
                    {
                        if (c == first)
                            continue;
                        if (c == second)
                            continue;

                        madeChanges |= cells[values[first][0]][c].possibleValues.Remove(i);
                        madeChanges |= cells[values[second][1]][c].possibleValues.Remove(i);
                    }
                }
            }
        }
    }
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// YWing
//   goal is to find two other pairs with the two values
//   i.e. if this pair is (3,8) find a pair that is (3,*)
//   and a pair that is (*,8) -- where the * are the same value -- 
//   and shares either a row, column, or group to the original cell (3,8).  
//   If we can find those properties, then any cell that shares
//   a row, column, or group with both the two * candidates can 
//   have the * eliminated.
//---------------------------------------------------------------------------------------------------------------------
bool Grid::YWing()
{
    bool madeChanges = false;
    struct Pair
    {
        int row;
        int col;
        int values[2];
        Pair(int a, int b, int c, int d) : row(a), col(b), values{c,d} { }
        bool operator==(const Pair& rhs) const { return row == rhs.row && col == rhs.col; }
        static int GetSharedValue(const Pair& a, const Pair& b) {
            if (a.values[0] == b.values[0])
                return a.values[0];
            if (a.values[1] == b.values[0])
                return a.values[1];
            if (a.values[0] == b.values[1])
                return a.values[0];
            if (a.values[1] == b.values[1])
                return a.values[1];
            return 0;
        }
        bool SharesValues(const Pair& other) const {
            return GetSharedValue(*this, other) != 0;
        }

    };

    std::vector<Pair> pairs;
    ITERATE_CELLS()
    {
        if (cell.possibleValues.HasNumValues(2))
        {
            pairs.push_back(Pair(row,col,cell.possibleValues.v[0],cell.possibleValues.v[1]));
        }
    }
    END_ITERATE_CELLS()

    for (const Pair& a : pairs)
    {


        std::vector<Pair> PotentialPairs[2];
        for(int i = 0; i < 2; i++)
        {
            for(const Pair& b : pairs)
            {
                if (a==b)
                    continue;
                
                if(!CanBeSeen(a.row,a.col,b.row,b.col))
                    continue;

                if(a.values[i] == b.values[0] || a.values[i] == b.values[1])
                {
                    PotentialPairs[i].push_back(b);
                }
            }
        }

        for(auto it : PotentialPairs[0])
        {
            // pairs matching a.values[0]
            for(auto innerIt : PotentialPairs[1])
            {
                // pairs matching a.values[1]
                int sharedValue = Pair::GetSharedValue(it, innerIt);
                if(!sharedValue)
                    continue;

                if(sharedValue == a.values[0] || sharedValue == a.values[1])
                    continue;

                ITERATE_UNKNOWN_CELLS()
                {
                    if(row == a.row && col == a.col)
                        continue;
                    if(row == it.row && col == it.col)
                        continue;
                    if(row == innerIt.row && col == innerIt.col)
                        continue;
                    if(CanBeSeen(row,col,it.row,it.col) && CanBeSeen(row,col,innerIt.row,innerIt.col))
                    {
                        madeChanges |= cell.possibleValues.Remove(sharedValue);
                    }
                }
                END_ITERATE_UNKNOWN_CELLS()
            }
        }
    }

    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// Pointing Pairs
//   If any one number occurs twice or three times in just one unit (any row, column or box) then we can remove 
//   that number from the intersection of another unit. There are four types of intersection:
//   - A Pair or Triple in a box - if they are aligned on a row, n can be removed from the rest of the row.
//   - A Pair or Triple in a box - if they are aligned on a column, n can be removed from the rest of the column.
//   - (NYI) A Pair or Triple on a row - if they are all in the same box, n can be removed from the rest of the box.
//   - (NYI) A Pair or Triple on a column - if they are all in the same box, n can be removed from the rest of the box.
//---------------------------------------------------------------------------------------------------------------------
bool Grid::PointingPairs()
{
    bool madeChanges = false;
    for (int i = 1; i < 10; i++)
    {
        
        for(int group = 0; group < 9; group++)
        {
            std::vector<std::pair<int, int>> cellsWithValue;
            for (int row = ((group / 3) * 3); row < (((group / 3) * 3) + 3); row++)
            {
                for (int col = ((group % 3) * 3); col < (((group % 3) * 3) + 3); col++)
                {
                    if(cells[row][col].possibleValues.Contains(i))
                    {
                        cellsWithValue.push_back(std::make_pair(row,col));
                    }
                }
            }

            if (cellsWithValue.size() < 2)
                continue;

            bool allSameRow = true;
            bool allSameCol = true;
            for (auto& cell : cellsWithValue)
            {
                if (cell.first != cellsWithValue[0].first)
                {
                    allSameRow = false;
                }
                if (cell.second != cellsWithValue[0].second)
                {
                    allSameCol = false;
                }
            }

            if (allSameRow)
            {
                for (int col = 0; col < 9; col++)
                {
                    // skip the group we're in
                    int startOfGroup = ((group % 3) * 3);
                    int endOfGroup = (((group % 3) * 3) + 3);
                    if(col >= startOfGroup && col < endOfGroup)
                        continue;

                    madeChanges |= cells[cellsWithValue[0].first][col].possibleValues.Remove(i);
                }
            }

            if (allSameCol)
            {
                for (int row = 0; row < 9; row++)
                {
                    // skip the group we're in
                    int startOfGroup = ((group / 3) * 3);
                    int endOfGroup = (((group / 3) * 3) + 3);
                    if (row >= startOfGroup && row < endOfGroup)
                        continue;

                    madeChanges |= cells[row][cellsWithValue[0].second].possibleValues.Remove(i);
                }
            }
        }
    }
    return madeChanges;
}
//---------------------------------------------------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------------------------------------------------
int main()
{
    read_file("96/sudoku.txt");

    int solvedCount = 0;
    for (int i = 0; i < s_grids.size(); i++)
    {
        bool solved = s_grids[i]->Solve();
        if (!solved)
        {
            snprintf(s_buf, 128,"Failed to solve puzzle %d\n", i);     
            OutputDebugStringA(s_buf);

            OutputDebugStringA("******* Failed board ***********\n");
            s_grids[i]->DrawBoard(false);
            OutputDebugStringA("\n");
        }
        else
        {
            solvedCount++;
            snprintf(s_buf, 128, "SOLVED! %d\n", i);
            OutputDebugStringA(s_buf);
        }
    }

    int sum = 0;
    if (solvedCount == 50)
    {
        for (auto& grid : s_grids)
        {
            sum += grid->cells[0][0].value * 100 + grid->cells[0][1].value * 10 + grid->cells[0][2].value;
        }
    }

    snprintf(s_buf, 1024, "Solved Count: %d - sum = %d\n", solvedCount, sum);

    OutputDebugStringA(s_buf);
	return 0;
}



















//---------------------------------------------------------------------------------------------------------------------
static const char* pvals[] =
{
    " ", "1", "2", "3", "4", "5", "6", "7", "8", "9",
};
//---------------------------------------------------------------------------------------------------------------------
static const char* topBottom  = "|-----------------------||-----------------------||-----------------------|\n";

static const char* endOfGroup = "|-----------------------||-----------------------||-----------------------|\n"
                                "|-----------------------||-----------------------||-----------------------|\n";

static const char* midGroup   = "|-------+-------+-------||-------+-------+-------||-------+-------+-------|\n";

void Grid::DrawBoard(bool completedCells) const
{
    std::string s;

    OutputDebugStringA(topBottom);
    for (int a = 0; a < 9; a++) // row
    {
        if (a == 3 || a == 6)
        {
            OutputDebugStringA(endOfGroup);
        }
        else
        {
            OutputDebugStringA(midGroup);
        }
        for (int b = 0; b < 3; b++) // line in row
        {
            s.clear();
            for (int c = 0; c < 9; c++) // column
            {
                int cellValue = cells[a][c].value;
                s.append(c % 3 == 0 ? "|" : "");
                if (cellValue != 0)
                {
                    if (b == 0)
                    {
                        snprintf(s_buf, 32, "%s      |", completedCells ? "k" : " ");
                        s.append(s_buf);
                    }
                    else if (b == 1)
                    {
                        snprintf(s_buf, 32, "   %s   |", completedCells ? pvals[cellValue] : " ");
                        s.append(s_buf);
                    }
                    else
                    {
                        s.append("       |");
                    }
                }
                else
                {
                    auto& v = cells[a][c].possibleValues.v;
                    bool hasFirst = false;
                    bool hasSecond = false;
                    bool hasThird = false;
                    for (auto& value : cells[a][c].possibleValues.v)
                    {
                        hasFirst |= value == 1 + b * 3;
                        hasSecond |= value == 2 + b * 3;
                        hasThird |= value == 3 + b * 3;
                    }
                    snprintf(s_buf, 1024, " %s %s %s |", hasFirst ? pvals[1 + (b * 3)] : pvals[0], hasSecond ? pvals[2 + (b * 3)] : pvals[0], hasThird ? pvals[3 + (b * 3)] : pvals[0]);
                    s.append(s_buf);
                }
            }
            OutputDebugStringA(s.c_str());
            OutputDebugStringA("\n");
        }

    }
    OutputDebugStringA(topBottom);

    OutputDebugStringA("\n\n");
}