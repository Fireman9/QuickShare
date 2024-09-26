#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT

  public:
    enum class LogLevel { Trace, Debug, Info, Warning, Error, Fatal };

    static Logger& instance();

    void setLogFile(const QString& filename);
    void setConsoleOutput(bool enable);
    void setLogLevel(LogLevel level);

    void log(LogLevel level, const QString& message);

  private:
    Logger();
    ~Logger();

    QString levelToString(LogLevel level);

    QFile       m_logFile;
    QTextStream m_logStream;
    bool        m_consoleOutput;
    LogLevel    m_logLevel;
};

#define LOG_TRACE(msg)   Logger::instance().log(Logger::LogLevel::Trace, msg)
#define LOG_DEBUG(msg)   Logger::instance().log(Logger::LogLevel::Debug, msg)
#define LOG_INFO(msg)    Logger::instance().log(Logger::LogLevel::Info, msg)
#define LOG_WARNING(msg) Logger::instance().log(Logger::LogLevel::Warning, msg)
#define LOG_ERROR(msg)   Logger::instance().log(Logger::LogLevel::Error, msg)
#define LOG_FATAL(msg)   Logger::instance().log(Logger::LogLevel::Fatal, msg)

#endif // LOGGER_HPP
