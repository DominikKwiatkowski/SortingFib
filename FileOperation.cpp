#include "FileOperation.h"


File::File(string fileName, Mode mode)
{
    this->file = fopen(fileName.c_str(), "a+b");
    this->fileName = fileName;
    if (file == NULL)
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
    this->readPageCounter++;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < PARAM; j++)
        {
            if(fread(&this->buffer[i][j], sizeof(double), 1, file)==0)
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

    fwrite(this->buffer,sizeof(double),line*PARAM,file);

    this->line = 0;
    this->writePageCounter++;
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
        fclose(file);
        remove(fileName.c_str());
        this->file = fopen(fileName.c_str(), "a+b");
        if (file == NULL)
        {
            cout << "Exception during opening file!";
        }
        line = 0;
        mode = Output;
    }
    else
    {
        writePage();
        fclose(file);
        this->file = fopen(fileName.c_str(), "a+b");
        if (file == NULL)
        {
            cout << "Exception during opening file!";
        }
        mode = Input;
        readPage();
    }
    fileEnd = false;
}
