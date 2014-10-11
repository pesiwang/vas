/*
 * buffer.cpp
 *
 *  Created on: Oct 8, 2014
 *	  Author: chenrui
 */

#include <stdlib.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "exception.h"

using namespace vas;

Buffer::Buffer(size_t capacity) : _size(0), _capacity(capacity)
{
	_data_beg_ptr = (char*)malloc(_capacity);
	_data_end_ptr = _data_beg_ptr + _capacity;
	_cursor_beg_ptr = _data_beg_ptr;
	_cursor_end_ptr = _cursor_beg_ptr;
}

Buffer::~Buffer()
{
	free(_data_beg_ptr);
}

void Buffer::shrink(size_t count)
{
	if(count == 0) return;
	if(count > this->size())
		throw Exception("not enough data to shrink");

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		_cursor_beg_ptr += count;
		if(_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		size_t n_items_2 = count - n_items_1;
		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
}

void Buffer::read(const char* data, size_t count)
{
	if(count == 0) return;
	if(count > size())
		throw Exception("not enough data to read");

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy((void *)data, _cursor_beg_ptr, count);
		_cursor_beg_ptr += count;
		if(_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy((void *)data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy((void *)(data + n_items_1), _data_beg_ptr, n_items_2);

		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
}

void Buffer::peek(char* data, size_t count) const
{
	if(count == 0) return;
	if(count > size())
		throw Exception("not enough data to peek");

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy(data, _cursor_beg_ptr, count);
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy(data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(data + n_items_1, _data_beg_ptr, n_items_2);
	}
}

void Buffer::write(const char* data, size_t count)
{
	if(count == 0) return;
	while(count > (capacity() - size()))
		this->_inflate();

	if(_cursor_end_ptr + count <= _data_end_ptr){
		memcpy(_cursor_end_ptr, data, count);
		_cursor_end_ptr += count;
		if (_cursor_end_ptr == _data_end_ptr)
			_cursor_end_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_end_ptr;
		memcpy(_cursor_end_ptr, data, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(_data_beg_ptr, data + n_items_1, n_items_2);

		_cursor_end_ptr = _data_beg_ptr + n_items_2;
	}
	_size += count;
	return;
}

void Buffer::append(const Buffer* buffer)
{
	size_t count = buffer->size();
	while(count > (capacity() - size()))
		this->_inflate();

	if(count < (size_t)(buffer->_data_end_ptr - buffer->_cursor_beg_ptr)){
		this->write(buffer->_cursor_beg_ptr, count);
	}
	else{
		size_t n_items_1 = buffer->_data_end_ptr - buffer->_cursor_beg_ptr;
		this->write(buffer->_cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		this->write(buffer->_data_beg_ptr, n_items_2);
	}
	return;
}

void Buffer::shrinkInt8()
{
	this->shrink(sizeof(int8_t));
}

void Buffer::shrinkUInt8()
{
	this->shrink(sizeof(uint8_t));
}

int8_t Buffer::peekInt8() const
{
	int8_t val;
	this->peek((char*)&val, sizeof(val));
	return val;
}

uint8_t Buffer::peekUInt8() const
{
	uint8_t val;
	this->peek((char*)&val, sizeof(val));
	return val;
}

int8_t Buffer::readInt8()
{
	int8_t val;
	this->read((char*)&val, sizeof(val));
	return val;
}

uint8_t Buffer::readUInt8()
{
	uint8_t val;
	this->read((char*)&val, sizeof(val));
	return val;
}

void Buffer::writeInt8(const int8_t data)
{
	this->write((char *)&data, sizeof(data));
}

void Buffer::writeUInt8(const uint8_t data)
{
	this->write((char *)&data, sizeof(data));
}

void Buffer::shrinkInt16()
{
	this->shrink(sizeof(int16_t));
}

void Buffer::shrinkUInt16()
{
	this->shrink(sizeof(uint16_t));
}

int16_t Buffer::peekInt16() const
{
	int16_t val;
	this->peek((char*)&val, sizeof(val));
	return ntohs(val);
}

uint16_t Buffer::peekUInt16() const
{
	uint16_t val;
	this->peek((char*)&val, sizeof(val));
	return ntohs(val);
}

int16_t Buffer::readInt16()
{
	int16_t val;
	this->read((char*)&val, sizeof(val));
	return ntohs(val);
}

uint16_t Buffer::readUInt16()
{
	uint16_t val;
	this->read((char*)&val, sizeof(val));
	return ntohs(val);
}

void Buffer::writeInt16(const int16_t data)
{
	int16_t val = htons(data);
	this->write((char*)&val, sizeof(val));
}

void Buffer::writeUInt16(const uint16_t data)
{
	uint16_t val = htons(data);
	this->write((char*)&val, sizeof(val));
}

void Buffer::shrinkInt32()
{
	this->shrink(sizeof(int32_t));
}

void Buffer::shrinkUInt32()
{
	this->shrink(sizeof(uint32_t));
}

int32_t Buffer::peekInt32() const
{
	int32_t val;
	this->peek((char*)&val, sizeof(val));
	return ntohl(val);
}

uint32_t Buffer::peekUInt32() const
{
	uint32_t val;
	this->peek((char*)&val, sizeof(val));
	return ntohl(val);
}

int32_t Buffer::readInt32()
{
	int32_t val;
	this->read((char*)&val, sizeof(val));
	return ntohl(val);
}

uint32_t Buffer::readUInt32()
{
	uint32_t val;
	this->read((char*)&val, sizeof(val));
	return ntohl(val);
}

void Buffer::writeInt32(const int32_t data)
{
	int32_t val = htonl(data);
	this->write((char*)&val, sizeof(val));
}

void Buffer::writeUInt32(const uint32_t data)
{
	uint32_t val = htonl(data);
	this->write((char*)&val, sizeof(val));
}

void Buffer::_inflate()
{
	size_t newCapacity = capacity() * 2;
	size_t cursor_beg_ofset = _cursor_beg_ptr - _data_beg_ptr;

	size_t sizeToMove = 0;
	if(size() > (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		sizeToMove = size() - (_data_end_ptr - _cursor_beg_ptr);
	}

	_data_beg_ptr = (char*)realloc(_data_beg_ptr, newCapacity);
	_data_end_ptr = _data_beg_ptr + newCapacity;
	_cursor_beg_ptr = _data_beg_ptr + cursor_beg_ofset;
	_cursor_end_ptr = _cursor_beg_ptr + size();
	_capacity = newCapacity;

	if(sizeToMove > 0)
		memcpy(_cursor_beg_ptr + (size() - sizeToMove), _data_beg_ptr, sizeToMove);
}
