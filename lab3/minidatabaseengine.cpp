#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using namespace std;

const unsigned int PRIMARY_KEY = 1;
const unsigned int NOT_NULL = 2;
const unsigned int UNIQUE = 4;

class Column {
    string columnName;
    string columnType;
    unsigned int columnConstraints;

public:
    Column(string name, string type, unsigned int constraints) {
        columnName = name;
        columnType = type;
        columnConstraints = constraints;
    }

    string getName() { return columnName; }
    string getType() { return columnType; }
    unsigned int getConstraints() { return columnConstraints; }

    bool isPrimaryKey() { return (columnConstraints & PRIMARY_KEY) != 0; }
    bool isNotNull() { return (columnConstraints & NOT_NULL) != 0; }
    bool isUnique() { return (columnConstraints & UNIQUE) != 0; }
};

class Row {
    vector<string> rowValues;

public:
    Row(vector<string> values) {
        rowValues = values;
    }

    vector<string> getValues() { return rowValues; }

    string getValue(int index) {
        if (index >= 0 && index < (int)rowValues.size())
            return rowValues[index];
        throw out_of_range("Invalid column index");
    }

    int getSize() { return rowValues.size(); }
};

class Table {
    string tableName;
    vector<Column> columnList;
    vector<Row*> rowList;

public:
    Table(string name, vector<Column> columns) {
        tableName = name;
        columnList = columns;
    }

    ~Table() {
        for (int i = 0; i < (int)rowList.size(); i++) {
            delete rowList[i];
        }
        rowList.clear();
    }

    void addRow(Row* newRow) {
        rowList.push_back(newRow);
    }

    string getTableName() { return tableName; }
    vector<Column>& getColumns() { return columnList; }
    vector<Row*>& getRows() { return rowList; }

    int getColumnCount() { return columnList.size(); }
    int getRowCount() { return rowList.size(); }
};

Table* createTable() {
    try {
        char tableNameInput[256];
        int columnCount;

        cout << "CREATE TABLE (Enter table name): ";
        cin >> tableNameInput;

        cout << "Enter number of columns: ";
        cin >> columnCount;

        if (columnCount <= 0)
            throw invalid_argument("Column count must be positive");

        vector<Column> tempColumnList;

        for (int i = 0; i < columnCount; i++) {
            char columnNameInput[256];
            char columnTypeInput[256];
            unsigned int constraintFlags = 0;

            cout << "Column " << i + 1 << " name: ";
            cin >> columnNameInput;

            cout << "Type (int/string): ";
            cin >> columnTypeInput;

            cout << "Constraints (0=None, 1=PK, 2=NotNull, 4=Unique, e.g. 3=PK+NotNull): ";
            cin >> constraintFlags;

            tempColumnList.push_back(
                Column(columnNameInput, columnTypeInput, constraintFlags)
            );
        }

        Table* newTable = new Table(tableNameInput, tempColumnList);

        cout << "Table created successfully.\n\n";
        return newTable;
    }
    catch (exception& e) {
        cout << "Error creating table: " << e.what() << "\n";
        return nullptr;
    }
}

void insertIntoTable(Table* tablePtr) {
    try {
        if (!tablePtr)
            throw runtime_error("Table does not exist");

        vector<string> newRowData;

        cout << "INSERT INTO " << tablePtr->getTableName() << " VALUES:\n";

        for (int i = 0; i < tablePtr->getColumnCount(); i++) {
            char valueInput[256];

            Column currentColumn = tablePtr->getColumns()[i];

            cout << currentColumn.getName() << " (" << currentColumn.getType() << "): ";
            cin >> valueInput;

            string valueString(valueInput);

            if (currentColumn.isNotNull() && valueString == "null") {
                throw runtime_error(currentColumn.getName() + " cannot be null");
            }

            if (currentColumn.isPrimaryKey() || currentColumn.isUnique()) {
                for (Row* existingRow : tablePtr->getRows()) {
                    if (existingRow->getValue(i) == valueString) {
                        throw runtime_error("Constraint violation: value already exists");
                    }
                }
            }

            newRowData.push_back(valueString);
        }

        tablePtr->addRow(new Row(newRowData));

        cout << "Record inserted.\n\n";
    }
    catch (exception& e) {
        cout << "Insert Error: " << e.what() << "\n\n";
    }
}

