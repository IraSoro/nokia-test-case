#include <charconv>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>

namespace {
std::vector<std::string> split_string(std::string line, char del) {
    std::vector<std::string> splitted;
    size_t pos{};
    while ((pos = line.find(del)) != std::string::npos) {
        splitted.push_back(line.substr(0, pos));
        line.erase(0, pos + sizeof(del));
    }
    splitted.push_back(line);
    return splitted;
}
}  // namespace

struct Expression {
    std::variant<int64_t, std::string> left;
    std::variant<int64_t, std::string> right;
    char op{};
};

class Cell {
private:
    size_t row{};
    size_t column{};
    int64_t value;
    std::string str_expr;

    std::variant<int64_t, std::string> get_value(std::string_view token) {
        int result{};
        auto [ptr, ec]{
            std::from_chars(token.data(), token.data() + token.size(), result)};
        if (ec == std::errc::invalid_argument) {
            return token.data();
        } else {
            return result;
        }
    }

public:
    Cell(size_t row, size_t column) : row(row), column(column) {}

    bool compare_by_idx(size_t row_idx, size_t column_idx) {
        if (column == column_idx && row == row_idx) {
            return true;
        }
        return false;
    }

    bool str_expr_empty() {
        if (str_expr.empty()) {
            return true;
        }
        return false;
    }

    void set_str_expr(std::string str_expr) { this->str_expr = str_expr; }

    size_t get_row() { return row; }

    int64_t get_value() { return value; }

    void set_value(int64_t value) { this->value = value; }

    void set_value(int64_t l, int64_t r, char op) {
        value = calculate_values(l, r, op);
    }

    int64_t calculate_values(int64_t l, int64_t r, char op) {
        std::map<char, std::function<int()>> operations{
            {'+', [l, r] { return l + r; }},
            {'-', [l, r] { return l - r; }},
            {'*', [l, r] { return l * r; }},
            {'/', [l, r] {
                 if (r == 0) {
                     throw std::overflow_error("zero division");
                 }
                 return l / r;
             }}};
        auto operation = operations.find(op);
        if (operation == operations.end()) {
            std::cout << "unknown arithmetic operation\n";
            return 0;
        }
        return operation->second();
    }

    Expression get_expression() {
        if (str_expr.empty()) {
            return {};
        }
        std::string_view arithm_operations = "+-*/";
        for (const auto &op : arithm_operations) {
            size_t pos = str_expr.find(op);
            if (pos == std::string::npos) {
                continue;
            }

            return Expression{
                get_value(str_expr.substr(0, pos)),
                get_value(str_expr.substr(pos + 1, str_expr.size())), op};
        }
        return {};
    }
};

class Table {
private:
    std::vector<std::string> title_column;
    std::vector<std::string> title_row;
    std::vector<Cell> cells;
    const char delimiter = ',';

    void split_title_column(std::string first_line) {
        title_column = split_string(first_line, delimiter);
    }

    void split_title_row(std::vector<std::string> lines) {
        for (size_t i = 1; i < lines.size(); i++) {
            title_row.push_back(split_string(lines[i], delimiter)[0]);
        }
    }

    void split_cells(std::vector<std::string> lines) {
        std::vector<std::string> line;
        for (size_t i = 1; i < lines.size(); i++) {
            line = split_string(lines[i], delimiter);
            for (size_t j = 1; j < line.size(); j++) {
                Cell cell(i, j);
                if (line[j][0] == '=') {
                    cell.set_str_expr(line[j].substr(1, line[j].size()));
                } else {
                    int64_t val{};
                    auto [ptr, ec]{std::from_chars(
                        line[j].data(), line[j].data() + line[j].size(), val)};
                    if (ec == std::errc::invalid_argument) {
                        std::cerr << R"(Error: number ")" << line[j]
                                  << R"(" does not exist\n)";
                    } else {
                        cell.set_value(val);
                    }
                }
                cells.push_back(cell);
            }
        }
    }

    void get_values() {
        for (auto &cell : cells) {
            if (cell.str_expr_empty()) {
                continue;
            }
            Expression exp{cell.get_expression()};
            int64_t left{get_value_arg(exp.left)};
            int64_t right{get_value_arg(exp.right)};
            cell.set_value(left, right, exp.op);
        }
    }

    int64_t get_value_arg(std::variant<int64_t, std::string> val) {
        if (std::holds_alternative<int64_t>(val)) {
            return std::get<int64_t>(val);
        }
        return str_expr_to_value(std::get<std::string>(val));
    }

    int64_t str_expr_to_value(std::string str) {
        size_t column_idx = column_search(str);
        str.erase(0, title_column[column_idx].size());
        size_t row_idx = row_search(str);

        for (auto &cell : cells) {
            if (cell.compare_by_idx(row_idx, column_idx)) {
                return cell.get_value();
            }
        }
        std::cerr << R"(Error: cell ")" << str << R"(" does not exist\n)";
        return 0;
    }

    size_t column_search(std::string_view value_cell) {
        for (size_t i = 1; i < title_column.size(); i++) {
            if (value_cell.find(title_column[i]) != std::string::npos) {
                return i;
            }
        }
        return {};
    }

    size_t row_search(std::string_view value_cell) {
        for (size_t i = 0; i < title_row.size(); i++) {
            if (title_row[i].size() == value_cell.size() &&
                (value_cell.find(title_row[i]) != std::string::npos)) {
                return ++i;
            }
        }
        return {};
    }

public:
    Table(std::string_view path) {
        std::ifstream file(path.data());
        if (!file.is_open()) {
            std::cout << "Can't open file " << path << "\n";
            return;
        }
        std::string line;
        std::vector<std::string> lines;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        print_file_content(lines);

        split_title_column(lines[0]);
        split_title_row(lines);
        split_cells(lines);
        get_values();
    }

    void print_file_content(std::vector<std::string> lines) {
        for (size_t i = 0; i < lines.size(); i++) {
            std::cout << lines[i] << "\n";
        }
    }

    void print() {
        std::cout << "Print:\n";

        for (size_t i = 0; i < title_column.size(); i++) {
            std::cout << title_column[i];
            if (i != title_column.size() - 1) {
                std::cout << ",";
            }
        }

        for (size_t i = 0; i < cells.size(); i++) {
            if ((i == 0) || (cells[i].get_row() != cells[i - 1].get_row())) {
                std::cout << "\n" << title_row[cells[i].get_row() - 1] << ",";
            } else {
                std::cout << ",";
            }
            std::cout << cells[i].get_value();
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Too few arguments, need 1"
                  << "\n";
        return -1;
    }
    Table t(argv[1]);
    t.print();

    return 0;
}
