// SortingFib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "FileOperation.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <float.h>

//g(a,y,c,z,x)=10a[x2+3c3z4-5y7]
double g(double a, double y, double c, double z, double x)
{
    //return 10 * a * (pow(x, 2) + 3 * pow(c, 3) * pow(z, 4) - 5 * pow(y, 7));
    return a;
}

void eraseFile(string fileName)
{
    fstream file;
    file.open(fileName, fstream::in | fstream::out | fstream::app);
    file.close();
    remove(fileName.c_str());
}
void generateFile(File *&fileToSort,File*&firstDisk,File*&secondDisk)
{
    int recordNumber;
    fstream generateFile;
    eraseFile("fileToSort.txt");
    generateFile.open("fileToSort.txt", fstream::in | fstream::out | fstream::app);
    cout << "Podaj liczbe rekordow\n";
    cin >> recordNumber;
    srand(time(NULL));
    for (int i = 0; i < recordNumber;i++)
    {
        for(int j=0;j<PARAM;j++)
        {
            int generatedNumber = rand()%128;
            generateFile << generatedNumber << " ";
        }
        generateFile << endl;
    }
    generateFile.close();
    eraseFile("firstDisk.txt");
    eraseFile("secondDisk.txt");
    fileToSort = new File("fileToSort.txt", File::Input);
    firstDisk = new File("firstDisk.txt", File::Output);
    secondDisk = new File("secondDisk.txt", File::Output);
}

void loadFile(File* &fileToSort, File* &firstDisk, File* &secondDisk)
{
    string filename;
    cout << "Podaj sciezke do pliku\n";
    cin >> filename;
    eraseFile("firstDisk.txt");
    eraseFile("secondDisk.txt");
    fileToSort = new File(filename, File::Input);
    firstDisk = new File("firstDisk.txt", File::Output);
    secondDisk = new File("secondDisk.txt", File::Output);
}


void createFileFromInput(File* &fileToSort, File* &firstDisk, File* &secondDisk)
{
    double recordNumber;
    fstream generateFile;
    eraseFile("fileToSort.txt");
    generateFile.open("fileToSort.txt", fstream::in | fstream::out | fstream::app);
    cout << "Podaj liczbe rekordow\n";
    cin >> recordNumber;
    cout << "podaj rekordy, kazdy rekord ma " << PARAM << " parametrow oddzielonych spacja\n";
    int number;
    for (int i = 0; i < recordNumber; i++)
    {
        for (int j = 0; j < PARAM; j++)
        {
            cin >> number;
            generateFile << number << " ";
        }
        generateFile << endl;
    }
    generateFile.close();
    eraseFile("firstDisk.txt");
    eraseFile("secondDisk.txt");
    fileToSort = new File("fileToSort.txt", File::Input);
    firstDisk = new File("firstDisk.txt", File::Output);
    secondDisk = new File("secondDisk.txt", File::Output);
}

void initialDistribution(File*& fileToSort, File*& firstDisk, File*& secondDisk)
{
    int a = 1, b = 1, firstLength = 0, secondLength = 0;
    double firstLastValue= -DBL_MAX,secondLastValue= -DBL_MAX;
    double record[PARAM];
    double previousRecord[PARAM];
    double oldValue=0;
    bool fileChange = false;
    while(fileToSort->readRecord(record)==0)
    {
        double value = g(record[0], record[1], record[2], record[3], record[4]);
        if(firstLength<a)
        {
            if(fileChange)
            {
                if (oldValue > firstLastValue)
                {
                    firstLength--;
                }
                firstLastValue = oldValue;
                firstDisk->writeRecord(previousRecord);
                fileChange = false;
            }
            if(value < firstLastValue)
            {
                //end of series
                firstLength++;
                if(firstLength==a)
                {
                    fileChange = true;
                    memcpy(previousRecord, record, sizeof(double) * PARAM);
                    oldValue = value;
                }
                else
                {
                    firstDisk->writeRecord(record);
                    firstLastValue = value;
                }
            }
            else
            {
                firstDisk->writeRecord(record);
                firstLastValue = value;
            }
        }
        else if(secondLength<b)
        {
            if (fileChange)
            {
                if (oldValue > secondLastValue && secondLength>0)
                {
                    secondLength--;
                }
                secondLastValue = oldValue;
                secondDisk->writeRecord(previousRecord);
                fileChange = false;
            }
            if (value < secondLastValue)
            {
                //end of series
                secondLength++;
                if (secondLength == b)
                {
                    fileChange = true;
                    memcpy(previousRecord, record, sizeof(double) * PARAM);
                    oldValue = value;
                }
                else
                {
                    secondDisk->writeRecord(record);
                    secondLastValue = value;
                }
            }
            else
            {
                secondDisk->writeRecord(record);
                secondLastValue = value;
            }
        }
        if(firstLength==a && secondLength==b)
        {
            if (a > b)
                b += a;
            else
                a += b;
        }
    }
    
    if(fileChange==true)
    {
        if (firstLength < a)
        {
            firstDisk->writeRecord(previousRecord);
            if (oldValue < firstLastValue || firstLength == 0)
                firstLength++;
        }
        else
        {
            secondDisk->writeRecord(previousRecord);
            if (oldValue < secondLastValue || secondLength==0)
                secondLength++;
        }
    }

    else if (firstLength < a)
        firstLength++;
    else if (secondLength < b)
        secondLength++;

    if (a > b)
        firstDisk->emptySeries = a - firstLength;
    else
        secondDisk->emptySeries = b - secondLength;
}

