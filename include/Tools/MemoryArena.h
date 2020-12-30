
/*
	pbrt source code is Copyright(c) 1998-2016
						Matt Pharr, Greg Humphreys, and Wenzel Jakob.

	This file is part of pbrt.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

	- Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.

	- Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
	IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

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

inline void FreeAligned(void* ptr) {
	if (!ptr) return;
	_aligned_free(ptr);
}

class MemoryArena {
public:
	MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) { }
	void* Alloc(size_t nBytes);

	template<typename T>
	T* Alloc(size_t n = 1, bool runConstructor = true);

	void Reset();


	~MemoryArena() {
		FreeAligned(currentBlock);
		for (auto& block : usedBlocks) FreeAligned(block.second);
		for (auto& block : availableBlocks) FreeAligned(block.second);
	}
	
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

inline void MemoryArena::Reset()
{
	currentBlockPos = 0;
	availableBlocks.splice(availableBlocks.begin(), usedBlocks);
}

#define ARENA_ALLOC(arena, Type) new (arena.Alloc(sizeof(Type))) Type
