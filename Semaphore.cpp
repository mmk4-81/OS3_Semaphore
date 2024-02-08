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
HANDLE q, rw, r, threads, printSem, parent;

double runtime = 0;
double *Lands;
int DataSize = 0;
double totalLands = 0;
int threadcnt = 0;
int readCount = 0;
bool endthreads;

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

void print(const GlobalResult &result)
{
    cout << "Best result for ThreadID " << result.bestThreadID << ":" << endl;
    cout << "Difference: " << result.bestDiff << endl;
    cout << "-------------------------------------------------------------------------------------------------------" << endl;
}

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

    q = CreateSemaphore(NULL, 1, 1, NULL);
    rw = CreateSemaphore(NULL, 1, 1, NULL);
    r = CreateSemaphore(NULL, 1, 1, NULL);
    threads = CreateSemaphore(NULL, 1, 1, NULL);
    printSem = CreateSemaphore(NULL, 0, 1, NULL);
    parent = CreateSemaphore(NULL, 0, 1, NULL);

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

    while (true)
    {
        WaitForSingleObject(printSem, INFINITE);
        WaitForSingleObject(threads, INFINITE);
        if (endthreads)
        {
            ReleaseSemaphore(threads, 1, NULL);
            break;
        }
        ReleaseSemaphore(threads, 1, NULL);
        print(globalResult);
        ReleaseSemaphore(parent, 1, NULL);
    }

    return 0;
}

DWORD WINAPI F(LPVOID param)
{
    MyParam *input = static_cast<MyParam *>(param);
    DWORD threadID = input->threadID;
    GlobalResult *globalResult = input->globalResult;

    auto start_time = high_resolution_clock::now();
    double localBest = -1;

    srand(static_cast<unsigned>(time(NULL)) + threadID);
    double *distribution = new double[DataSize]();

    while (true)
    {
        double sum[3] = {0, 0, 0};

        for (int i = 0; i < DataSize; i++)
        {
            int randomindex = rand() % 3;
            sum[randomindex] += Lands[i];
            distribution[i] = randomindex;
        }

        double newdiff = abs(sum[0] - 0.4 * totalLands) + abs(sum[1] - 0.4 * totalLands) + abs(sum[2] - 0.2 * totalLands);
        bool writer = false;

        if (newdiff < localBest || localBest == -1)
        {
            WaitForSingleObject(q, INFINITE);
            WaitForSingleObject(r, INFINITE);
            ReleaseSemaphore(q, 1, NULL);
            if (readCount == 0)
            {
                WaitForSingleObject(rw, INFINITE);
            }
            readCount++;

            ReleaseSemaphore(r, 1, NULL);
            localBest = globalResult->bestDiff;

            if (newdiff < globalResult->bestDiff || globalResult->bestDiff == -1)
            {
                writer = true;
            }

            WaitForSingleObject(r, INFINITE);
            readCount--;

            if (readCount == 0)
            {
                ReleaseSemaphore(rw, 1, NULL);
            }
            ReleaseSemaphore(r, 1, NULL);
        }

        if (writer)
        {
            WaitForSingleObject(q, INFINITE);
            WaitForSingleObject(rw, INFINITE);
            ReleaseSemaphore(q, 1, NULL);
            if (newdiff < globalResult->bestDiff || globalResult->bestDiff == -1)
            {
                for (int i = 0; i < DataSize; i++)
                {
                    globalResult->bestResult[i + 1] = distribution[i];
                }

                for (int i = 0; i < 3; i++)
                {
                    globalResult->bestResult[DataSize + i + 1] = sum[i];
                }

                globalResult->bestThreadID = threadID;
                globalResult->bestDiff = newdiff;
                localBest = globalResult->bestDiff;

                ReleaseSemaphore(printSem, 1, NULL);
                WaitForSingleObject(parent, INFINITE);
            }

            ReleaseSemaphore(rw, 1, NULL);
        }

        auto end_time = high_resolution_clock::now();
        auto elapsed_time = duration_cast<duration<double>>(end_time - start_time).count();

        if (elapsed_time >= runtime)
        {
            break;
        }
    }

    WaitForSingleObject(threads, INFINITE);

    if (threadcnt == 1)
    {
        endthreads = true;
        ReleaseSemaphore(printSem, 1, NULL);
    }

    threadcnt--;
    ReleaseSemaphore(threads, 1, NULL);
}