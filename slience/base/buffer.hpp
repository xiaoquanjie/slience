#ifndef M_BUFFER_INCLUDE
#define M_BUFFER_INCLUDE

#include "slience/base/config.hpp"
#include <string>

M_BASE_NAMESPACE_BEGIN

class Buffer
{
public:
	struct _data_ {
		s_uint32_t	_size;
		s_uint32_t	_pos;
		s_uint32_t	_offset;
		s_byte_t*	_data;
	};

	Buffer();

	Buffer(s_uint32_t hdrlen);

	~Buffer();

	void Clear();

	void CutData(s_int32_t len);

	s_byte_t* Data();

	const s_byte_t* Data()const;

	s_byte_t* Raw();

	const s_byte_t* Raw()const;

	s_uint32_t Capacity()const;

	s_uint32_t Size()const;

	s_uint32_t Length()const;

	void Write(const void* data, s_uint32_t len);

	template<typename T>
	void Write(T const& value) {
		Write(&value, sizeof(value));
	}

	void Write(std::string const& value);

	template<typename T>
	Buffer& operator<<(T const& value){
		Write(value);
		return *this;
	}

	void Read(void* data, s_uint32_t len);

	template<typename T>
	void Read(T& value) {
		Read(&value, sizeof(value));
	}

	void Read(std::string& value);

	template<typename T>
	Buffer& operator >> (T& value){
		Read(value);
		return *this;
	}

	void Swap(Buffer& buffer);

protected:
	Buffer(const Buffer&);
	Buffer& operator=(const Buffer&);

private:
	_data_ _data;
};

M_BASE_NAMESPACE_END
#endif