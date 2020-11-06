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
    
    // name of file
    string fileName;
    
    // number of records in one page
    static constexpr int N = BUFFOR_SIZE / PARAM / sizeof(double);

    //number of empty series in this file
    int emptySeries = 0;

    int writePageCounter = 0;
    int readPageCounter = 0;

    bool fileEnd = false;


    File(string fileName, Mode mode);

    //funtion to read/write records
    int  readRecord(double record[PARAM]);
    void writeRecord(double record[PARAM]);

    //function to change file from source/destination to destination/source
    void changeMode();

private:
    void readPage();
    void writePage();

    int line = 0;
    int linesReaded = 0;
    double buffer[N][PARAM];
    // file stream
    FILE* file;
    // mode of file, to prevent reading from destination file and writing to source file
    Mode mode;
};