void merge(File*& destinationFile, File*& firstDisk, File*& secondDisk, double firstRecord[PARAM], double secondRecord[PARAM])
{
    double firstLastValue = -DBL_MAX, secondLastValue = -DBL_MAX;
    double firstValue = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4]);
    double secondValue = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4]);
    bool firstRecordEnd = false;
    bool secondRecordEnd = false;
    while (firstRecordEnd == false && secondRecordEnd == false)
    {
        while (firstValue >= firstLastValue && firstRecordEnd == false && secondValue >= secondLastValue && secondRecordEnd == false)
        {
            if (firstValue > secondValue)
            {
                destinationFile->writeRecord(secondRecord);
                if (secondDisk->readRecord(secondRecord))
                {
                    secondRecordEnd = true;
                    secondValue = -DBL_MAX;
                }
                else
                {
                    secondLastValue = secondValue;
                    secondValue = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4]);
                }
            }
            else
            {
                destinationFile->writeRecord(firstRecord);
                if (firstDisk->readRecord(firstRecord))
                {
                    firstRecordEnd = true;
                    firstValue = -DBL_MAX;
                }
                else
                {
                    firstLastValue = firstValue;
                    firstValue = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4]);
                }
            }
        }

        //end second series
        while (firstValue >= firstLastValue && firstRecordEnd == false)
        {
            destinationFile->writeRecord(firstRecord);
            if (firstDisk->readRecord(firstRecord))
            {
                firstRecordEnd = true;
                firstValue = -DBL_MAX;
            }
            else
            {
                firstLastValue = firstValue;
                firstValue = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4]);
            }
        }

        while (secondValue >= secondLastValue && secondRecordEnd == false)
        {
            destinationFile->writeRecord(secondRecord);
            if (secondDisk->readRecord(secondRecord))
            {
                secondRecordEnd = true;
                secondValue = -DBL_MAX;
            }
            else
            {
                secondLastValue = secondValue;
                secondValue = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4]);
            }
        }

        firstLastValue = -DBL_MAX, secondLastValue = -DBL_MAX;
    }

    
    destinationFile->changeMode();
    if (firstRecordEnd && secondRecordEnd)
    {
        cout << "plik z posortowanymi danymi to: " << destinationFile->fileName;
        return;
    }
    if(firstRecordEnd)
    {
        firstDisk->changeMode();
        destinationFile->readRecord(firstRecord);
        merge(firstDisk, destinationFile, secondDisk, firstRecord, secondRecord);
    }
    else
    {
        secondDisk->changeMode();
        destinationFile->readRecord(secondRecord);
        merge(secondDisk, firstDisk, destinationFile, firstRecord, secondRecord);
    }
}

void firstMerge(File*& destinationFile, File*& firstDisk, File*& secondDisk)
{
    double firstLastValue = DBL_MIN, secondLastValue = DBL_MIN;
    double firstRecord[PARAM];
    double secondRecord[PARAM];
    firstDisk->readRecord(firstRecord);
    secondDisk->readRecord(secondRecord);
    double value;
    for(firstDisk->emptySeries;firstDisk->emptySeries>0;firstDisk->emptySeries--)
    {   
        while((value = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4])) >= secondLastValue)
        {
            destinationFile->writeRecord(secondRecord);
            secondDisk->readRecord(secondRecord);
            secondLastValue = value;
        }
        secondLastValue = DBL_MIN;
    }
    for (secondDisk->emptySeries; secondDisk->emptySeries > 0; secondDisk->emptySeries--)
    {
        while ((value = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4])) >= firstLastValue)
        {
            destinationFile->writeRecord(firstRecord);
            firstDisk->readRecord(firstRecord);
            firstLastValue = value;
        }
        firstLastValue = DBL_MIN;
    }
    merge(destinationFile, firstDisk, secondDisk, firstRecord, secondRecord);
}

int main()
{
    File *fileToSort = nullptr;
    File* firstDisk = nullptr;
    File * secondDisk = nullptr;
    while(true)
    {
        int choice = 0;
        cout << "Wybierz z menu opcje\n";
        cout << "1.Wygeneruj plik do posortowania\n";
        cout << "2.Podaj plik do posortowania\n";
        cout << "3.Wpisz rekordy do posortowania z klawiatury\n";
        cin >> choice;
        cout << endl;
        switch(choice)
        {
        case 1:
            generateFile(fileToSort,firstDisk,secondDisk);
            break;
        case 2:
            loadFile(fileToSort, firstDisk, secondDisk);
            break;
        case 3:
            createFileFromInput(fileToSort, firstDisk, secondDisk);
            break;
        default:
            cout << "Unexpected option!!!";
        }
        if (fileToSort != nullptr)
            break;
    }

    initialDistribution(fileToSort, firstDisk, secondDisk);
    //fileToSort->changeMode();
    firstDisk->changeMode();
    secondDisk->changeMode();
    eraseFile("thirdDisk.txt");
    File*thirdDisk = new File("thirdDisk.txt", File::Output);
    firstMerge(thirdDisk, firstDisk, secondDisk);
}

