// SortingFib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "FileOperation.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <float.h>
#include <stdio.h>


void printFile(string fileName)
{
    double number;
    FILE* file = fopen(fileName.c_str(), "r");
    while(true)
    {
        for (int j = 0; j < PARAM; j++)
        {
            if (fread(&number, sizeof(double), 1, file) == 0)
            {
               
                return;
            }
            cout << number << " ";
        }
        cout << endl;
    }
}

//g(a,y,c,z,x)=10a[x2+3c3z4-5y7]
double g(double a, double y, double c, double z, double x)
{
    //return 10 * a * (pow(x, 2) + 3 * pow(c, 3) * pow(z, 4) - 5 * pow(y, 7));
    return a;
}


//function erase file from a disk
void eraseFile(string fileName)
{
    FILE* f = fopen(fileName.c_str(), "ab");
    fclose(f);
    remove(fileName.c_str());
}


//function take number of records to generate from user and 
void generateFile(File *&fileToSort,string filename)
{
    int recordNumber;
    FILE* generateFile;
    eraseFile(filename);
    generateFile = fopen(filename.c_str(), "ab");
    cout << "Podaj liczbe rekordow\n";
    cin >> recordNumber;
    srand(time(NULL));
    for (int i = 0; i < recordNumber;i++)
    {
        for(int j=0;j<PARAM;j++)
        {
            double generatedNumber = rand();
            fwrite(&generatedNumber,sizeof(double),1,generateFile);
        }
    }
    fclose(generateFile);
    fileToSort = new File(filename, File::Input);
    
}

//function take filename from user and open it
void loadFile(File* &fileToSort, string& filename)
{
    cout << "Podaj sciezke do pliku\n";
    cin >> filename;
    fileToSort = new File(filename, File::Input);
}

//function create file from input
void createFileFromInput(File* &fileToSort, string filename)
{
    double recordNumber;
    FILE* generateFile;
    eraseFile(filename);
    generateFile = fopen(filename.c_str(), "ab");
    cout << "Podaj liczbe rekordow\n";
    cin >> recordNumber;
    cout << "podaj rekordy, kazdy rekord ma " << PARAM << " parametrow oddzielonych spacja\n";
    double number;
    for (int i = 0; i < recordNumber; i++)
    {
        for (int j = 0; j < PARAM; j++)
        {
            cin >> number;
            fwrite(&number, sizeof(double), 1, generateFile);
        }
    }
    fclose(generateFile);
    fileToSort = new File(filename, File::Input);
}


