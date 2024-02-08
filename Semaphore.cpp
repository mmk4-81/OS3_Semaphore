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
double *Lands;
int DataSize = 0;
double totalLands = 0;
int threadcnt = 0;

struct GlobalResult
{
    double *bestResult;
    double bestDiff;
    DWORD bestThreadID;
};

struct MyParam
{
    DWORD threadID;
    GlobalResult *globalResult;
};

DWORD WINAPI F(LPVOID param);

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

    int n = DataSize + 4;

    GlobalResult globalResult;
    globalResult.bestResult = new double[n]();
    globalResult.bestDiff = -1;
    globalResult.bestThreadID = 0;

    MyParam *threadParams = new MyParam[threadCount];
    HANDLE *threadHandle = new HANDLE[threadCount];

    for (int i = 0; i < threadCount; i++)
    {
        threadParams[i].threadID = i + 1;
        threadParams[i].globalResult = &globalResult;

        threadHandle[i] = CreateThread(NULL, 0, F, &threadParams[i], 0, NULL);
        threadcnt++;
        if (!threadHandle[i])
        {
            cout << "Could not create thread, program will terminate." << endl;
            exit(1);
        }
    }
    return 0;
}

DWORD WINAPI F(LPVOID param) {
    
}