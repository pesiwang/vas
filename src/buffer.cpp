#include <stdlib.h>
#include <arpa/inet.h>
#include "buffer.h"

CBuffer::CBuffer(size_t capacity) : _size(0), _capacity(capacity)
{
	_data_beg_ptr = (char*)malloc(_capacity);
	_data_end_ptr = _data_beg_ptr + _capacity;
	_cursor_beg_ptr = _data_beg_ptr;
	_cursor_end_ptr = _cursor_beg_ptr;
}

CBuffer::~CBuffer()
{
	free(_data_beg_ptr);
}

size_t CBuffer::shrink(size_t count)
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

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
	return count;
}

size_t CBuffer::read(char* data, size_t count)
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy(data, _cursor_beg_ptr, count);
		_cursor_beg_ptr += count;
		if(_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy(data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(data + n_items_1, _data_beg_ptr, n_items_2);
		
		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
	return count;
}

size_t CBuffer::peek(char* data, size_t count) const
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy(data, _cursor_beg_ptr, count);
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy(data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(data + n_items_1, _data_beg_ptr, n_items_2);
	}
	return count;
}

void CBuffer::write(const char* data, size_t count)
{
	if(count == 0) return;
	while(count > (capacity() - size()))
		this->inflate();

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

void CBuffer::append(CBuffer* buffer)
{
	size_t count = buffer->size();
	while(count > (capacity() - size()))
		this->inflate();

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

void CBuffer::shrinkByte()
{
	this->shrink(sizeof(char));
}

char CBuffer::peekByte() const
{
	char val;
	this->peek((char *)&val, sizeof(val));
	return val;
}

char CBuffer::readByte()
{
	char val;
	this->read((char *)&val, sizeof(val));
	return val;
}

void CBuffer::writeByte(const char data)
{
	char val = data;
	this->write((char *)&val, sizeof(val));
}

void CBuffer::shrinkUnsignedByte()
{
	this->shrink(sizeof(unsigned char));
}

unsigned char CBuffer::peekUnsignedByte() const
{
	unsigned char val;
	this->peek((char *)&val, sizeof(val));
	return val;
}

unsigned char CBuffer::readUnsignedByte()
{
	unsigned char val;
	this->read((char *)&val, sizeof(val));
	return val;
}

void CBuffer::writeUnsignedByte(const unsigned char data)
{
	unsigned char val = data;
	this->write((char *)&val, sizeof(val));
}

void CBuffer::shrinkShort()
{
	this->shrink(sizeof(short));
}

short CBuffer::peekShort() const
{
	short val;
	this->peek((char *)&val, sizeof(val));
	return ntohs(val);
}

short CBuffer::readShort()
{
	short val;
	this->read((char *)&val, sizeof(val));
	return ntohs(val);
}

void CBuffer::writeShort(const short data)
{
	short val = htons(data);
	this->write((char *)&val, sizeof(val));
}

void CBuffer::shrinkUnsignedShort()
{
	this->shrink(sizeof(unsigned short));
}

unsigned short CBuffer::peekUnsignedShort() const
{
	unsigned short val;
	this->peek((char *)&val, sizeof(val));
	return ntohs(val);
}

unsigned short CBuffer::readUnsignedShort()
{
	unsigned short val;
	this->read((char *)&val, sizeof(val));
	return ntohs(val);
}

void CBuffer::writeUnsignedShort(const unsigned short data)
{
	unsigned short val = htons(data);
	this->write((char *)&val, sizeof(val));
}

void CBuffer::shrinkInt()
{
	this->shrink(sizeof(int));
}

int CBuffer::peekInt() const
{
	int val;
	this->peek((char *)&val, sizeof(val));
	return ntohl(val);
}

int CBuffer::readInt()
{
	int val;
	this->read((char *)&val, sizeof(val));
	return ntohl(val);
}

void CBuffer::writeInt(const int data)
{
	int val = htonl(data);
	this->write((char *)&val, sizeof(val));
}

void CBuffer::shrinkUnsignedInt()
{
	this->shrink(sizeof(unsigned int));
}

unsigned int CBuffer::peekUnsignedInt() const
{
	unsigned int val;
	this->peek((char *)&val, sizeof(val));
	return ntohl(val);
}

unsigned int CBuffer::readUnsignedInt()
{
	unsigned int val;
	this->read((char *)&val, sizeof(val));
	return ntohl(val);
}

void CBuffer::writeUnsignedInt(const unsigned int data)
{
	unsigned int val = htonl(data);
	this->write((char *)&val, sizeof(val));
}

///////////////////////////////////////////////
void CBuffer::inflate()
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
