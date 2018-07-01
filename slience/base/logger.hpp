#ifndef M_BASE_LOGGER_INCLUDE
#define M_BASE_LOGGER_INCLUDE

#include "slience/base/config.hpp"
#include "slience/base/thread.hpp"
#include "slience/base/tls.hpp"
#include "slience/base/condition.hpp"
#include <string>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "slience/base/win.hpp"

#ifndef M_PLATFORM_WIN
#include <unistd.h>
#include <sys/time.h>
#endif

M_BASE_NAMESPACE_BEGIN

namespace logger {

	std::string _getpid_();

	std::string _getcurtime_(const time_t& now);

	std::string _logname_(const time_t& now, const std::string& pid,
		const std::string& filename, size_t logidx, bool withpid);

	bool _issameday_(time_t& t1, time_t& t2);

	// 毫秒级
	long long _gettime_();

///////////////////////////////////////////////////////////////////////////////////////////////////

	// 日志文件类
	class logfile {
	protected:
		logfile(const logfile&);
		logfile& operator=(const logfile&);

	private:
		std::string _filename;
		FILE* _file;
		size_t _writesize;
		size_t _rollsize;
		size_t _logidx;
		time_t _rolltime;
		time_t _lastctime;
		std::string _pid;
		bool _withpid;

	public:
		logfile(const std::string filename, size_t rollsize,bool withpid);

		~logfile();

		bool write(const char* data, size_t len);

		void flush();

	protected:
		void _roll_file(bool newday, const time_t& now);

		void _close();
	};

	
///////////////////////////////////////////////////////////////////////////////////////////////

	// 固定缓存
	template<size_t SIZE>
	struct fixedbuffer {
	public:
		fixedbuffer();

		void append(const char* data, size_t len);

		const char* data()const;

		size_t length()const;

		const char* current()const;

		char* current();

		size_t avail()const;

		void seek(int incr);

		void clear();

		const char* getstring();

	protected:
		fixedbuffer(const fixedbuffer&);
		fixedbuffer& operator=(const fixedbuffer&);

	private:
		char _data[SIZE+1];
		size_t _pos;
	};

	template<size_t SIZE>
	fixedbuffer<SIZE>::fixedbuffer() {
		clear();
	}

	template<size_t SIZE>
	void fixedbuffer<SIZE>::append(const char* data, size_t len) {
		if (len > (SIZE - _pos))
			len = SIZE - _pos;
		memcpy(_data + _pos, data, len);
		_pos += len;
	}

	template<size_t SIZE>
	const char* fixedbuffer<SIZE>::data()const {
		return _data;
	}

	template<size_t SIZE>
	size_t fixedbuffer<SIZE>::length()const {
		return _pos;
	}
	
	template<size_t SIZE>
	const char* fixedbuffer<SIZE>::current()const {
		return &_data[_pos];
	}

	template<size_t SIZE>
	char* fixedbuffer<SIZE>::current() {
		return &_data[_pos];
	}

	template<size_t SIZE>
	size_t fixedbuffer<SIZE>::avail()const {
		return (SIZE - _pos);
	}

	template<size_t SIZE>
	void fixedbuffer<SIZE>::seek(int incr) {
		if (incr >= 0) {
			_pos += incr;
			if (_pos > SIZE)
				_pos = SIZE;
		}
		else if ((size_t)(incr*-1) > _pos) {
			_pos = 0;
		}
		else {
			_pos += incr;
		}
	}

	template<size_t SIZE>
	void fixedbuffer<SIZE>::clear() {
		_pos = 0;
		//memset(_data, 0, SIZE + 1);
	}

