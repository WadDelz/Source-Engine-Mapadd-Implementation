#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <iostream>
#include <string>
#include <initializer_list>
#include <unordered_map>

//I made this dynamic array because I do not know how to use std::vector.
//like tf is a pull_back push_back or pop. so I just decided to make my own dynamic array

template <typename T>
class DynamicArray {
public:
	DynamicArray() : arr(nullptr), size(0), capacity(0) {}

	DynamicArray(std::initializer_list<T> initList) : arr(nullptr), size(0), capacity(0) {
		for (const T& element : initList) {
			addElement(element);
		}
	}

	~DynamicArray() {
		delete[] arr;
	}

	void addElement(const T& element) {
		if (size == capacity) {
			int newCapacity = (capacity == 0) ? 1 : capacity * 2;
			T* newArr = new T[newCapacity];

			for (int i = 0; i < size; ++i) {
				newArr[i] = arr[i];
			}

			delete[] arr;
			arr = newArr;
			capacity = newCapacity;
		}

		arr[size] = element;
		size++;
	}

	void deleteElement(int index) {
		if (index >= 0 && index < size) {
			for (int i = index; i < size - 1; ++i) {
				arr[i] = arr[i + 1];
			}

			size--;
		}
	}

	void changeElement(int index, const T& newValue) {
		if (index >= 0 && index < size) {
			arr[index] = newValue;
		}
	}

	void changePosition(int index1, int index2) {
		if (index1 >= 0 && index1 < size && index2 >= 0 && index2 < size) {
			T temp = arr[index1];
			arr[index1] = arr[index2];
			arr[index2] = temp;
		}
	}

	void changeValuePosition(const T& value1, const T& value2) {
		int index1 = -1;
		int index2 = -1;

		for (int i = 0; i < size; ++i) {
			if (arr[i] == value1) {
				index1 = i;
				if (index2 != -1) break;
			}
			if (arr[i] == value2) {
				index2 = i;
				if (index1 != -1) break;
			}
		}

		if (index1 != -1 && index2 != -1) {
			T temp = arr[index1];
			arr[index1] = arr[index2];
			arr[index2] = temp;
		}
	}

	void reverseArray() {
		for (int i = 0; i < size / 2; ++i) {
			T temp = arr[i];
			arr[i] = arr[size - 1 - i];
			arr[size - 1 - i] = temp;
		}
	}

	int getSize() const {
		return size;
	}

	int at(int index) {
		if (index <= 0 || index > size)
			return;

		return arr[index];
	}

	T& operator[](int index) {
		return arr[index];
	}
private:
	T* arr;
	int size;
	int capacity;
};

#endif
