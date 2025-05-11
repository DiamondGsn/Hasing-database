#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <tuple>
#include <iomanip>
#include <unordered_map>

using namespace std;

struct Record {
    string full_name;
    string short_name;
    uint32_t full_hash;
    uint32_t short_hash;

    bool operator<(const Record& other) const {
        if (full_hash != other.full_hash)
            return full_hash < other.full_hash;
        return short_hash < other.short_hash;
    }
};

uint32_t hash_string(const string &str) noexcept {
    uint32_t hash = 5381;
    for (unsigned char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string normalize(const string &str) {
    string result;
    for (char c : str) {
        if (c == '.' || c == ' ') {
            result += c;
        } else if (isalpha(c)) {
            result += toupper(c);
        }
    }
    return result;
}

string make_short_name(const string &surname, const string &name, const string &patronymic) {
    if (name.empty() || patronymic.empty()) return surname;
    return surname + " " + name[0] + "." + patronymic[0] + ".";
}

vector<tuple<string, string, string>> load_database(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: Не удалось открыть файл " << filename << endl;
        exit(1);
    }

    vector<tuple<string, string, string>> database;
    string line;

    while (getline(file, line)) {
        auto parts = split(line, ';');
        if (parts.size() == 3) {
            database.emplace_back(parts[0], parts[1], parts[2]);
        }
    }

    return database;
}

void search(const vector<Record>& records, const string &query) {
    string norm_query = normalize(query);

    size_t dot_pos;
    while ((dot_pos = norm_query.find(" .")) != string::npos) {
        norm_query.replace(dot_pos, 2, ".");
    }
    while ((dot_pos = norm_query.find(". ")) != string::npos) {
        norm_query.replace(dot_pos, 2, ".");
    }
    while ((dot_pos = norm_query.find("..")) != string::npos) {
        norm_query.replace(dot_pos, 2, ".");
    }

    norm_query.erase(unique(norm_query.begin(), norm_query.end(), 
        [](char a, char b) { return a == ' ' && b == ' '; }), norm_query.end());

    size_t start = norm_query.find_first_not_of(" ");
    size_t end = norm_query.find_last_not_of(" ");
    if (start != string::npos && end != string::npos) {
        norm_query = norm_query.substr(start, end - start + 1);
    }

    vector<string> query_parts;
    istringstream iss(norm_query);
    string part;
    while (iss >> part) {
        if (part != ".") {
            query_parts.push_back(part);
        }
    }


    bool is_short_form = (query_parts.size() == 2) && 
                        (query_parts[1].size() == 3 || query_parts[1].size() == 4) && 
                        (query_parts[1].find('.') != string::npos);

    string search_query;
    uint32_t query_hash;

    if (is_short_form) {
  
        if (query_parts[1].size() == 3) {
            query_parts[1] = query_parts[1] + "."; 
        }
        search_query = query_parts[0] + " " + query_parts[1];
        query_hash = hash_string(search_query);
    } else if (query_parts.size() >= 3) {
        search_query = query_parts[0] + " " + query_parts[1] + " " + query_parts[2];
        query_hash = hash_string(search_query);
    } else {
        cout << "Некорректный формат запроса. Используйте:\n"
             << "- Полное ФИО (Иванов Иван Иванович)\n"
             << "- Сокращенное ФИО (Иванов И.И. или Иванов И.И)\n";
        return;
    }

    vector<Record> matches;
    int hash_comparisons = 0;
    int string_comparisons = 0;

    for (const auto& record : records) {
        hash_comparisons++;
        if (is_short_form) {
            if (record.short_hash == query_hash) {
                string_comparisons++;
                if (record.short_name == search_query) {
                    matches.push_back(record);
                }
            }
        } else {
            if (record.full_hash == query_hash) {
                string_comparisons++;
                if (record.full_name == search_query) {
                    matches.push_back(record);
                }
            }
        }
    }

    cout << "\n=== Результаты поиска ===\n";
    cout << "Запрос: " << query << "\n";
    cout << "Тип: " << (is_short_form ? "Сокращенная форма" : "Полная форма") << "\n";
    cout << "Хэш: " << query_hash << "\n\n";

    if (!matches.empty()) {
        cout << "Найдено совпадений: " << matches.size() << "\n";
        for (const auto& match : matches) {
            cout << "----------------------------------------\n";
            cout << "Полное ФИО: " << match.full_name << "\n";
            cout << "Сокращенное ФИО: " << match.short_name << "\n";
            cout << "Хэш полный: " << match.full_hash << "\n";
            cout << "Хэш сокращенный: " << match.short_hash << "\n";
        }
    } else {
        cout << "Совпадений не найдено.\n";
    }

    cout << "\nСтатистика:\n";
    cout << "Сравнений хэшей: " << hash_comparisons << "\n";
    cout << "Сравнений строк: " << string_comparisons << "\n";
}

int main() {
    auto database = load_database("database.txt");

    vector<Record> records;
    for (const auto& [surname, name, patronymic] : database) {
        string norm_surname = normalize(surname);
        string norm_name = normalize(name);
        string norm_patronymic = normalize(patronymic);
        
        string full_name = norm_surname + " " + norm_name + " " + norm_patronymic;
        string short_name = make_short_name(norm_surname, norm_name, norm_patronymic);

        records.push_back({
            full_name,
            short_name,
            hash_string(full_name),
            hash_string(short_name)
        });
    }

    sort(records.begin(), records.end());

    cout << "======================================= База данных ======================================" << endl;
    cout << left << setw(35) << "Полное ФИО" << setw(25) << "Сокращенное ФИО" 
         << setw(19) << "       " << "Полный хэш" << "    " << " Сокращенный хэш\n";
    cout << string(80, '-') << endl;

    for (const auto& record : records) {
        cout << setw(35) << record.full_name 
             << setw(25) << record.short_name
             << setw(15) << record.full_hash 
             << record.short_hash << endl;
    }

    while (true) {
        string query;
        cout << "\nВведите ФИО для поиска (или 'exit' для выхода): ";
        getline(cin, query);

        if (query == "exit") break;
        if (query.empty()) continue;

        search(records, query);
    }

    cout << "Программа завершена.\n";
    return 0;
}