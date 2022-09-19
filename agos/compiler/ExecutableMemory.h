#ifndef AGOS_COMPILER_EXECUTABLEMEMORY_H_
#define AGOS_COMPILER_EXECUTABLEMEMORY_H_

#include <cstddef>
#include <cstdint>

namespace agos {

class ExecutableMemory {
private:
	uint8_t* a;
	size_t size;

public:
	ExecutableMemory();
	ExecutableMemory(size_t size);
	ExecutableMemory(ExecutableMemory const& o);
	~ExecutableMemory();

	ExecutableMemory& operator=(ExecutableMemory const& o);

	void setSize(size_t size);
	size_t getSize() const;

	uint8_t* begin();
	uint8_t* end();
	uint8_t const* begin() const;
	uint8_t const* end() const;

	bool operator==(ExecutableMemory const& o) const;
	bool operator!=(ExecutableMemory const& o) const;

	uint8_t& operator[](size_t idx);
	uint8_t const& operator[](size_t idx) const;

	void fill(uint8_t v);

private:
	void alloc(size_t size);
	void realloc(size_t size);
	void dealloc();
};

} /* namespace agos */

#endif /* AGOS_COMPILER_EXECUTABLEMEMORY_H_ */