void selectFromTable(Table* tablePtr) {
    try {
        if (!tablePtr)
            throw runtime_error("Table not loaded");

        cout << "SELECT * FROM " << tablePtr->getTableName() << "\n";

        for (Column column : tablePtr->getColumns()) {
            cout << column.getName() << "\t";
        }

        cout << "\n-----------------------------------\n";

        for (Row* row : tablePtr->getRows()) {
            for (string value : row->getValues()) {
                cout << value << "\t";
            }
            cout << "\n";
        }

        cout << "\n";
    }
    catch (exception& e) {
        cout << "Select Error: " << e.what() << "\n";
    }
}

void saveToFile(Table* tablePtr) {
    try {
        if (!tablePtr)
            throw runtime_error("No table to save");

        string fileName = tablePtr->getTableName() + ".txt";

        ofstream outputFile(fileName);

        if (!outputFile)
            throw runtime_error("Cannot open file for saving");

        outputFile << "TABLE " << tablePtr->getTableName() << "\n";

        for (Column column : tablePtr->getColumns()) {
            outputFile << column.getName() << " "
                       << column.getType() << " "
                       << column.getConstraints() << "\n";
        }

        outputFile << "DATA\n";

        for (Row* row : tablePtr->getRows()) {
            for (int i = 0; i < row->getSize(); i++) {
                outputFile << row->getValue(i)
                           << (i == row->getSize() - 1 ? "" : " ");
            }
            outputFile << "\n";
        }

        outputFile.close();

        cout << "Saved to " << fileName << " successfully.\n\n";
    }
    catch (exception& e) {
        cout << "Save Error: " << e.what() << "\n";
    }
}

Table* loadFromFile() {
    try {
        char fileNameInput[256];

        cout << "Enter filename to load (e.g., users.txt): ";
        cin >> fileNameInput;

        ifstream inputFile(fileNameInput);

        if (!inputFile)
            throw runtime_error("File not found");

        string keyword, tableName;

        inputFile >> keyword >> tableName;

        vector<Column> columnList;

        string columnName;
        string columnType;
        unsigned int columnConstraint;

        while (inputFile >> columnName && columnName != "DATA") {
            inputFile >> columnType >> columnConstraint;

            columnList.push_back(
                Column(columnName, columnType, columnConstraint)
            );
        }

        Table* loadedTable = new Table(tableName, columnList);

        while (!inputFile.eof()) {
            vector<string> rowValues;
            string value;

            for (int i = 0; i < (int)columnList.size(); i++) {
                if (inputFile >> value)
                    rowValues.push_back(value);
            }

            if (rowValues.size() == columnList.size()) {
                loadedTable->addRow(new Row(rowValues));
            }
        }

        inputFile.close();

        cout << "Loaded successfully.\n\n";

        return loadedTable;
    }
    catch (exception& e) {
        cout << "Load Error: " << e.what() << "\n";
        return nullptr;
    }
}

int main() {
    int menuChoice = 0;
    Table* currentTable = nullptr;

    try {
        while (menuChoice != 6) {

            cout << "1. Create Table\n"
                 << "2. Insert Row\n"
                 << "3. Select All\n"
                 << "4. Save to File\n"
                 << "5. Load from File\n"
                 << "6. Exit\n"
                 << "Choice: ";

            cin >> menuChoice;
            cout << "\n";

            if (menuChoice == 1) {

                if (currentTable)
                    delete currentTable;

                currentTable = createTable();
            }
            else if (menuChoice == 2) {

                insertIntoTable(currentTable);
            }
            else if (menuChoice == 3) {

                selectFromTable(currentTable);
            }
            else if (menuChoice == 4) {

                saveToFile(currentTable);
            }
            else if (menuChoice == 5) {

                if (currentTable)
                    delete currentTable;

                currentTable = loadFromFile();
            }
        }
    }
    catch (exception& e) {
        cout << "Unexpected Error: " << e.what() << endl;
    }

    if (currentTable)
        delete currentTable;

    return 0;
}