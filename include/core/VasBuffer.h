/*
 * VasBuffer.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_CORE_VAS_BUFFER_H_
#define INCLUDE_CORE_VAS_BUFFER_H_

#include <string.h>
#include <stdint.h>

class VasBuffer {
public:
	VasBuffer(size_t capacity = 1024);
	virtual ~VasBuffer();
	size_t size() const {
		return _size;
	}
	size_t capacity() const {
		return _capacity;
	}
	void shrink(size_t count);
	size_t peek(char* data, size_t count) const;
	size_t read(const char* data, size_t count);
	void write(const char* data, size_t count);
	void append(const VasBuffer* buffer);
	void shrinkInt8();
	void shrinkUInt8();
	int8_t peekInt8() const;
	uint8_t peekUInt8() const;
	int8_t readInt8();
	uint8_t readUInt8();
	void writeInt8(const int8_t data);
	void writeUInt8(const uint8_t data);
	void shrinkInt16();
	void shrinkUInt16();
	int16_t peekInt16() const;
	uint16_t peekUInt16() const;
	int16_t readInt16();
	uint16_t readUInt16();
	void writeInt16(const int16_t data);
	void writeUInt16(const uint16_t data);
	void shrinkInt32();
	void shrinkUInt32();
	int32_t peekInt32() const;
	uint32_t peekUInt32() const;
	int32_t readInt32();
	uint32_t readUInt32();
	void writeInt32(const int32_t data);
	void writeUInt32(const uint32_t data);

private:
	void _inflate();
	size_t _size;
	size_t _capacity;
	char *_cursor_beg_ptr, *_cursor_end_ptr;
	char *_data_beg_ptr, *_data_end_ptr;
};
#endif /* INCLUDE_CORE_VAS_BUFFER_H_ */
