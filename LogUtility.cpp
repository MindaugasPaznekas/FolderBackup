#include "LogUtility.h"
#include <fstream>
#include <chrono>
#include <thread>

#ifdef _MSC_VER
#include <format>
#endif

using namespace std;
namespace fs = std::filesystem;
const string LogUtility::s_deleteString{" was deleted"};
const string LogUtility::s_backupString{" was backed up to: "};
const string LogUtility::s_updateString{ " was updated" };
const string LogUtility::s_createdString{ " was created" };
queue<string> LogUtility::s_writeQueue{};
std::mutex LogUtility::s_writeQueueMutex{};
std::mutex LogUtility::s_fileOperationMutex{};

LogUtility::LogUtility():
    m_isThreadStopRequested(false),
    LogFileName("FolderBackupLog.txt")
{
}

LogUtility::~LogUtility()
{
    m_isThreadStopRequested.store(true);
}

void LogUtility::stopThreads()
{
    m_isThreadStopRequested.store(true);
}

void LogUtility::searchLog(const std::function<void(const string&)>& func) const
{
    ifstream logFile;
    logFile.open(LogFileName, fstream::in);

    string line;
    while (getline(logFile, line))
    {
        if (logFile.eof())
        {
            break;
        }

        func(line);
    }

    safelyReadLastLineFromFile(line, logFile);

    logFile.close();

    func(line);
}

void LogUtility::safelyReadLastLineFromFile(string& singleMessage, std::ifstream& logFile) const
{
    while (!s_fileOperationMutex.try_lock())
    {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    getline(logFile, singleMessage);
    s_fileOperationMutex.unlock();
}

void LogUtility::writeToFileThread() const
{
    while (true)
    {
        if (m_isThreadStopRequested.load())
        {
            return;
        }

        this_thread::sleep_for(chrono::milliseconds(1));

        string singleMessage;

        if (!readMessageFromQueue(singleMessage))
        {
            continue;
        }

        while (!writeSingleMessageToFile(singleMessage))
        {
            this_thread::sleep_for(chrono::milliseconds(1));
            if (m_isThreadStopRequested.load())
            {
                return;
            }
        }
    }
}

bool LogUtility::readMessageFromQueue(string& singleMessage) const
{
    if (!s_writeQueueMutex.try_lock())
    {
        return false;
    }

    if (s_writeQueue.empty())
    {
        s_writeQueueMutex.unlock();
        return false;
    }

    singleMessage = s_writeQueue.front();
    s_writeQueue.pop();
    s_writeQueueMutex.unlock();

    return true;
}

bool LogUtility::writeSingleMessageToFile(std::string& singleMessage) const
{
    std::ofstream logFile;

    if (!s_fileOperationMutex.try_lock())
    {
        return false;
    }

    logFile.open(LogFileName, fstream::app);
    logFile << singleMessage.c_str() << endl;
    logFile.close();
    s_fileOperationMutex.unlock();

    return true;
}

void LogUtility::getTimeString(std::string& string)
{
#ifdef _MSC_VER //version of method for MSVC compiler
    auto now = chrono::floor<chrono::seconds>(chrono::system_clock::now());

    string = std::format("{0:%F}T{0:%T%Ez}", now);
#else //This version is for MINGW compiler
    time_t now;
    time(&now);
    char buf[256];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%S+00:00", gmtime(&now));
    
    string = buf;
#endif
}


void LogUtility::addMessageToLog(const string& path, Action action)
{
    string message;
    getTimeString(message);
    message += " " + path + actionToString(action);

    addMessageToQueue(message);
}

void LogUtility::addMessageToLog(const string& source, const string& destination, Action action)
{
    if (action == Action::Update)
    {
        addMessageToLog(destination, action);
    }
    else
    {
        string message;
        getTimeString(message);
        message += " " + source + actionToString(action) + destination;
        
        addMessageToQueue(message);
    }
}

constexpr const std::string& LogUtility::actionToString(Action action)
{
    if (action == Action::Delete)
    {
        return s_deleteString;
    }
    else if (action == Action::Update)
    {
        return s_updateString;
    }
    else if (action == Action::Created)
    {
        return s_createdString;
    }
    return s_backupString;
}

void LogUtility::addMessageToQueue(const std::string& message)
{
    while (true)
    {
        if (s_writeQueueMutex.try_lock())
        {
            s_writeQueue.emplace(message);
            s_writeQueueMutex.unlock();
            return;
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
}