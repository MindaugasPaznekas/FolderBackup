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
    LogUtility(const LogUtility&) = delete;
    LogUtility& operator=(const LogUtility&) = delete;
    LogUtility& operator==(const LogUtility&) = delete;

    LogUtility();

    ~LogUtility();

    /**
     * @brief read log file and apply provided function to each line
     * @param func: function to execute on log lines
     */
    void searchLog(const std::function<void(const std::string&)>& func) const;
    /**
     * @brief loop for writing queued messages to the end of log file
     */
    void writeToFileThread() const;
    /**
     * @brief: call to prepare threads for finishing
     */
    void stopThreads();
    /**
     * @brief Class which handles writing to log file
     */
    class LogWriter
    {
    public:
        LogWriter(const LogWriter&) = delete;
        LogWriter& operator=(const LogWriter&) = delete;
        LogWriter& operator==(const LogWriter&) = delete;
        LogWriter();
        ~LogWriter();
        /**
         * @brief put a message to queue which will be written to a log file when possible
         */
        void addMessageToLog(const std::string& path, Action action);
        void addMessageToLog(const std::string& source, const std::string& destination, Action action);
    private:
        void getTimeString(std::string& string) const;
        constexpr const std::string& actionToString(Action action);
        void addMessageToQueue(const std::string& message);
        const std::string DeleteString;
        const std::string BackupString;
        const std::string UpdateString;
        const std::string CreatedString;
    };

    LogUtility::LogWriter& getLogWriter();
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

    std::atomic<bool> m_isThreadStopRequested;
    const std::string LogFileName;
    static std::mutex s_writeQueueMutex;
    static std::mutex s_fileOperationMutex;
    static std::queue<std::string> s_writeQueue;
    LogWriter m_logWriter;
};