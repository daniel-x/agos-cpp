#include "ExecutableMemory.h"

#include <cstdint> /* uint64_t, ... */
#include <cstring> /* memcmp, memset */
#include <cstdio> /* printf */
#include <sys/mman.h> /* mmap, ... */

namespace agos {

ExecutableMemory::ExecutableMemory() :
		a(NULL), size(0) {
}

ExecutableMemory::ExecutableMemory(size_t size) {
	alloc(size);
}

ExecutableMemory::ExecutableMemory(ExecutableMemory const& o) {
	if (o.a != NULL) {
		alloc(o.size);
		std::memcpy(a, o.a, size);
	} else {
		a = NULL;
		size = 0;
	}
}

ExecutableMemory::~ExecutableMemory() {
	dealloc();
}

ExecutableMemory& ExecutableMemory::operator=(ExecutableMemory const& o) {
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

bool ExecutableMemory::operator==(ExecutableMemory const& o) const {
	return (size == o.size) && !std::memcmp(a, o.a, size);
}

bool ExecutableMemory::operator!=(ExecutableMemory const& o) const {
	return (size != o.size) || std::memcmp(a, o.a, size);
}

uint8_t& ExecutableMemory::operator[](size_t idx) {
	return a[idx];
}

uint8_t const& ExecutableMemory::operator[](size_t idx) const {
	return a[idx];
}

void ExecutableMemory::setSize(size_t size) {
	if (this->size != size) {
		if (a == NULL) {
			alloc(size);
		} else {
			realloc(size);
		}
	}
}

size_t ExecutableMemory::getSize() const {
	return size;
}

uint8_t* ExecutableMemory::begin() {
	return a;
}

uint8_t const* ExecutableMemory::begin() const {
	return a;
}

uint8_t* ExecutableMemory::end() {
	return a + size;
}

uint8_t const* ExecutableMemory::end() const {
	return a + size;
}

void ExecutableMemory::fill(uint8_t v) {
	std::memset(a, v, size);
}

void ExecutableMemory::alloc(size_t size) {
	uint8_t* a_ = (uint8_t*) mmap( //
			NULL,// addr
			size, // size
			PROT_READ | PROT_WRITE | PROT_EXEC, // protection (permissions)
			MAP_PRIVATE | MAP_ANONYMOUS, //
			-1, // file descriptor
			0 // offset
			);

	if (a_ == MAP_FAILED) {
		printf("%s:%i: mmap() of %li bytes failed\n", __FILE__, __LINE__, size);
	} else {
		a = a_;
		this->size = size;
	}
}

void ExecutableMemory::realloc(size_t size) {
	uint8_t* a_ = (uint8_t*) mremap( //
			a, // addr
			this->size, // old len
			size, // new len
			MREMAP_MAYMOVE // flags
			/*, NULL // new address */
			);

	if (a_ == MAP_FAILED) {
		printf("%s:%i: mmap() of %li bytes failed\n", __FILE__, __LINE__, size);
	} else {
		a = a_;
		this->size = size;
	}
}

void ExecutableMemory::dealloc() {
	if (a != NULL) {
		munmap(a, size);
		a = NULL;
		size = 0;
	}
}

} /* namespace agos */
