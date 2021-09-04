//TODO: во время чтения файла срабаотывает переход на последнюю строку, которой нет
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

struct Cell{
    int row = 0;
    int column = 0;
    std::string meaning = "";
    std::string calculation = ""; 
};

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr << "Too few arguments, need 1" << "\n";
        return -1;
    }
    std::string filepath{argv[1]};
    std::cout << "Filepath: " << filepath << "\n";

    std::ifstream file(filepath);
    if (file.is_open()){
        std::cout<<"Open file\n";
    }else{
        std::cout<<"Not open file\n";
        return -1;
    }

    std::vector <std::string> title_column;
    std::vector <std::string> title_row;

    std::string delimiter = ",";
    int pos = 0;
    std::string token;

    std::vector <Cell> cells;
    int count_rows = 0;
    while(file){
        std::string str;
        std::getline(file, str);
        std::cout<<str<<"\n";

        if (count_rows == 0){
            while (str.find(delimiter) != std::string::npos) {
                pos = str.find(delimiter);
                token = str.substr(0, pos);
                std::cout<<token<<"\n";
                title_column.push_back(token);
                str.erase(0, pos + delimiter.length());
            }
            std::cout<<str<<"\n";
            title_column.push_back(str);
        }else{
            int count_columns = 0;
            while (str.find(delimiter) != std::string::npos) {
                pos = str.find(delimiter);
                token = str.substr(0, pos);
                std::cout << token << "\n";
                if (count_columns == 0){
                    title_row.push_back(token);
                }else{
                    Cell temp_cell;
                    temp_cell.row = count_rows-1;
                    temp_cell.column = count_columns;
                    if (token.find("=") != std::string::npos){
                        temp_cell.calculation = token;
                    } else{                        
                        temp_cell.meaning = token;
                    }
                    cells.push_back(temp_cell);
                }
                str.erase(0, pos + delimiter.length());
                count_columns++;
            }
            std::cout<<str<<"\n";
            Cell temp_cell;
            temp_cell.row = count_rows-1;
            temp_cell.column = count_columns;
            if (str.find("=") != std::string::npos){
                temp_cell.calculation = str;
            } else{                
                temp_cell.meaning = str;
            }
            cells.push_back(temp_cell);
        }
        count_rows++;
    }

    file.close();
    std::cout<<"column:\n";
    for (size_t j = 0; j < title_column.size(); j++){
        std::cout<<"-----"<<title_column[j]<<"\n";
    }
    std::cout<<"row:\n";
    for (size_t j = 0; j < title_row.size(); j++){
        std::cout<<"-----"<<title_row[j]<<"\n";
    }
    std::cout<<"count_rows = "<<count_rows<<"\n";

    for (size_t i = 0; i < cells.size(); i++){
        std::cout<<"row = "<<cells[i].row<<" column = "<<cells[i].column<< " meaning = "<<cells[i].meaning<<" calculation = "<<cells[i].calculation<<"\n";
    }


    return 0;
}