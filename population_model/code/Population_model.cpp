// Population_model.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <stdlib.h>
#include <map>
using namespace std;

ofstream groups_database;
ofstream values_database;
ofstream assessments_database;
ofstream assessments_day_database;
ifstream database;

map <int, string> days;
map <string, string> intensity;
map <string, string> collaboration;
map <string, string> power;
vector <string> groups;

int char_pos(string line, char char_needed, int order)
{
    if (order == 0) {
        return -1;
    }
    int char_counter = 0;
    int quotes_count = 0;
    int brackets_count = 0;
    int i = 0;
    while (line[i] != '\0')
    {
        if (((quotes_count % 2) == 0) && ((brackets_count % 2) == 0))
        {
            if (line[i] == char_needed)
            {
                char_counter++;
            }
            if (char_counter == order)
            {
                return i;
            }
        }
        if (line[i] == '\"')
        {
            quotes_count++;
        }
        if ((line[i] == '(') || (line[i] == ')'))
        {
            brackets_count++;
        }
        i++;
    }
    return line.length();
}

string atribut(int atr_num, string line)
{
    int first_char = char_pos(line, ',', atr_num - 1) + 1;
    int second_char = char_pos(line, ',', atr_num);
    if (first_char > second_char) {
        return "null";
    }
    line = line.substr(first_char, (second_char - first_char));
    if (line.length() == 0)
    {
        return "null";
    }
    else
    {
        return line;
    }
}

string clean_quotes(string line)
{
    int line_length = line.length();
    for (int i = 0; i < line_length; i++)
    {
        if (line[i] == '\"')
        {
                line = line.substr(0, i) + line.substr(i+1, line_length - i-1);
                line_length--;
                i--;
        }
    }
    return line;
}

void create_assessment_day_object(string group_index, int assessment_index, int day_index, string value)
{
    assessments_database << "pm:hasDayAssessment pm:AD_" << group_index << "_" << assessment_index << "_" << day_index+1 << ";" << endl;

    assessments_day_database << "###  http://citygraph.onti.actcognitive.org/population#AD_" << group_index << "_" << assessment_index << "_" << day_index+1 << endl;
    assessments_day_database << "pm:AD_" << group_index << "_" << assessment_index << "_" << day_index+1 << " rdf:type owl:NamedIndividual ," << endl;
    assessments_day_database << "pm:AssessmentDay ;" << endl;
    assessments_day_database << "pm:hasDay pm:" << days[day_index+1] << ";" << endl;
    if (value == "null") {
        value = "0";
    }
    assessments_day_database << "pm:hasUsageValue " << value << "." << endl << endl << endl;
}


void create_assessment_object(string line, string group_index, int assessment_index)
{
    groups_database << "pm:hasAssessment pm:A_" << group_index << "_" << assessment_index << " ;" << endl;

    assessments_database << "###  http://citygraph.onti.actcognitive.org/population#A_" << group_index << "_" << assessment_index << endl;
    assessments_database << "pm:A_" << group_index << "_" << assessment_index << " rdf:type owl:NamedIndividual," << endl;
    assessments_database << "pm:Assessment ;" << endl;
    assessments_database << "pm:hasOCE pm:" << atribut(4, line) << " ;" << endl;
    for (int i = 0; i < 19; i++) {
        create_assessment_day_object(group_index, assessment_index, i, atribut(i + 5, line));
    }
    if (atribut(25, line) != "null") {
        assessments_database << "pm:hasSignificance " << atribut(25, line) << " ;" << endl;
    }
    if (atribut(26, line) != "null") {
    assessments_database << "pm:hasIntensity pm:" << intensity[atribut(26, line)] << " ;" << endl;
    }
    if (atribut(27, line) != "null") {
    assessments_database << "pm:hasD3Minutes " << atribut(27, line) << " ;" << endl;
    }
    if (atribut(28, line) != "null") {
        assessments_database << "pm:hasD2Minutes " << atribut(28, line) << " ;" << endl;
    }
    if (atribut(29, line) != "null") {
        assessments_database << "pm:hasD1Minutes " << atribut(29, line) << " ;" << endl;
    }
    if (atribut(30, line) != "null") {
        assessments_database << "pm:hasPower pm:" << power[atribut(30, line)] << " ;" << endl;
    }
    if (atribut(31, line) != "null") {
        assessments_database << "pm:hasDiversity " << atribut(31, line) << " ;" << endl;
    }
    if (atribut(32, line) != "null") {
        assessments_database << "pm:hasCollaboration pm:" << collaboration[atribut(32, line)] << " ;" << endl;
    }
        assessments_database << "pm:hasGroup pm:" << group_index << " ." << endl << endl << endl;
}

void create_value_function_object(string line, string group_index, int value_index)
{
    if (value_index < 10) 
    {
        groups_database << "                    pm:hasValueFunction pm:" << group_index << "_VF_0" << value_index << " ;" << endl;
        values_database << "###  http://citygraph.onti.actcognitive.org/population#" << group_index << "_VF_0" << value_index << endl;
        values_database << "pm:" << group_index << "_VF_0" << value_index << " rdf:type owl:NamedIndividual ," << endl;
    }
    else
    {
        groups_database << "                    pm:hasValueFunction pm:" << group_index << "_VF_" << value_index << " ;" << endl;
        values_database << "###  http://citygraph.onti.actcognitive.org/population#" << group_index << "_VF_" << value_index << endl;
        values_database << "pm:" << group_index << "_VF_" << value_index << " rdf:type owl:NamedIndividual ," << endl;
    }

    values_database << "                pm:ValueFunction ;" << endl;
    values_database << "        pm:hasOCEFuctionValue " << atribut(4, line) << " ;" << endl;
    values_database << "pm:hasOCE pm:ex_" << atribut(1, line) << " ." << endl << endl << endl;
}

