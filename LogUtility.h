#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
/**
 * @brief Handles file read and write operations for log file
 */
class LogUtility
{
public:
    /**
     * @brief enum for possible log events
     */
    enum class Action
    {
        Delete,
        Backup,
        Update,
        Created
    };

    LogUtility();

    ~LogUtility();

    /**
     * @brief read log file and apply provided function to each line
     * @param func: function to execute on log lines
     */
    void searchLog(const std::function<void(const std::string&)>& func) const;
    /**
     * @brief put a message to queue which will be written to a log file when possible
     */
    static void addMessageToLog(const std::string& path, Action action);
    static void addMessageToLog(const std::string& source, const std::string& destination, Action action);
    /**
     * @brief loop for writing queued messages to the end of log file
     */
    void writeToFileThread() const;
    /**
     * @brief: call to prepare threads for finishing
     */
    void stopThreads();

private:
    bool readMessageFromQueue(std::string& singleMessage) const;
    bool writeSingleMessageToFile(std::string& singleMessage) const;
    /**
     * @brief As last line in file might be being written to by another thread 
     * we will lock acces to it via mutex
     * @param singleMessage: reference for string to be put from file
     * @param logFile: reference to input file stream that needs to be read
     */
    void safelyReadLastLineFromFile(std::string& singleMessage, std::ifstream& logFile) const;
    constexpr static const std::string& actionToString(Action action);
    static void addMessageToQueue(const std::string& message);
    static void getTimeString(std::string& string);

    std::atomic<bool> m_isThreadStopRequested;
    const std::string LogFileName;
    static std::mutex s_writeQueueMutex;
    static std::mutex s_fileOperationMutex;

    static const std::string s_deleteString;
    static const std::string s_backupString;
    static const std::string s_updateString;
    static const std::string s_createdString;

    static std::queue<std::string> s_writeQueue;
};