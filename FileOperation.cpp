#include "FileOperation.h"


File::File(string fileName, Mode mode)
{
    this->fileStream.open(fileName, fstream::in | fstream::out |fstream::app);
    this->fileName = fileName;
    if (!this->fileStream.good())
    {
        cout << "Exception during opening file!";
    }
    this->mode = mode;

    if( mode == Input)
    {
        readPage();
    }
}

void File::readPage()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < PARAM; j++)
        {
            if(!(this->fileStream >> this->buffer[i][j]))
            {
                linesReaded = i;
                this->line = 0;
                return;
            }
        }
    }
    linesReaded = N;
    this->line = 0;
}

void File::writePage()
{
    for (int i = 0; i < line; i++)
    {
        for (int j = 0; j < PARAM; j++)
        {
            this->fileStream << static_cast<int>(this->buffer[i][j])<<" ";
        }
        this->fileStream << endl;
    }
    this->line = 0;
}

int File::readRecord(double record[PARAM])
{
    if (mode == Output)
        return -1;

    if (line >= linesReaded)
        return -2;

    for (int i=0;i<PARAM;i++)
        record[i] = buffer[line][i];

    line++;
    if (line == 100)
        readPage();

    return 0;
}


void File::writeRecord(double record[PARAM])
{
    for (int i = 0; i < PARAM; i++)
        buffer[line][i] = record[i];
    line++;

    if(line==N)
    {
        writePage();
    }

}


void File::changeMode()
{
    if(mode == Input)
    {
        fileStream.close();
        remove(fileName.c_str());
        fileStream.open(fileName, fstream::in | fstream::out | fstream::app);
        line = 0;
        mode = Output;
    }
    else
    {
        writePage();
        fileStream.close();
        fileStream.open(fileName);
        mode = Input;
        readPage();
    }
}

void File::refresh()
{
    if (mode == Input)
    {
        fileStream.close();

        fileStream.open(fileName);
        readPage();
    }
    else
    {
        writePage();
        fileStream.close();
        fileStream.open(fileName);
    }
}