void create_value_structure_object(string line, string group_index, int value_index) 
{
    groups_database << "                    pm:hasValueStructure pm:" << group_index << "_VS_0" << value_index << " ;" << endl;

    values_database << "###  http://citygraph.onti.actcognitive.org/population#" << group_index << "_VS_0" << value_index << endl;
    values_database << "pm:" << group_index << "_VS_0" << value_index << " rdf:type owl:NamedIndividual ," << endl;
    values_database << "                pm:ValueStructure ;" << endl;
    values_database << "        pm:hasTopOCEValue " << atribut(4, line) << " ;" << endl;
    if (atribut(5, line) != "null") {
        values_database << "        pm:hasTopOCESecure " << atribut(5, line) << " ;" << endl;
    }
    if (atribut(6, line) != "null") {
        values_database << "        pm:hasTopOCEQuality " << atribut(6, line) << " ;" << endl;
    }
    values_database << "pm:hasOCE pm:ex_" << atribut(1,line) << " ." << endl << endl << endl;
}

void create_group_object(string group)
{
    string group_index = atribut(1, group);
    string income = atribut(2, group);
    string start_date = atribut(3, group);
    string end_date = atribut(4, group);
    string isPensioner = atribut(5, group);
    string label = atribut(6, group);
    groups_database << "###  http://citygraph.onti.actcognitive.org/population#" << group_index << endl;
    groups_database << "pm:" << group_index << " rdf:type owl:NamedIndividual," << endl;
    if (group_index.substr(0, 1) == "S") {
        groups_database << "pm:Situation ;" << endl;
    }
    else
    {
        groups_database << "pm:" << group_index.substr(0, 4) << " ;" << endl;
    }
    string object;
    getline(database, object);
    int assessment_index = 1;
    while (atribut(1, object) != "VALUE_STRUCTURE") {
        if (atribut(4, object) != "null") {
            create_assessment_object(object, group_index, assessment_index);
            assessment_index++;
        }
        getline(database, object);
    }
    getline(database, object);
    int value_structure_index = 1;
    while (atribut(1, object) != "VALUE_FUNCTION") {
        if (atribut(1, object) != "null") {
            create_value_structure_object(object, group_index, value_structure_index);
            value_structure_index++;
        }
        getline(database, object);
    }
    getline(database, object);
    int value_function_index = 1;
    while (atribut(1, object) != "null") {
            create_value_function_object(object, group_index, value_function_index);
            value_function_index++;
            getline(database, object);
    }
    if (start_date != "null") {
        groups_database << "pm:hasStartDate " << start_date << " ;" << endl;
    }
    if (end_date != "null") {
        groups_database << "pm:hasEndDate " << end_date << " ;" << endl;
    }
    if (income != "null") {
        groups_database << "pm:hasIncome " << income << " ;" << endl;
    }
    if (isPensioner != "null") {
        if (isPensioner != "1") {
            groups_database << "pm:isPensioner \"true\"^^xsd:boolean ;" << endl;
        }
        else {
            groups_database << "pm:isPensioner \"false\"^^xsd:boolean ;" << endl;
        }
    }
        groups_database << "rdfs:label \"" << label << "\" ." << endl << endl << endl;
}

void create_service_map() {
    ifstream service_database("service_database.txt");
    string line;
    getline(service_database, line);
    while (line != "") {
        if (atribut(1, line) == "Day") {
            days[atoi(atribut(3, line).c_str())] = atribut(2, line);
        }
        if (atribut(1, line) == "Collaboration") {
            collaboration[atribut(3, line)] = atribut(2, line);
        }
        if (atribut(1, line) == "Power") {
            power[atribut(3, line)] = atribut(2, line);
        }
        if (atribut(1, line) == "Intensity"){
            intensity[atribut(3, line)] = atribut(2, line);
        }
        getline(service_database, line);
    }
    getline(database, line);
    getline(database, line);
    while (atribut(1, line) != "null") {
        string group = atribut(3,line)+ ","+ atribut(5, line) + "," + atribut(6, line) + "," + atribut(7, line) + "," + atribut(8, line) + "," + atribut(9, line);
        groups.push_back(group);
        getline(database, line);
    }
}

int main()
{
    database.open("database.csv");
    groups_database.open("groups_database.txt");
    values_database.open("values_database.txt");
    assessments_database.open("assessments_database.txt");
    assessments_day_database.open("assessments_day_database.txt");
    string line = "";
    create_service_map();
    int groups_count = groups.size();
    cout << groups_count;
    int group_index = 0;
    while (group_index < groups_count) {
        if (atribut(5, line) == "Dwd") {
            create_group_object(groups[group_index]);
            group_index++;
            cout << group_index;
        }
        getline(database, line);
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
