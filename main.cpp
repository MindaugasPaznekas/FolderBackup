#include <iostream>
#include <string>
#include <chrono>
#include "GlobalData.h"
#include "FShelper.h"
#include "LogUtility.h"
#include <thread>
#include <atomic>
#include <regex>

using namespace std;
atomic<bool> isThreadStopRequested;

/**
 * @brief loop for file backup thread
 * Crawls through input directory and checks each file
 */
void runBackupForDirectory()
{
    while (true)
    {
        for (const auto& fileToBackup : filesystem::recursive_directory_iterator(GlobalData::getInstance().getHotFolderDir()))
        {
            if (isThreadStopRequested.load() == true)
            {
                return;
            }

            FSHelper::backupSingleFile(fileToBackup);
        }

        if (isThreadStopRequested.load() == true)
        {
            return;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void regexHandler(LogUtility& log)
{
    string regexInput;
    getline(cin, regexInput);
    cout << "Results in log file matching search term:" << regexInput << endl;

    regex regexSearchTerm;

    try
    {
        regexSearchTerm.assign(regexInput);

        auto regexSearch = [regexSearchTerm](const string& singleLine)
        {
            if (regex_search(singleLine, regexSearchTerm))
            {
                cout << singleLine << endl;
            }
        };

        log.searchLog(regexSearch);

    }
    catch (const exception&)
    {
        cout << "Unsupported regex format" << endl;
    }
}

void simpleSearchHandler(LogUtility& log)
{
    string simpleSearchTerm;

    getline(cin, simpleSearchTerm, '\n');
    cout << "Results in log file matching search term: " << simpleSearchTerm << endl;;
    cin.clear();

    auto simpleSearch = [simpleSearchTerm](const string& singleLine)
    {
        if (singleLine.find(simpleSearchTerm, 0) != string::npos)
        {
            cout << singleLine << endl;
        }
    };

    log.searchLog(simpleSearch);
}

void printHandler(LogUtility& log)
{
    auto justPrint = [](const string& singleLine)
    {
        cout << singleLine << "\n";
    };
    log.searchLog(justPrint);
}

bool hanldeMainMenu(LogUtility& log)
{
    cin.clear();

    string menuOption;
    getline(cin, menuOption);

    if (menuOption == "p")
    {
        cout << "Log file contens:" << endl;

        printHandler(log);
        return false;
    }
    else if (menuOption == "s")
    {
        cout << "Provide simple search term. For example: 2023-02-12T12:20:55+00:00" << endl;

        simpleSearchHandler(log);
        return false;
    }
    else if (menuOption == "r")
    {
        cout << "Provide regex search term. Example \\b(sub)([^] *) returns string starting with 'sub'" << endl;

        regexHandler(log);
        return false;
    }
    else if (menuOption == "e")
    {
        return true;
    }

    cout << "Invalid option" << endl;

    return false;
}

void handleUI(LogUtility& log)
{
    bool exitRequested = false;
    while (exitRequested == false)
    {
        cout << "Enter 'p' to print the log.\n";
        cout << "Enter 's' to execute simple search through the log.\n";
        cout << "Enter 'r' to execute regex search through the log.\n";
        cout << "Enter 'e' to exit application.\n";

        exitRequested = hanldeMainMenu(log);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout<< "Please enter paths for hot and backup folders.\n";
        cout<< "Example: FolderBackup.exe C:\\hot C:\\backup\n";
        return 0;
    }

    auto& globaldata = GlobalData::getInstance();

    globaldata.setHotFolderPath(argv[1]);
    globaldata.setBackupFolderPath(argv[2]);

    if (!FSHelper::initEnvironment())
    {
        return -1;
    }

    LogUtility log;

    thread writeToFileThread(&LogUtility::writeToFileThread, &log );

    thread backupFilesThread(&runBackupForDirectory);

    handleUI(log);

    isThreadStopRequested.store(true);
    backupFilesThread.join();

    log.stopThreads();
    writeToFileThread.join();

    return 0;
}