#include "slience/base/logger.hpp"

M_BASE_NAMESPACE_BEGIN

namespace logger {

	std::string _getpid_() {
		char tmp_buf[20] = { 0 };
#ifdef M_PLATFORM_WIN
		snprintf(tmp_buf, sizeof(tmp_buf), "%d", GetCurrentProcessId());
#else
		snprintf(tmp_buf, sizeof(tmp_buf), "%d", getpid());
#endif
		return std::string(tmp_buf);
	}

	std::string _getcurtime_(const time_t& now) {
		struct tm ltm;
#ifdef M_PLATFORM_WIN
		localtime_s(&ltm, &now);
#else
		localtime_r(&now, &ltm);
#endif
		char tmp_buf[20] = { 0 };
		strftime(tmp_buf, sizeof(tmp_buf), "%Y%m%d", &ltm);
		return std::string(tmp_buf);
	}

	std::string _logname_(const time_t& now, const std::string& pid,
		const std::string& filename, size_t logidx, bool withpid) {
		char tmp_buf[100] = { 0 };
		if (withpid) {
			snprintf(tmp_buf, sizeof(tmp_buf), "%s_%s_%s_%d.log", filename.c_str(),
				_getcurtime_(now).c_str(), pid.c_str(), (int)logidx);
		}
		else {
			snprintf(tmp_buf, sizeof(tmp_buf), "%s_%s_%d.log", filename.c_str(),
				_getcurtime_(now).c_str(), (int)logidx);
		}
		return std::string(tmp_buf);
	}

	bool _issameday_(time_t& t1, time_t& t2) {
		struct tm ltm1, ltm2;
#ifdef M_PLATFORM_WIN
		localtime_s(&ltm1, &t1);
		localtime_s(&ltm2, &t2);
#else
		localtime_r(&t1, &ltm1);
		localtime_r(&t2, &ltm2);
#endif
		return (ltm1.tm_year == ltm2.tm_year
			&& ltm1.tm_mon == ltm2.tm_mon
			&& ltm1.tm_mday == ltm2.tm_mday);
	}

