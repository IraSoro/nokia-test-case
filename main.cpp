#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <charconv>

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

class Cell {
public:
    int row{};
    int column{};
    std::string value;
    std::string calculation;

    Calculation calculate_value(std::vector<std::string> title_columns,
                                std::vector<std::string> title_rows) {
        if (calculation.empty()) {
            return {};
        }
        const std::string arithm_operation = "+-*/";
        for (size_t i = 0; i < arithm_operation.size(); i++) {
            if (calculation.find(arithm_operation[i]) != std::string::npos) {
                int pos = calculation.find(arithm_operation[i]);
                int row_arg{};
                int column_arg{};

                std::string token = calculation.substr(0, pos);
                column_arg = column_search(title_columns, token);
                token.erase(0, title_columns[column_arg].size());
                row_arg = row_search(title_rows, token);
                CoordinateArg arg1(row_arg, column_arg);

                calculation.erase(0, pos + 1);
                column_arg = column_search(title_columns, calculation);
                calculation.erase(0, title_columns[column_arg].size());
                row_arg = row_search(title_rows, calculation);
                CoordinateArg arg2(row_arg, column_arg);

                Calculation total{arg1, arg2, arithm_operation[i]};
                return total;
            }
        }
        return {};
    }

private:
    int column_search(std::vector<std::string> title_columns,
                      std::string value_cell) {
        for (size_t i = 1; i < title_columns.size(); i++) {
            if (value_cell.find(title_columns[i]) != std::string::npos) {
                return i;
            }
        }
        return {};
    }

    int row_search(std::vector<std::string> title_rows,
                   std::string value_cell) {
        for (size_t i = 0; i < title_rows.size(); i++) {
            if ((value_cell.find(title_rows[i]) != std::string::npos) &&
                title_rows[i].size() == value_cell.size()) {
                return ++i;
            }
        }
        return {};
    }
};

class Table{
    private:
        std::vector <std::string> lines;
        std::vector<std::string> title_column;
        std::vector<std::string> title_row;
        std::vector<Cell> cells;
        const char delimiter = ',';

        std::vector <std::string> split_string(std::string line, char del){
            std::vector <std::string> parsing_line;
            int pos{};
            std::string token;;

            while (line.find(del) != std::string::npos) {
                pos = line.find(del);
                token = line.substr(0, pos);
                parsing_line.push_back(token);
                line.erase(0, pos + sizeof(del));
            }
            parsing_line.push_back(line);
            return parsing_line;
        }
        
        void split_title_column(){
            title_column = split_string(lines[0], delimiter);
        }

        void split_title_row(){
            for (size_t i = 1; i < lines.size(); i++){
                title_row.push_back(split_string(lines[i], delimiter)[0]);
            }
        } 

        void split_cells(){
            std::vector <std::string> line;
            for (size_t i = 1; i < lines.size(); i++){
                line = split_string(lines[i], delimiter);
                for (size_t j = 1; j < line.size(); j++){
                    Cell cell;
                    cell.column = j;
                    cell.row = i;
                    if (line[j].find('=') != std::string::npos){
                        cell.calculation = line[j];
                        cell.calculation.erase(0, 1);
                    }else{
                        cell.value = line[j];
                    }
                    cells.push_back(cell);
                }
            }
        }

        void get_values(){
            for (size_t i = 0; i < cells.size(); i++) {
                if (cells[i].value.empty()) {
                    Calculation val;
                    val = cells[i].calculate_value(title_column, title_row);
                    int val_arg1 {};
                    int val_arg2 {};
                    for (size_t j = 0; j < cells.size(); j++) {
                        if (cells[j].column == val.arg1.column && cells[j].row == val.arg1.row) {
                            std::from_chars(cells[j].value.data(), cells[j].value.data() + cells[j].value.size(), val_arg1);
                        }
                        if (cells[j].column == val.arg2.column && cells[j].row == val.arg2.row) {
                            std::from_chars(cells[j].value.data(), cells[j].value.data() + cells[j].value.size(), val_arg2);
                        }
                    }
                cells[i].value = std::to_string(calculate_values(val_arg1, val_arg2, val.op));
                }
            }
        } 
        int calculate_values(int arg1, int arg2, char op){
            int total_value {};
            switch (op) {
                case '+':
                    total_value = arg1 + arg2;
                    break;
                case '-':
                    total_value = arg1 - arg2;
                    break;
                case '*':
                    total_value = arg1 * arg2;
                    break;
                case '/':
                    total_value = arg1 / arg2;
                    break;
                default:
                    std::cout << "unknown arithmetic operation\n";
                    break;
            }
            return total_value;                
        }     

    public:
        static Table from(std::string_view path) {
            Table t;

            std::ifstream file(path.data());

            if(!file.is_open()) {
                std::cout << "Can't open file " << path << "\n";
                return {};
            }

            std::string line;
            while (getline(file, line)){
                t.lines.push_back(line);
            }

            t.output_file();

            t.split_title_column();
            t.split_title_row();
            t.split_cells();
            // for (size_t i = 0; i < t.cells.size(); i++){
            //     std::cout<<i<<". "<<"column = "<<t.cells[i].column<<"  row = "<<t.cells[i].row<<"  value = "<<t.cells[i].value<<"\n";
            // }
            t.get_values();

            return t;
        }
        
        void output_file(){
            for (size_t i = 0; i < lines.size(); i++){
                std::cout<<lines[i]<<"\n";
            }
        }

        void print(){
            std::cout<<"Print:\n";

            for (size_t i = 0; i < title_column.size(); i++) {
                std::cout << title_column[i];
                if (i != title_column.size() - 1) {
                    std::cout << ",";
                }
            }

            for (size_t i = 0; i < cells.size(); i++) {
                if ((i == 0) || (cells[i].row != cells[i - 1].row)) {
                    std::cout << "\n" << title_row[cells[i].row - 1] << ",";
                } else {
                    std::cout << ",";
                }
                std::cout << cells[i].value;
            }
        }

};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Too few arguments, need 1"
                  << "\n";
        return -1;
    }   
    
    Table t = Table::from(argv[1]);
    t.print();
 
    return 0;
}
