#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

#define Filepath "\\dataset\\"
const int threadCount = thread::hardware_concurrency();

double runtime = 0;
double* Lands;
int DataSize = 0;
double totalLands = 0;

struct GlobalResult {
    double* bestResult;
    double bestDiff;
    DWORD bestThreadID;
};

struct MyParam {
    DWORD threadID;
    GlobalResult* globalResult;
};

int main()
{
    string filename;
    cout << "Enter dataset filename(1-6): ";
    cin >> filename;
    ifstream file(Filepath + filename + ".txt");
    if (!file)
    {
        cout << "Cannot open file: " << filename << endl;
        return 1;
    }

    cout << "Enter runtime (seconds): ";
    cin >> runtime;

    double number;
    while (file >> number)
    {
        DataSize++;
        totalLands += number;
    }

    Lands = new double[DataSize];

    file.clear();
    file.seekg(0, ios::beg);

    for (int i = 0; i < DataSize; i++)
    {
        file >> Lands[i];
    }
    file.close();



    return 0;
}