	long long _gettime_() {
		long long t = 0;
#ifdef M_PLATFORM_WIN
		SYSTEMTIME st;
		struct tm  tm;
		time_t clock;
		GetLocalTime(&st);
		tm.tm_year = st.wYear - 1900;
		tm.tm_mon = st.wMonth - 1;
		tm.tm_mday = st.wDay;
		tm.tm_hour = st.wHour;
		tm.tm_min = st.wMinute;
		tm.tm_sec = st.wSecond;
		tm.tm_isdst = -1;
		clock = mktime(&tm);
		t = clock * 1000 + st.wMilliseconds;
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		t = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
		return t;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	logfile::logfile(const std::string filename, size_t rollsize, bool withpid)
		:_filename(filename) {
		_rollsize = rollsize;
		_file = 0;
		_writesize = 0;
		_logidx = 0;
		_rolltime = 0;
		_lastctime = 0;
		_withpid = withpid;
		_pid = _getpid_();
		_roll_file(true, time(0));
	}

	logfile::~logfile() {
		_close();
	}

	bool logfile::write(const char* data, size_t len) {
		if (_file) {
			time_t now = time(0);
			if (now != _lastctime) {
				_lastctime = now;
				if (!_issameday_(_rolltime, now))
					_roll_file(false, now);
			}
			if (_writesize >= _rollsize) {
				_roll_file(false, now);
			}
			size_t wsize = 0;
			while (len) {
				wsize = fwrite(data, 1, len, _file);
				if (wsize == 0)
					break;
				len -= wsize;
				_writesize += wsize;
			}
		}
		return (len == 0);
	}

	void logfile::flush() {
		if (_file) {
			fflush(_file);
		}
	}

	void logfile::_close() {
		if (_file) {
			fflush(_file);
			fclose(_file);
			_file = 0;
		}
	}

	void logfile::_roll_file(bool newday, const time_t& now) {
		if (_file) {
			_close();
		}
		_writesize = 0;
		_rolltime = now;
		_logidx = newday ? 1 : _logidx + 1;
		std::string name = _logname_(now, _pid, _filename, _logidx, _withpid);
#ifdef M_PLATFORM_WIN
		_file = _fsopen(name.c_str(), "a"/*"w+"*/, _SH_DENYWR);
#else
		_file = fopen(name.c_str(), "a"/*"w+"*/);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	logstream& logstream::operator<<(const bool& value) {
		_buffer.append(value ? "1" : "0", 1);
		return *this;
	}

	logstream& logstream::operator<<(const short& value) {
		*this << static_cast<int>(value);
		return *this;
	}

	logstream& logstream::operator<<(const unsigned short& value) {
		*this << static_cast<unsigned int>(value);
		return *this;
	}

	logstream& logstream::operator<<(const int& value) {
		_convert(value, 0);
		return *this;
	}

	logstream& logstream::operator<<(const unsigned int& value) {
		_convert(value, 0);
		return *this;
	}

	logstream& logstream::operator<<(const long long& value) {
		_convert(value, 0);
		return *this;
	}

	logstream& logstream::operator<<(const unsigned long long& value) {
		_convert(value, 0);
		return *this;
	}

	logstream& logstream::operator<<(const float& value) {
		*this << static_cast<double>(value);
		return *this;
	}

	logstream& logstream::operator<<(const double& value) {
		_convert(value, 1);
		return *this;
	}

	logstream& logstream::operator<<(const char& value) {
		_buffer.append(&value, 1);
		return *this;
	}

	logstream& logstream::operator<<(const std::string& value) {
		_buffer.append(value.c_str(), value.size());
		return *this;
	}

	logstream& logstream::operator<<(const char* value) {
		_buffer.append(value, strlen(value));
		return *this;
	}

	logstream& logstream::operator<<(const void* value) {
		_convert(value, 2);
		return *this;
	}

	logstream::buffer_type& logstream::buffer() {
		return _buffer;
	}

	void console_output(const char* data, size_t len) {
		fwrite(data, 1, len, stdout);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	logger::logger()
		:_condition(_mutex) {
		_level = LOG_LEVEL_TRACE;
		_level_desc[LOG_LEVEL_TRACE] = "TRACE ";
		_level_desc[LOG_LEVEL_DEBUG] = "DEBUG ";
		_level_desc[LOG_LEVEL_INFO] = "INFO  ";
		_level_desc[LOG_LEVEL_WARN] = "WARN  ";
		_level_desc[LOG_LEVEL_ERROR] = "ERROR ";
		_level_desc[LOG_LEVEL_FATAL] = "FATAL ";
		_logerror = "The log buffer is full and has been discarded\n";
		_file = NULL;
		_thread = NULL;
		_rollsize = 1024 * 1024 * 300;
		_flushtime = 50;
		_run = false;
		_output = console_output;
		_node_cnt = 5;
		_circular = 0;
		_producer = 0;
		_consumer = 0;
	}

	logger::~logger() {
		stop();
		delete _circular;
	}

	void logger::stop() {
		if (_run) {
			_run = false;
			_condition.notify();
			if (_thread) {
				_thread->join();
				delete _thread;
			}
			if (_file) {
				delete _file;
			}
		}
	}

	void logger::setlevel(loglevel level) {
		_level = level;
	}

	loglevel logger::getlevel()const {
		return _level;
	}

	void logger::setfilename(const std::string& filename, bool withpid) {
		if (_filename.empty()) {
			_run = true;
			_circular = new buffer_circular(_node_cnt);
			_producer = _circular->first();
			_consumer = _circular->first();
			_filename = filename;
			_file = new logfile(_filename, _rollsize, withpid);
			_thread = new thread(&logger::dump, this, 0);
		}
	}

	void logger::setrollsize(const size_t rollsize) {
		_rollsize = rollsize;
	}

	void logger::setflushtime(size_t time) {
		_flushtime = time;
	}

	logger& logger::instance() {
		static logger static_logger;
		return static_logger;
	}

	void logger::log(const char* data, size_t len) {
		ScopedLock sl(_mutex);
		if (!_producer) {
			return;
		}
		if (_producer->status
			== buffer_circular::Enum_Buffer_Full) {
			// 缓存写满，日志需要丢掉
			fwrite(_logerror, 1, strlen(_logerror), stderr);
			return;
		}
		if (_producer->buffer.avail() > len) {
			_producer->buffer.append(data, len);
		}
		else {
			// 标志为满的
			_producer->status = buffer_circular::Enum_Buffer_Full;
			_producer = _circular->next(_producer);
			if (_producer->status
				== buffer_circular::Enum_Buffer_Full) {
				// 缓存写满，日志需要丢掉
				fwrite(_logerror, 1, strlen(_logerror), stderr);
			}
			else {
				_producer->buffer.append(data, len);
			}
			_condition.notify();
		}
	}

	void logger::setoutput(void(*output)(const char*, size_t)) {
		_output = output;
	}

	void logger::setnodecnt(size_t cnt) {
		_node_cnt = cnt;
	}

	void logger::dump(void*) {
		while (_run) {
			_mutex.lock();
			if (_consumer->status == buffer_circular::Enum_Buffer_Free) {
				_condition.wait((int)_flushtime);
				if (_consumer->status == buffer_circular::Enum_Buffer_Free) {
					if (_consumer->buffer.length() > 0)
						_consumer->status = buffer_circular::Enum_Buffer_Full;
					else {
						_mutex.unlock();
						continue;
					}
				}
			}
			_mutex.unlock();

			_file->write(_consumer->buffer.data(), _consumer->buffer.length());
			if (_output)
				_output(_consumer->buffer.data(), _consumer->buffer.length());

			_mutex.lock();
			if (_consumer == _producer)
				_producer = _circular->next(_producer);
			_consumer->status = buffer_circular::Enum_Buffer_Free;
			_consumer->buffer.clear();
			_consumer = _circular->next(_consumer);
			_mutex.unlock();
			_file->flush();
		}
		_file->flush();
	}
}

M_BASE_NAMESPACE_END