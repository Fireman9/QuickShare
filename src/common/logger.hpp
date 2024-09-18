#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#define LOG_TRACE   BOOST_LOG_TRIVIAL(trace)
#define LOG_DEBUG   BOOST_LOG_TRIVIAL(debug)
#define LOG_INFO    BOOST_LOG_TRIVIAL(info)
#define LOG_WARNING BOOST_LOG_TRIVIAL(warning)
#define LOG_ERROR   BOOST_LOG_TRIVIAL(error)
#define LOG_FATAL   BOOST_LOG_TRIVIAL(fatal)

namespace logger = boost::log;
namespace trivial = boost::log::trivial;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

/// @brief Initializes the logging system with configurable severity level and
/// output destinations.
/// @param min_severity The minimum severity level for logging.
/// @param log_to_file Whether to log to a file.
/// @param log_to_console Whether to log to the console.
inline void init_logging(trivial::severity_level min_severity = trivial::info,
                         bool log_to_file = true, bool log_to_console = true)
{
    logger::register_simple_formatter_factory<trivial::severity_level, char>(
        "Severity");

    auto log_formatter =
        expr::stream << "["
                     << expr::format_date_time<boost::posix_time::ptime>(
                            "TimeStamp", "%Y-%m-%d %H:%M:%S")
                     << "] ["
                     << expr::attr<attrs::current_thread_id::value_type>(
                            "ThreadID")
                     << "] [" << trivial::severity << "] " << expr::smessage;

    if (log_to_file)
    {
        logger::add_file_log(keywords::file_name = "Quickshare_%N.log",
                             keywords::format = log_formatter);
    }

    if (log_to_console)
    {
        logger::add_console_log(std::cout, keywords::format = log_formatter);
    }

    logger::core::get()->set_filter(trivial::severity >= min_severity);

    logger::add_common_attributes();
}

#endif // __LOGGER_HPP__