//function perform initial distribution of file. It also save number of empty series
void initialDistribution(File*& fileToSort, File*& firstDisk, File*& secondDisk)
{
    int a = 1, b = 1, firstLength = 0, secondLength = 0;
    double firstLastValue= -DBL_MAX,secondLastValue= -DBL_MAX;
    double record[PARAM];
    double previousRecord[PARAM];
    double oldValue=0;
    bool fileChange = false;

    //1. while we have records in main file
    while(fileToSort->readRecord(record)==0)
    {
        double value = g(record[0], record[1], record[2], record[3], record[4]);

       //2. check to which file should record go
        if(firstLength<a)
        {
            //3. if we changed file, we have to check if sticking happened
            if(fileChange)
            {
                //if sticked, change number of series
                if (oldValue >= firstLastValue)
                {
                    firstLength--;
                }
                firstLastValue = oldValue;
                firstDisk->writeRecord(previousRecord);
                fileChange = false;
            }

            //4. If series end
            if(value < firstLastValue)
            {
                firstLength++;
                //5. if we need to change file after this record
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
        //6. Analogical to second file
        else if(secondLength<b)
        {
            if (fileChange)
            {
                if (oldValue >= secondLastValue && secondLength>0)
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

        //7. If both file have enough series, we have to increase number of series in smaller file
        if(firstLength==a && secondLength==b)
        {
            if (a > b)
                b += a;
            else
                a += b;
        }
    }
    //8. if last record ggo to diffrent file, we have to handle this case
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
    //9. if not, we have to add one series to file we currenty writing in
    else if (firstLength < a)
        firstLength++;
    else if (secondLength < b)
        secondLength++;

    firstDisk->changeMode();
    secondDisk->changeMode();

    //10. Counting number of empty series
    if (a > b)
        firstDisk->emptySeries = a - firstLength;
    else
        secondDisk->emptySeries = b - secondLength;
}

// after choice from which file we should write record, perform some operations
void writeRecord(double &value, double &lastValue, File *& disk, File*&destinationFile, double record[PARAM])
{
    destinationFile->writeRecord(record);
    if (disk->readRecord(record))
    {
        disk->fileEnd = true;
        value = -DBL_MAX;
    }
    else
    {
        lastValue = value;
        value = g(record[0], record[1], record[2], record[3], record[4]);
    }
}

//functions merge files into another one. it takes recordEnd param to know, which disk have ended
void merge(File*& destinationFile, File*& firstDisk, File*& secondDisk, double firstRecord[PARAM], double secondRecord[PARAM])
{
    double firstLastValue = -DBL_MAX, secondLastValue = -DBL_MAX;
    double firstValue = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4]);
    double secondValue = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4]);
    
    while (!firstDisk->fileEnd && !secondDisk->fileEnd)
    {
        while (firstValue >= firstLastValue && !firstDisk->fileEnd && secondValue >= secondLastValue && !secondDisk->fileEnd)
        {
            if (firstValue > secondValue)
            {
                writeRecord(secondValue, secondLastValue, secondDisk, destinationFile, secondRecord);
            }
            else
            {
                writeRecord(firstValue, firstLastValue, firstDisk, destinationFile, firstRecord);
            }
        }

        //end second series
        while (firstValue >= firstLastValue && !firstDisk->fileEnd)
        {
            writeRecord(firstValue, firstLastValue, firstDisk, destinationFile, firstRecord);
        }

        while (secondValue >= secondLastValue && !secondDisk->fileEnd)
        {
            writeRecord(secondValue, secondLastValue, secondDisk, destinationFile, secondRecord);
        }

        firstLastValue = -DBL_MAX, secondLastValue = -DBL_MAX;
    }
}

