#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#define LOG_TRACE   BOOST_LOG_TRIVIAL(trace)
#define LOG_DEBUG   BOOST_LOG_TRIVIAL(debug)
#define LOG_INFO    BOOST_LOG_TRIVIAL(info)
#define LOG_WARNING BOOST_LOG_TRIVIAL(warning)
#define LOG_ERROR   BOOST_LOG_TRIVIAL(error)
#define LOG_FATAL   BOOST_LOG_TRIVIAL(fatal)

/// @brief Initializes the logging system. File sink, log file name, logging
/// format, severity level
inline void init_logging()
{
    boost::log::register_simple_formatter_factory<
        boost::log::trivial::severity_level, char>("Severity");

    boost::log::add_file_log(
        boost::log::keywords::file_name = "Quickshare_%N.log",
        boost::log::keywords::format =
            "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");

    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        boost::log::trivial::info);

    boost::log::add_common_attributes();
}

#endif // __LOGGER_HPP__
