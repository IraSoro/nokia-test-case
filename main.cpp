#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <charconv>
#include <utility>
#include <system_error>

namespace {
std::vector<std::string> split_string(std::string line, char del) {
    std::vector<std::string> parsing_line;
    int pos{};
    std::string token;

    while (line.find(del) != std::string::npos) {
        pos = line.find(del);
        token = line.substr(0, pos);
        parsing_line.push_back(token);
        line.erase(0, pos + sizeof(del));
    }
    parsing_line.push_back(line);
    return parsing_line;
}
}

struct Expression {
    std::variant<int64_t, std::string> left;
    std::variant<int64_t, std::string> right;
    char op{};
};

class Cell {
public:
    int64_t row{};
    int64_t column{};
    int64_t value;
    std::string calculation;

    Expression calculate_value() {
        if (calculation.empty()) {
            return {};
        }
        const std::string arithm_operation = "+-*/";
        for(auto &op : arithm_operation) {
            size_t pos = calculation.find(op);
            if(pos == std::string::npos) {
                continue;
            }

            std::string left = calculation.substr(0, pos);
            std::string right = calculation.substr(pos + 1, calculation.size());
            
            return Expression {
                select_value(left),
                select_value(right),
                op
            };

        }
        return {};
    }

private:
    std::variant <int64_t, std::string> select_value(std::string_view token){
        int result{}; 
        auto [ptr, ec] { std::from_chars(token.data(), token.data() + token.size(), result) };
        if (ec == std::errc::invalid_argument){
            return token.data();
        }else{
            return result;
        }
    }

};

class Table{
    private:
        std::vector<std::string> lines;
        std::vector<std::string> title_column;
        std::vector<std::string> title_row;
        std::vector<Cell> cells;
        const char delimiter = ',';
        
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
                    cell.row = i;
                    cell.column = j;
                    if (line[j][0] == '='){
                        cell.calculation = line[j];
                        cell.calculation.erase(0, 1);
                    }else{
                        std::from_chars(line[j].data(), line[j].data() + line[j].size(), cell.value);
                    }
                    cells.push_back(cell);
                }
            }
        }

        void get_values(){
            for (size_t i = 0; i < cells.size(); i++) {
                if (!cells[i].calculation.empty()) {
                    Expression exp;
                    exp = cells[i].calculate_value();
                    int64_t left_value = get_value_arg(exp.left);
                    int64_t right_value = get_value_arg(exp.right);
                    cells[i].value = calculate_values(left_value, right_value, exp.op);
                }
            }
        }

        int get_value_arg(std::variant<int64_t, std::string> val){
            if (std::holds_alternative<int64_t>(val)){
                return std::get<int64_t>(val);
            }else{
                return define_value(std::get<std::string>(val));
            }
        }
        
        int64_t define_value(std::string str){
            int64_t column_arg = column_search(str);
            str.erase(0, title_column[column_arg].size());
            int64_t row_arg = row_search(str);

            for (size_t i = 0; i < cells.size(); i++) {
                if (cells[i].column == column_arg && cells[i].row == row_arg) {
                    return cells[i].value;
                }
            }          

            std::cout<<"Error: cell \""<<str<<"\" does not exist\n";
            exit(2);
        }

        int column_search(std::string_view value_cell) {
            for (size_t i = 1; i < title_column.size(); i++) {
                if (value_cell.find(title_column[i]) != std::string::npos) {
                    return i;
                }
            }
            return {};
        }

        int row_search(std::string_view value_cell) {
            for (size_t i = 0; i < title_row.size(); i++) {
                if (title_row[i].size() == value_cell.size()
                    && (value_cell.find(title_row[i]) != std::string::npos)) {
                    return ++i;
                }
            }
            return {};
        }

        int calculate_values(int l, int r, char op) {
            std::map<char, std::function<int()>> operations {
                {'+', [&l, &r] { return l + r; }},
                {'-', [&l, &r] { return l - r; }},
                {'*', [&l, &r] { return l * r; }},
                {'/', [&l, &r] { return l / r; }}
            };
            auto operation = operations.find(op);
            if (operation == operations.end()) {
                std::cout << "unknown arithmetic operation\n";
                return 0;
            }
            return operation->second();
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

            t.print_file_content();

            t.split_title_column();
            t.split_title_row();
            t.split_cells();
            // for (size_t i = 0; i < t.cells.size(); i++){
            //     std::cout<<i<<". "<<"column = "<<t.cells[i].column<<"  row = "<<t.cells[i].row<<"  value = "<<t.cells[i].value<<"\n";
            // }
            t.get_values();

            return t;
        }
        
        void print_file_content(){
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
