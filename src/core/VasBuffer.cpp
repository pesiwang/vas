/*
 * VasBuffer.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#include <stdlib.h>
#include <arpa/inet.h>
#include "core/VasBuffer.h"

VasBuffer::VasBuffer(size_t capacity) : _size(0), _capacity(capacity) {
	_data_beg_ptr = (char*) malloc(_capacity);
	_data_end_ptr = _data_beg_ptr + _capacity;
	_cursor_beg_ptr = _data_beg_ptr;
	_cursor_end_ptr = _cursor_beg_ptr;
}

VasBuffer::~VasBuffer() {
	free(_data_beg_ptr);
}

void VasBuffer::shrink(size_t count) {
	if (count == 0)
		return;
	if (count > this->size())
		count = this->size();
	if (count < (size_t) (_data_end_ptr - _cursor_beg_ptr)) {
		_cursor_beg_ptr += count;
		if (_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	} else {
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		size_t n_items_2 = count - n_items_1;
		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
}

size_t VasBuffer::read(const char* data, size_t count) {
	if (count == 0)
		return 0;
	if (count > this->size())
		count = this->size();
	if (count < (size_t) (_data_end_ptr - _cursor_beg_ptr)) {
		memcpy((void *) data, _cursor_beg_ptr, count);
		_cursor_beg_ptr += count;
		if (_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	} else {
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy((void *) data, _cursor_beg_ptr, n_items_1);
		size_t n_items_2 = count - n_items_1;
		memcpy((void *) (data + n_items_1), _data_beg_ptr, n_items_2);
		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
	return count;
}

size_t VasBuffer::peek(char* data, size_t count) const {
	if (count == 0)
		return 0;
	if (count > this->size())
		count = this->size();
	if (count < (size_t) (_data_end_ptr - _cursor_beg_ptr)) {
		memcpy(data, _cursor_beg_ptr, count);
	} else {
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy(data, _cursor_beg_ptr, n_items_1);
		size_t n_items_2 = count - n_items_1;
		memcpy(data + n_items_1, _data_beg_ptr, n_items_2);
	}
	return count;
}

void VasBuffer::write(const char* data, size_t count) {
	if (count == 0)
		return;
	while (count > (capacity() - size()))
		this->_inflate();
	if (_cursor_end_ptr + count <= _data_end_ptr) {
		memcpy(_cursor_end_ptr, data, count);
		_cursor_end_ptr += count;
		if (_cursor_end_ptr == _data_end_ptr)
			_cursor_end_ptr = _data_beg_ptr;
	} else {
		size_t n_items_1 = _data_end_ptr - _cursor_end_ptr;
		memcpy(_cursor_end_ptr, data, n_items_1);
		size_t n_items_2 = count - n_items_1;
		memcpy(_data_beg_ptr, data + n_items_1, n_items_2);
		_cursor_end_ptr = _data_beg_ptr + n_items_2;
	}
	_size += count;
	return;
}

void VasBuffer::append(const VasBuffer* buffer) {
	size_t count = buffer->size();
	while (count > (capacity() - size()))
		this->_inflate();
	if (count < (size_t) (buffer->_data_end_ptr - buffer->_cursor_beg_ptr)) {
		this->write(buffer->_cursor_beg_ptr, count);
	} else {
		size_t n_items_1 = buffer->_data_end_ptr - buffer->_cursor_beg_ptr;
		this->write(buffer->_cursor_beg_ptr, n_items_1);
		size_t n_items_2 = count - n_items_1;
		this->write(buffer->_data_beg_ptr, n_items_2);
	}
	return;
}

void VasBuffer::shrinkInt8() {
	this->shrink(sizeof(int8_t));
}

void VasBuffer::shrinkUInt8() {
	this->shrink(sizeof(uint8_t));
}

int8_t VasBuffer::peekInt8() const {
	int8_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return val;
	return 0;
}

uint8_t VasBuffer::peekUInt8() const {
	uint8_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return val;
	return 0;
}

int8_t VasBuffer::readInt8() {
	int8_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return val;
	return 0;
}

uint8_t VasBuffer::readUInt8() {
	uint8_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return val;
	return 0;
}

void VasBuffer::writeInt8(const int8_t data) {
	this->write((char *) &data, sizeof(data));
}

void VasBuffer::writeUInt8(const uint8_t data) {
	this->write((char *) &data, sizeof(data));
}

void VasBuffer::shrinkInt16() {
	this->shrink(sizeof(int16_t));
}

void VasBuffer::shrinkUInt16() {
	this->shrink(sizeof(uint16_t));
}

int16_t VasBuffer::peekInt16() const {
	int16_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohs(val);
	return 0;
}

uint16_t VasBuffer::peekUInt16() const {
	uint16_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohs(val);
	return 0;
}

int16_t VasBuffer::readInt16() {
	int16_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohs(val);
	return 0;
}

uint16_t VasBuffer::readUInt16() {
	uint16_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohs(val);
	return 0;
}

void VasBuffer::writeInt16(const int16_t data) {
	int16_t val = htons(data);
	this->write((char*) &val, sizeof(val));
}

void VasBuffer::writeUInt16(const uint16_t data) {
	uint16_t val = htons(data);
	this->write((char*) &val, sizeof(val));
}

void VasBuffer::shrinkInt32() {
	this->shrink(sizeof(int32_t));
}

void VasBuffer::shrinkUInt32() {
	this->shrink(sizeof(uint32_t));
}

int32_t VasBuffer::peekInt32() const {
	int32_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohl(val);
	return 0;
}

uint32_t VasBuffer::peekUInt32() const {
	uint32_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohl(val);
	return 0;
}

int32_t VasBuffer::readInt32() {
	int32_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohl(val);
	return 0;
}

uint32_t VasBuffer::readUInt32() {
	uint32_t val = 0;
	if(sizeof(val) == this->peek((char*) &val, sizeof(val)))
		return ntohl(val);
	return 0;
}

void VasBuffer::writeInt32(const int32_t data) {
	int32_t val = htonl(data);
	this->write((char*) &val, sizeof(val));
}

void VasBuffer::writeUInt32(const uint32_t data) {
	uint32_t val = htonl(data);
	this->write((char*) &val, sizeof(val));
}

void VasBuffer::_inflate() {
	size_t newCapacity = capacity() * 2;
	size_t cursor_beg_ofset = _cursor_beg_ptr - _data_beg_ptr;
	size_t sizeToMove = 0;
	if (size() > (size_t) (_data_end_ptr - _cursor_beg_ptr)) {
		sizeToMove = size() - (_data_end_ptr - _cursor_beg_ptr);
	}
	_data_beg_ptr = (char*) realloc(_data_beg_ptr, newCapacity);
	_data_end_ptr = _data_beg_ptr + newCapacity;
	_cursor_beg_ptr = _data_beg_ptr + cursor_beg_ofset;
	_cursor_end_ptr = _cursor_beg_ptr + size();
	_capacity = newCapacity;
	if (sizeToMove > 0)
		memcpy(_cursor_beg_ptr + (size() - sizeToMove), _data_beg_ptr,
				sizeToMove);
}
