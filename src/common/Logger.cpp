#include "Logger.hpp"

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : m_consoleOutput(true), m_logLevel(LogLevel::Info) {}

Logger::~Logger()
{
    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
}

void Logger::setLogFile(const QString& filename)
{
    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }

    m_logFile.setFileName(filename);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append |
                        QIODevice::Text))
    {
        qWarning() << "Failed to open log file:" << filename;
        return;
    }

    m_logStream.setDevice(&m_logFile);
}

void Logger::setConsoleOutput(bool enable)
{
    m_consoleOutput = enable;
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::log(LogLevel level, const QString& message)
{
    if (level < m_logLevel)
    {
        return;
    }

    QString timestamp =
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logMessage = QString("[%1] [%2] %3")
                             .arg(timestamp)
                             .arg(levelToString(level))
                             .arg(message);

    if (m_logFile.isOpen())
    {
        m_logStream << logMessage << Qt::endl;
        m_logStream.flush();
    }

    if (m_consoleOutput)
    {
        switch (level)
        {
            case LogLevel::Trace:
            case LogLevel::Debug: qDebug().noquote() << logMessage; break;
            case LogLevel::Info: qInfo().noquote() << logMessage; break;
            case LogLevel::Warning: qWarning().noquote() << logMessage; break;
            case LogLevel::Error:
            case LogLevel::Fatal: qCritical().noquote() << logMessage; break;
        }
    }
}

QString Logger::levelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}