	template<size_t SIZE>
	const char* fixedbuffer<SIZE>::getstring() {
		_data[_pos] = '\0';
		return _data;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	// 日志流,一条日志最大4k
	class logstream {
	public:
		typedef fixedbuffer<4 * 1024> buffer_type;

		logstream& operator<<(const bool& value);

		logstream& operator<<(const short& value);

		logstream& operator<<(const unsigned short& value);

		logstream& operator<<(const int& value);

		logstream& operator<<(const unsigned int& value);

		logstream& operator<<(const long long& value);

		logstream& operator<<(const unsigned long long& value);

		logstream& operator<<(const float& value);

		logstream& operator<<(const double& value);

		logstream& operator<<(const char& value);

		logstream& operator<<(const std::string& value);

		template<int len>
		logstream& operator<<(const char(&value)[len]);

		logstream& operator<<(const char* value);

		// support for address
		logstream& operator<<(const void* value);

		buffer_type& buffer();

	protected:
		template<typename T>
		void _convert(const T&value, size_t type);

	protected:
		buffer_type _buffer;
	};

	template<int len>
	logstream& logstream::operator<<(const char(&value)[len]) {
		_buffer.append(value, len - 1);
		return *this;
	}
	
	template<typename T>
	void logstream::_convert(const T&value, size_t type) {
		// 整数最长接受32个字符
		if (_buffer.avail() >= 32) {
			const char* ptype = 0;
			switch (type) {
			case 0:
				ptype = "%d";
				break;
			case 1:
				ptype = "%.12g";
				break;
			default:
				ptype = "0x%0X";
				break;
			}
			int len = snprintf(_buffer.current(), _buffer.avail(), ptype, value);
			_buffer.seek(len);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	struct logtime {
		char _time_str[32];
		char _mil_str[10];
		time_t _last;
		logtime() {
			_last = 0;
		}
		void to_format(logstream& ls) {
			long long now_in_mil = _gettime_();
			time_t now = now_in_mil / 1000;
			int mil = now_in_mil % 1000;
			if (now != _last) {
				_last = now;
				struct tm ltm;
#ifdef M_PLATFORM_WIN
				localtime_s(&ltm, &now);
#else
				localtime_r(&now, &ltm);
#endif
				strftime(_time_str, sizeof(_time_str), "%Y%m%d %H:%M:%S", 
					&ltm);
			}
			snprintf(_mil_str, sizeof(_mil_str), ".%03d ", mil);
			ls.buffer().append(_time_str, 17);
			ls.buffer().append(_mil_str, 5);
		}
	};

///////////////////////////////////////////////////////////////////////////////////////////////////

	struct logthread{
		std::string _thrid;
		logthread() {
			_thrid = thread::ctid_str();
			_thrid.append(" ");
		}
		void to_format(logstream& ls) {
			ls << _thrid;
		}
	};

	enum loglevel{
		LOG_LEVEL_TRACE = 0,
		LOG_LEVEL_DEBUG = 1,
		LOG_LEVEL_INFO = 2,
		LOG_LEVEL_WARN = 3,
		LOG_LEVEL_ERROR = 4,
		LOG_LEVEL_FATAL = 5,
		LOG_LEVEL_MAX = 6,
	};

///////////////////////////////////////////////////////////////////////////////////////////////////

	struct buffer_circular {
		enum {
			Enum_Buffer_Free,
			Enum_Buffer_Full,
		};
		typedef fixedbuffer<4 * 1024 * 1024> buffer_type;
		struct buffer_node {
			buffer_type buffer;
			size_t status;
			size_t idx;
		};
		buffer_node* _buffer_node;
		size_t _node_cnt;

		buffer_circular(size_t node_cnt) {
			_node_cnt = node_cnt;
			_buffer_node = new buffer_node[_node_cnt];
			for (size_t i = 0; i < _node_cnt; ++i) {
				_buffer_node[i].status = Enum_Buffer_Free;
				_buffer_node[i].idx = i;
			}
		}
		~buffer_circular() {
			delete[]_buffer_node;
		}
		buffer_node* first() {
			return &_buffer_node[0];
		}
		buffer_node* next(buffer_node* node) {
			size_t idx = node->idx;
			idx = (idx + 1) % _node_cnt;
			return &_buffer_node[idx];
		}
	};

///////////////////////////////////////////////////////////////////////////////////////////////////

	void console_output(const char* data, size_t len);

	class logger {
	public:
		const char* _level_desc[LOG_LEVEL_MAX];
	public:
		void stop();

		void setlevel(loglevel level);

		loglevel getlevel()const;

		void setfilename(const std::string& filename, bool withpid=true);

		void setrollsize(const size_t rollsize);

		void setflushtime(size_t time);

		static logger& instance();

		void log(const char* data, size_t len);

		void setoutput(void(*output)(const char*, size_t));

		void setnodecnt(size_t cnt);

	protected:
		logger();

		~logger();

		logger(const logger&);

		logger& operator=(const logger&);

		void dump(void*);

	private:
		bool _run;
		std::string _filename;
		loglevel _level;
		logfile* _file;
		thread* _thread;
		size_t _rollsize;
		size_t _flushtime;
		void(*_output)(const char*, size_t);
		MutexLock _mutex;
		Condition _condition;
		buffer_circular* _circular;
		size_t _node_cnt;
		buffer_circular::buffer_node* _producer;
		buffer_circular::buffer_node* _consumer;
		const char* _logerror;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////

	struct logimpl {
		logstream _stream;
		logimpl(loglevel level) {
			logtime& lg = tlsdata<logtime, 0>::data();
			lg.to_format(_stream);
			_stream << logger::instance()._level_desc[level];
			logthread& lt = tlsdata<logthread, 0>::data();
			lt.to_format(_stream);
		}
		~logimpl() {
			_stream << "\n";
			logger::instance().log(_stream.buffer().data(), _stream.buffer().length());
		}
		logstream& stream() {
			return _stream;
		}
	};

}

M_BASE_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define SetLogLevel(level)\
	base::logger::logger::instance().setlevel((base::logger::loglevel)level)
#define GetLogLevel()\
	base::logger::logger::instance().getlevel()
#define SetLogFileName(name, withpid)\
	base::logger::logger::instance().setfilename(name,withpid)
#define SetLogOutput(output)\
	base::logger::logger::instance().setoutput(output)
#define SetLogFlushTime(time)\
	base::logger::logger::instance().setflushtime(time)
#define SetLogRollSize(size)\
	base::logger::logger::instance().setrollsize(size)
#define SetLogNodeCount(cnt)\
	base::logger::logger::instance().setnodecnt(cnt)

#define LogTrace(content)\
{\
	if (base::logger::LOG_LEVEL_TRACE>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_TRACE).stream() \
		<<__FILE__<< ":" << __LINE__ << "(" << __FUNCTION__ << ")|"<< content; \
	}\
}

#define LogDebug(content)\
{\
	if (base::logger::LOG_LEVEL_DEBUG>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_DEBUG).stream() \
		<< __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")|" << content; \
	}\
}

#define LogInfo(content)\
{\
	if (base::logger::LOG_LEVEL_INFO>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_INFO).stream() \
		<< __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")|" << content; \
	}\
}

#define LogWarn(content)\
{\
	if (base::logger::LOG_LEVEL_WARN>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_WARN).stream() \
		<< __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")|" << content; \
	}\
}

#define LogError(content)\
{\
	if (base::logger::LOG_LEVEL_ERROR>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_ERROR).stream() \
		<< __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")|" << content; \
	}\
}

#define LogFatal(content)\
{\
	if (base::logger::LOG_LEVEL_FATAL>=GetLogLevel()){\
	base::logger::logimpl(base::logger::LOG_LEVEL_FATAL).stream() \
		<< __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")|" << content; \
	}\
}

#endif
