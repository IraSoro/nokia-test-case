#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct CoordinateArg {
    int row = 0;
    int column = 0;
    CoordinateArg(int row = 0, int column = 0) : row{row}, column{column} {}
};

struct Calculation {
    CoordinateArg arg1;
    CoordinateArg arg2;
    char op{};
};

struct Cell {
    int row{};
    int column{};
    std::string value;
    std::string calculation;

    Calculation calculate_value(std::vector<std::string> title_columns,
                                std::vector<std::string> title_rows) {
        if (calculation.empty()) {
            std::exit(2);
        }
        const std::string arithm_operation = "+-*/";
        for (size_t i = 0; i < arithm_operation.size(); i++) {
            if (calculation.find(arithm_operation[i]) != std::string::npos) {
                int pos = calculation.find(arithm_operation[i]);

                std::string token = calculation.substr(0, pos);
                int column_arg1 = column_search(title_columns, token);
                token.erase(0, title_columns[column_arg1].size());
                int row_arg1 = row_search(title_rows, token);
                CoordinateArg arg1(row_arg1, column_arg1);

                calculation.erase(0, pos + 1);
                int column_arg2 = column_search(title_columns, calculation);
                calculation.erase(0, title_columns[column_arg2].size());
                int row_arg2 = row_search(title_rows, calculation);
                CoordinateArg arg2(row_arg2, column_arg2);

                Calculation total{arg1, arg2, arithm_operation[i]};
                return total;
            }
        }
        std::exit(2);
    }

private:
    int column_search(std::vector<std::string> title_columns,
                      std::string value_cell) {
        for (size_t i = 1; i < title_columns.size(); i++) {
            if (value_cell.find(title_columns[i]) != std::string::npos) {
                return i;
            }
        }
        return -1;
    }

    int row_search(std::vector<std::string> title_rows,
                   std::string value_cell) {
        for (size_t i = 0; i < title_rows.size(); i++) {
            if ((value_cell.find(title_rows[i]) != std::string::npos) &&
                title_rows[i].size() == value_cell.size()) {
                return i;
            }
        }
        return -1;
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Too few arguments, need 1"
                  << "\n";
        return -1;
    }

    

    // Table t = Parser::parse_file(argv[1]);
    // t.print();

    std::ifstream file(argv[1]);
    if (file.is_open()) {
        std::cout << "Open file\n";
    } else {
        std::cout << "Not open file\n";
        return -1;
    }

    std::vector<std::string> title_column;
    std::vector<std::string> title_row;

    const char delimiter = ',';
    int pos{};
    std::string token;

    std::vector<Cell> cells;
    int count_rows{};

    std::string line;

    while (getline(file, line)) {
        std::cout << line << "\n";

        if (count_rows == 0) {
            // split_string(line, delimeter);
            // все, что ниже в эту функцию и возвратить в виде векора результат разделенной строки
            while (line.find(delimiter) != std::string::npos) {
                pos = line.find(delimiter);
                token = line.substr(0, pos);
                title_column.push_back(token);
                line.erase(0, pos + sizeof(delimiter));
            }
            title_column.push_back(line);
        } else {
            int count_columns = 0;
            while (line.find(delimiter) != std::string::npos) {
                pos = line.find(delimiter);
                token = line.substr(0, pos);
                if (count_columns == 0) {
                    title_row.push_back(token);
                } else {
                    Cell temp_cell;
                    temp_cell.row = count_rows - 1;
                    temp_cell.column = count_columns;
                    if (token.find("=") != std::string::npos) {
                        token.erase(0, 1);
                        temp_cell.calculation = token;
                    } else {
                        temp_cell.value = token;
                    }
                    cells.push_back(temp_cell);
                }
                line.erase(0, pos + sizeof(delimiter));
                count_columns++;
            }
            Cell temp_cell;
            temp_cell.row = count_rows - 1;
            temp_cell.column = count_columns;
            if (line.find("=") != std::string::npos) {
                line.erase(0, 1);
                temp_cell.calculation = line;
            } else {
                temp_cell.value = line;
            }
            cells.push_back(temp_cell);
        }
        count_rows++;
    }

    file.close();

    for (size_t i = 0; i < cells.size(); i++) {
        if (cells[i].value.empty()) {
            Calculation val;
            val = cells[i].calculate_value(title_column, title_row);
            int val_arg1 = 0;
            int val_arg2 = 0;
            for (size_t j = 0; j < cells.size(); j++) {
                if (cells[j].column == val.arg1.column &&
                    cells[j].row == val.arg1.row) {
                    val_arg1 = std::stoi(cells[j].value);
                }
                if (cells[j].column == val.arg2.column &&
                    cells[j].row == val.arg2.row) {
                    val_arg2 = std::stoi(cells[j].value);
                }
            }
            int temp_value = 0;
            switch (val.op) {
                case '+':
                    temp_value = val_arg1 + val_arg2;
                    break;
                case '-':
                    temp_value = val_arg1 - val_arg2;
                    break;
                case '*':
                    temp_value = val_arg1 * val_arg2;
                    break;
                case '/':
                    temp_value = val_arg1 / val_arg2;
                    break;
                default:
                    std::cout << "unknown arithmetic operation\n";
                    break;
            }
            cells[i].value = std::to_string(temp_value);
        }
    }

    std::cout << "output:\n";
    for (size_t i = 0; i < title_column.size(); i++) {
        std::cout << title_column[i];
        if (i != title_column.size() - 1) {
            std::cout << ",";
        }
    }

    for (size_t i = 0; i < cells.size(); i++) {
        if ((i == 0) || (cells[i].row != cells[i - 1].row)) {
            std::cout << "\n" << title_row[cells[i].row] << ",";
        } else {
            std::cout << ",";
        }
        std::cout << cells[i].value;
    }

    return 0;
}
