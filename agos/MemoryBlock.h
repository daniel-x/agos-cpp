#ifndef AGOS_MEMORYBLOCK_H_
#define AGOS_MEMORYBLOCK_H_

#include <cstddef>
#include <cstdint>

namespace agos {

class MemoryBlock {
private:
	uint8_t* a;
	size_t size;

public:
	MemoryBlock();
	MemoryBlock(size_t size);
	MemoryBlock(MemoryBlock const& o);
	~MemoryBlock();

	MemoryBlock& operator=(MemoryBlock const& o);

	void setSize(size_t size);
	size_t getSize() const;

	uint8_t* begin();
	uint8_t* end();
	uint8_t const* begin() const;
	uint8_t const* end() const;

	bool operator==(MemoryBlock const& o) const;
	bool operator!=(MemoryBlock const& o) const;

	uint8_t& operator[](size_t idx);
	uint8_t const& operator[](size_t idx) const;

	void fill(uint8_t v);

private:
	void alloc(size_t size);
	void realloc(size_t size);
	void dealloc();
};

} /* namespace agos */

#endif /* AGOS_MEMORYBLOCK_H_ */
