#pragma once
#include <map>
#include <array>
#include <tuple>
#include <utility>

template<class ValueType, ValueType DefaultValue, std::size_t DimensionCount = 2>
class Matrix;

template<class ValueType, ValueType DefaultValue, std::size_t DimensionCount, std::size_t FixedCount>
class MatrixRow;

template<class ValueType, ValueType DefaultValue, std::size_t DimensionCount>
class Matrix {
public:
	Matrix() = default;

	int size() const {
		return (int)data_.size();
	}

	struct Key {
		std::array<int, DimensionCount> index;

		int& operator[](std::size_t k) {
			return index[k];
		}

		const int& operator[](std::size_t k) const {
			return index[k];
		}
	};

	struct KeyLess {
		bool operator()(const Key& left, const Key& right) const {
			for (std::size_t k = 0; k < DimensionCount; ++k) {
				if (left[k] != right[k]) {
					return left[k] < right[k];
				}
			}
			return false;
		}
	};

private:
	std::map<Key, ValueType, KeyLess> data_;

	ValueType get_(const Key& key) const {
		auto it = data_.find(key);
		if (it == data_.end()) {
			return DefaultValue;
		}
		return it->second;
	}

	void set_(const Key& key, const ValueType& value) {
		if (value == DefaultValue) {
			data_.erase(key);
		}
		else {
			data_[key] = value;
		}
	}

	template<class FriendValueType, FriendValueType FriendDefaultValue, std::size_t FriendDimensionCount, std::size_t FriendFixedCount>
	friend class MatrixRow;

public:
	MatrixRow<ValueType, DefaultValue, DimensionCount, 1> operator[](int firstIndex) {
		Key key{};
		key[0] = firstIndex;
		return MatrixRow<ValueType, DefaultValue, DimensionCount, 1>(this, key);
	}

	class iterator {
		typename std::map<Key, ValueType, KeyLess>::iterator it_;
	public:
		iterator(typename std::map<Key, ValueType, KeyLess>::iterator it)
			: it_(it) {
		}

		iterator& operator++() {
			++it_;
			return *this;
		}

		bool operator!=(const iterator& other) const {
			return it_ != other.it_;
		}

		template<std::size_t... Is>
		auto make_tuple_(std::index_sequence<Is...>) const
		{
			return std::make_tuple(it_->first[Is]..., it_->second);
		}

		auto operator*() const
		{
			return make_tuple_(std::make_index_sequence<DimensionCount>{});
		}
	};

	iterator begin() {
		return iterator(data_.begin());
	}

	iterator end() {
		return iterator(data_.end());
	}
};

template<class ValueType, ValueType DefaultValue, std::size_t DimensionCount, std::size_t FixedCount>
class MatrixRow {
	Matrix<ValueType, DefaultValue, DimensionCount>* matrix_;
	typename Matrix<ValueType, DefaultValue, DimensionCount>::Key key_;
public:
	MatrixRow(Matrix<ValueType, DefaultValue, DimensionCount>* matrix,
		const typename Matrix<ValueType, DefaultValue, DimensionCount>::Key& key)
		: matrix_(matrix), key_(key) {
	}

	MatrixRow<ValueType, DefaultValue, DimensionCount, FixedCount + 1> operator[](int nextIndex) {
		auto nextKey = key_;
		nextKey[FixedCount] = nextIndex;
		return MatrixRow<ValueType, DefaultValue, DimensionCount, FixedCount + 1>(matrix_, nextKey);
	}
};

template<class ValueType, ValueType DefaultValue, std::size_t DimensionCount>
class MatrixRow<ValueType, DefaultValue, DimensionCount, DimensionCount> {
	Matrix<ValueType, DefaultValue, DimensionCount>* matrix_;
	typename Matrix<ValueType, DefaultValue, DimensionCount>::Key key_;
public:
	MatrixRow(Matrix<ValueType, DefaultValue, DimensionCount>* matrix,
		const typename Matrix<ValueType, DefaultValue, DimensionCount>::Key& key)
		: matrix_(matrix), key_(key) {
	}

	operator ValueType() const {
		return matrix_->get_(key_);
	}

	MatrixRow operator=(const ValueType& value) {
		matrix_->set_(key_, value);
		return *this;
	}
};

