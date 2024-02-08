#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

#define Filepath "\\dataset\\"

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
    return 0;
}