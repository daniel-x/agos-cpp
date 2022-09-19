#include "MemoryBlock.h"

#include <cstdint> /* uint8_t, ... */
#include <cstring> /* memcmp, memset */
#include <cstdio> /* printf */
#include <cstdlib> /* realloc, free, exit, NULL */

namespace agos {

MemoryBlock::MemoryBlock() :
		a(NULL), size(0) {
}

MemoryBlock::MemoryBlock(size_t size) {
	alloc(size);
}

MemoryBlock::MemoryBlock(MemoryBlock const& o) {
	if (o.a != NULL) {
		alloc(o.size);
		std::memcpy(a, o.a, size);
	} else {
		a = NULL;
		size = 0;
	}
}

MemoryBlock::~MemoryBlock() {
	dealloc();
}

MemoryBlock& MemoryBlock::operator=(MemoryBlock const& o) {
	if (o.a != NULL) {
		if (o.size <= size) {
			realloc(o.size);
		} else {
			dealloc();
			realloc(o.size);
		}

		std::memcpy(a, o.a, size);
	} else {
		dealloc();
	}

	return *this;
}

bool MemoryBlock::operator==(MemoryBlock const& o) const {
	return (size == o.size) && !std::memcmp(a, o.a, size);
}

bool MemoryBlock::operator!=(MemoryBlock const& o) const {
	return (size != o.size) || std::memcmp(a, o.a, size);
}

uint8_t& MemoryBlock::operator[](size_t idx) {
	return a[idx];
}

uint8_t const& MemoryBlock::operator[](size_t idx) const {
	return a[idx];
}

void MemoryBlock::setSize(size_t size) {
	if (this->size != size) {
		if (a == NULL) {
			alloc(size);
		} else {
			realloc(size);
		}
	}
}

size_t MemoryBlock::getSize() const {
	return size;
}

uint8_t* MemoryBlock::begin() {
	return a;
}

uint8_t const* MemoryBlock::begin() const {
	return a;
}

uint8_t* MemoryBlock::end() {
	return a + size;
}

uint8_t const* MemoryBlock::end() const {
	return a + size;
}

void MemoryBlock::fill(uint8_t v) {
	std::memset(a, v, size);
}

void MemoryBlock::alloc(size_t size) {
	uint8_t* a_ = (uint8_t*) std::malloc(size);

	if (a_ == NULL) {
		printf("%s:%i: std::malloc() of %li bytes failed\n", __FILE__, __LINE__,
				size);
		a = NULL;
		this->size = 0;
	} else {
		a = a_;
		this->size = size;
	}
}

void MemoryBlock::realloc(size_t size) {
	uint8_t* a_ = (uint8_t*) std::realloc(a, size);

	if (a_ == NULL) {
		printf("%s:%i: std::realloc() from %li to %li bytes failed\n", __FILE__,
				__LINE__, this->size, size);
	} else {
		a = a_;
		this->size = size;
	}
}

void MemoryBlock::dealloc() {
	if (a != NULL) {
		free(a);
		a = NULL;
		size = 0;
	}
}

} /* namespace agos */
