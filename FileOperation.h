#define BUFFOR_SIZE 4000 // number of baits in buffor
#define PARAM 5 //  number of parameters in one record
#include <fstream>
#include <iostream>

using namespace  std;

class File
{
public:
    enum Mode
    {
        Input=0,
        Output=1
    };
    Mode mode;
    string fileName;
    std::fstream fileStream;
    static constexpr int N = BUFFOR_SIZE / PARAM / sizeof(double);
    int line=0;
    int linesReaded = 0;
    int emptySeries = 0;
    int writePageCounter = 0;
    int readPageCounter = 0;
    double buffer[N][PARAM];
    File(string fileName, Mode mode);
    int  readRecord(double record[PARAM]);
    void writeRecord(double record[PARAM]);
    void changeMode();
    void refresh();

private:
    void readPage();
    void writePage();
};
