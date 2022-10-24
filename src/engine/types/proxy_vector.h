#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "utils.h"

// This datatype is intended to serve as a single entry point for multiple
// internal vectors and allows for much easy indexing and iterating

// Inspired by: https://stackoverflow.com/a/55838758
// Extended to support 'n' amount of vectors

template <class T>
class ProxyVector {
   public:
	ProxyVector() {}

	inline void Add(std::vector<T>* vector) { m_Vectors.push_back(vector); }
	inline void RemoveAt(int index) {
		m_Vectors.erase(m_Vectors.begin() + index);
	}

	inline size_t FindIndex(std::function<bool(const T&)> predicate) {
		for (size_t i = 0; i < size(); ++i) {
			if (predicate((*this)[i])) return i;
		}

		return -1;
	}

	const T& operator[](const size_t& i) const;
	const size_t size() const;

   private:
	std::vector<std::vector<T>*> m_Vectors;
};

template <class T>
const T& ProxyVector<T>::operator[](const size_t& i) const {
	int offset = 0;
	for (auto& vector : m_Vectors) {
		if (i < vector->size() + offset) {
			return vector->data()[i - offset];
		}

		offset += vector->size();
	}

	auto& lastVec = m_Vectors[m_Vectors.size() - 1];
	return lastVec->data()[lastVec->size() - 1];
};

template <class T>
const size_t ProxyVector<T>::size() const {
	int size = 0;

	for (auto* vector : m_Vectors) {
		size += vector->size();
	}

	return size;
};