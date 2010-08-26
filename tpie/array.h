// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; eval: (progn (c-set-style "stroustrup") (c-set-offset 'innamespace 0)); -*-
// vi:set ts=4 sts=4 sw=4 noet :
// Copyright 2008, The TPIE development team
// 
// This file is part of TPIE.
// 
// TPIE is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// TPIE is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with TPIE.  If not, see <http://www.gnu.org/licenses/>
#ifndef __TPIE_ARRAY_H__
#define __TPIE_ARRAY_H__

///////////////////////////////////////////////////////////////////////////
/// \file array.h
/// Contains a generic array with known memory requirements
///////////////////////////////////////////////////////////////////////////

#include <tpie/util.h>
#include <tpie/mm.h>

namespace tpie {

template <typename T>
class array: public linear_memory_base<array<T> > {
private:
	T * m_elements;
	size_t m_size;

	template <typename TT, bool forward> 
	class ibase {
	private:
		TT * elm;
		inline ibase(TT * e): elm(e) {}
		friend class array;
	public:
		inline ibase(): elm(0) {}
		inline bool operator != (const ibase & other) const{return elm != other.elm;}
		inline bool operator == (const ibase & other) const {return elm == other.elm;}
		inline void operator++() {elm += forward?1:-1;}
		inline void operator--() {elm += forward?-1:1;}
		inline void operator +=(size_type dist) {elm += forward?dist:-dist;}
		inline void operator -=(size_type dist) {elm += forward?-dist:dist;}
		inline const T & operator*() const {return *elm;}
		inline const T & operator->() const {return *elm;}
		inline ptrdiff_t operator-(const ibase & other) const {
			return elm - other.elm;
		}
	};

	template <bool forward>
	class ibase_d: public ibase<T, forward> {
	private:
		inline ibase_d(T * e): ibase<T, forward>(e) {};
		friend class array;
	public:
		inline ibase_d(): ibase<T, forward>(0) {};
		inline T & operator*() {return *ibase<T, forward>::elm;}
		inline T & operator->() {return *ibase<T, forward>::elm;}
		inline operator ibase<const T, forward>() const {
			return ibase<const T, true>(ibase<T, forward>::elm);
		}
	};

public:
	typedef T value_type;
	typedef ibase<const T, true> const_iterator;
	typedef ibase<const T, false> const_reverse_iterator;
	typedef ibase_d<true> iterator;
	typedef ibase_d<false> reverse_iterator;


	static double memory_coefficient() {
		return sizeof(T);
	}

	static double memory_overhead() {
		return sizeof(array) + MM_manager.space_overhead();
	}

	array(): m_elements(0), m_size(0) {};
	array(size_type s): m_elements(0), m_size(0) {resize(s);}
	array(size_type s, const T & elm): m_elements(0), m_size(0) {resize(s, elm);}

	array(const array & other): m_elements(0), m_size(0) {
		resize(other.size());
		for (size_t i=0; i < m_size; ++i) m_elements[i] = other[i];
	}	
	~array() {resize(0);}
	void operator=(const array & other){
		resize(other.size());
		for (size_t i=0; i < m_size; ++i) m_elements[i] = other[i];
	}
	void resize(size_t s) {
		if (s == m_size) return;
		delete[] m_elements;
		m_size = s;
		m_elements = s ? new T[m_size] : 0;
	}
	
	void resize(size_type s, const T & elm) {
		resize(s);
		for (size_type i=0; i < m_size; ++i) m_elements[i] = elm;
	}

	inline size_type size() const {return m_size;}
	inline bool empty() const {return m_size == 0;}
	inline const T & operator[](size_type i) const {
		assert(i < m_size);
		return m_elements[i];
	}
	inline T & operator[](size_type i) {
		assert(i < m_size);
		return m_elements[i];
	}
	inline iterator find(size_type i) {return iterator(m_elements+i);}
	inline const_iterator find(size_type i) const {return const_iterator(m_elements+i);}
	inline bool operator!=(const array<T> & other) const {
		if(m_size != other.size()) return true;
		for(size_t i=0;i<m_size;++i) if(m_elements[i] != other[i]) return true;
		return false;
	}
				   
	inline bool operator==(const array<T>   & other) const {
 		if(m_size != other.size()) return false;
		for(size_t i=0;i<m_size;++i) if(m_elements[i] != other[i]) return false;
		return true;
	}

	inline iterator begin() {return iterator(m_elements);}
	inline const_iterator begin() const {return const_iterator(m_elements);}
	inline iterator end() {return iterator(m_elements+m_size);}
	inline const_iterator end() const {return const_iterator(m_elements+m_size);}
	inline reverse_iterator rbegin() {return reverse_iterator(m_elements+m_size-1);}
	inline const_reverse_iterator rbegin() const {return const_reverse_iterator(m_elements+m_size-1);}
	inline reverse_iterator rend() {return reverse_iterator(m_elements-1);}
	inline const reverse_iterator rend() const {return const_reverse_iterator(m_elements-1);}
};
}
#endif //__TPIE_ARRAY_H__ 	

