#pragma once
#include <cstdint>
#include <list>
#include <utility>

inline void* AllocAligned(size_t size) {
	return _aligned_malloc(size, 64);
}

template <typename T>
inline T* AllocAligned(size_t count) {
	return (T*)AllocAligned(count * sizeof(T));
}

class MemoryArena {
public:
	MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) { }
	void* Alloc(size_t nBytes);

	template<typename T>
	T* Alloc(size_t n = 1, bool runConstructor = true);
private:
	size_t currentBlockPos = 0, currentAllocSize = 0;
	uint8_t* currentBlock = nullptr;
	const size_t blockSize;
	std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};

template <typename T>
T* MemoryArena::Alloc(size_t n, bool runConstructor)
{
	T* ret = (T*)Alloc(n * sizeof(T));
	if (runConstructor)
		for (size_t i = 0; i < n; ++i)
			new(&ret[i]) T();
	return ret;
}

inline void* MemoryArena::Alloc(size_t nBytes)
{
	//Round up nBytes to minimum machine alignment 1075
	nBytes = ((nBytes + 15) & (~15));
	if (currentBlockPos + nBytes > currentAllocSize)
	{
		//Add current block to usedBlocks list 1075
		if (currentBlock)
		{
			usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
			currentBlock = nullptr;
		}
		//Get new block of memory for MemoryArena 1075
		for (auto iter = availableBlocks.begin(); iter != availableBlocks.end();
			++iter)
		{
			if (iter->first >= nBytes)
			{
				currentAllocSize = iter->first;
				currentBlock = iter->second;
				availableBlocks.erase(iter);
				break;
			}
		}
		if (!currentBlock)
		{
			currentAllocSize = std::max(nBytes, blockSize);
			currentBlock = AllocAligned<uint8_t>(currentAllocSize);
		}
		currentBlockPos = 0;
	}
	void* ret = currentBlock + currentBlockPos;
	currentBlockPos += nBytes;
	return ret;
}