// Function firstly take care about empty records and after it, perform merge.
// Files are merged, when both files reach the end in one time.
// This situation can only happen when each oof files have 1 series left.
// As a result i should have sorted file and file with only one series and file with 2 series.
void performMerge(File*& destinationFile, File*& firstDisk, File*& secondDisk,bool breakAfterEachPhase, int initialFileReadLines)
{
    double firstLastValue = -DBL_MAX, secondLastValue = -DBL_MAX;
    double firstRecord[PARAM];
    double secondRecord[PARAM];
    firstDisk->readRecord(firstRecord);
    secondDisk->readRecord(secondRecord);
    double value;

    //taking care of empty seires. they could be only in one file,
    //but we don't know in which it is, so only one for loop will work
    for(firstDisk->emptySeries;firstDisk->emptySeries>0;firstDisk->emptySeries--)
    {   
        while((value = g(secondRecord[0], secondRecord[1], secondRecord[2], secondRecord[3], secondRecord[4])) >= secondLastValue)
        {
            destinationFile->writeRecord(secondRecord);
            secondDisk->readRecord(secondRecord);
            secondLastValue = value;
        }
        secondLastValue = -DBL_MAX;
    }

    for (secondDisk->emptySeries; secondDisk->emptySeries > 0; secondDisk->emptySeries--)
    {
        while ((value = g(firstRecord[0], firstRecord[1], firstRecord[2], firstRecord[3], firstRecord[4])) >= firstLastValue)
        {
            destinationFile->writeRecord(firstRecord);
            firstDisk->readRecord(firstRecord);
            firstLastValue = value;
        }
        firstLastValue = -DBL_MAX;
    }


    int numberOfPhases = 0;

    // main merge loop
    while (true)
    {
        // 1. increase phase counter
        numberOfPhases++;

        // 2. perform merge in files
        merge(destinationFile, firstDisk, secondDisk, firstRecord, secondRecord);
        destinationFile->changeMode();

        // 3. check end condition and if happen, do post merging print
        if (firstDisk->fileEnd && secondDisk->fileEnd)
        {
            cout << "plik z posortowanymi danymi to: " << destinationFile->fileName<<endl;
            printFile(destinationFile->fileName);
            cout << "liczba faz to: " << numberOfPhases<<endl;
            int readNumber = destinationFile->readPageCounter + firstDisk->readPageCounter + secondDisk->readPageCounter + initialFileReadLines;
            int writeNumber = destinationFile->writePageCounter + firstDisk->writePageCounter + secondDisk->writePageCounter;
            cout << "liczba odczytow dyskowych to: " << readNumber <<" stron"<<endl;
            cout << "liczba zapisow dyskowych to: " << writeNumber << " stron" << endl;
            cout << "calkowita liczba operacji dyskowych: " << readNumber + writeNumber << " stron" << endl;
            break;
        }

        // 4. print file after each phase on demand
        if (breakAfterEachPhase)
        {
            int choice;
            cout << "Najnowszy plik(docelowy z poprzedniej fazy):\n";
            printFile(destinationFile->fileName);
            cout << "czy chcesz przejsc do konca?\n";
            cout << "1.tak\n";
            cout << "2.Nie\n";
            cin >> choice;
            if (choice = 1)
            {
                breakAfterEachPhase = false;
            }
        }

        // 5. If first disk ended, second one is still in progress,
        // so now first disk will be destination one and previus destination will be source disk
        // same if second disk ended
        if (firstDisk->fileEnd)
        {
            firstDisk->changeMode();
            destinationFile->readRecord(firstRecord);
            swap(firstDisk, destinationFile);
            merge(destinationFile, firstDisk,secondDisk, firstRecord, secondRecord);
        }
        else 
        {
            secondDisk->changeMode();
            destinationFile->readRecord(secondRecord);
            swap(destinationFile, secondDisk);
            merge(destinationFile, firstDisk, secondDisk, firstRecord, secondRecord);
        }
    }
}

int main()
{
    File *fileToSort = nullptr;
    File* firstDisk = nullptr;
    File * secondDisk = nullptr;
    bool breakAfterEachPhase = false;

    string filename = "fileToSort.dat";  // initial file name
    const string firstDiskName = "firstDisk.dat";
    const string secondDiskName = "secondDisk.dat";
    const string thirdDiskName = "thirdDisk.dat";
    //1. intial menu, choosing way to give input
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
            generateFile(fileToSort, filename);
            break;
        case 2:
            loadFile(fileToSort, filename);
            break;
        case 3:
            createFileFromInput(fileToSort, filename);
            break;
        default:
            cout << "Unexpected option!!!";
        }
        if (fileToSort != nullptr)
            break;
    }

    //2. preparing file disk
    eraseFile(firstDiskName);
    eraseFile(secondDiskName);
    eraseFile(thirdDiskName);

    firstDisk = new File(firstDiskName, File::Output);
    secondDisk = new File(secondDiskName, File::Output);
    File* thirdDisk = new File(thirdDiskName, File::Output);

    //3. asking about printing
    int choice = 0;
    cout << "Czy chcesz ogladac pliki po kazdej fazie sortowania?\n";
    cout << "1.Tak\n";
    cout << "2.Nie\n";
    cin >> choice;
    cout << endl;
    switch(choice)
    {
    case 1:
        breakAfterEachPhase = true;
        break;
    default:
        break;
    }

    printFile(filename);
    getchar();
    
    //4. perform merge operations
    initialDistribution(fileToSort, firstDisk, secondDisk);
    //fileToSort->changeMode();
    int initialFileReadLines = fileToSort->readPageCounter;
    
    
    
    performMerge(thirdDisk, firstDisk, secondDisk, breakAfterEachPhase, initialFileReadLines);

    return 0;
}

