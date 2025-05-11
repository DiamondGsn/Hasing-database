Name Database Search System

## Program Description
This C++ program implements a database system for storing and searching personal names (surname, first name, and patronymic). Key features include:
- Loading data from a text file
- Displaying all records in a formatted table
- Searching by either full or abbreviated name formats
- Hash-based search optimization
- Search statistics and comparison metrics

## Input File Format
Data must be stored in `database.txt` with the following format:
    Surname;Firstname;Patronymic

Each record should be on a separate line.

## How to Use
1. Compile the program: `g++ lab8.cpp -o lab8`
2. Run the executable: `./lab8`
3. Enter search queries in either format:
   - Full name (e.g., "Ivanov Ivan Ivanovich")
   - Abbreviated name (e.g., "Ivanov I.I." or "Ivanov I.I")
4. Type "exit" to quit the program

## Implementation Details
- Uses DJB2 hash algorithm for efficient searching
- Input normalization (uppercase conversion, whitespace handling)
- Supports both full and abbreviated name formats
- Provides search statistics (hash and string comparisons)
- Sorted output display

## Sample Queries
- `Ivanov Ivan Ivanovich` - full name search
- `Kuznetsov M.S.` - abbreviated name search
- `Pyatin P.A.` - Latin alphabet search (if present in database)

## Requirements
- C++17 compatible compiler
- `database.txt` file in the program directory

## Database Structure
The program processes each record to create:
- Full name (Surname Firstname Patronymic)
- Short name (Surname F.P.)
- Full name hash
- Short name hash

## Search Algorithm
1. Query normalization and parsing
2. Hash calculation for the query
3. Two-phase comparison:
   - Initial hash comparison (fast)
   - String comparison (exact match)
4. Results display with match details

## Output Format
Search results show:
- Original query
- Match type (full/abbreviated)
- Query hash value
- Matching records (if any)
- Performance statistics

Note: The program handles various input formats (with/without spaces around dots) through automatic normalization.
