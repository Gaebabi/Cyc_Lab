// File: Interval.cpp
// Author: Lucas Kovar


#include <iostream>
#include <sstream>
#include "Interval.h"


Interval::Interval() :
	m_isEmpty(true),
	m_start(0),
	m_end(0)
{
}




Interval::Interval(float start, float end) :
	m_start(start),
	m_end(end)
{


	if (m_start > m_end) {
		std::cout << "Warning: tried to make backwards Interval [" << m_start << "," << m_end << "]; setting empty" << std::endl;
		m_isEmpty = true;
	}
	else {
		m_isEmpty = false;
	}
}




Interval::Interval(const Interval& interval) :
	m_start(interval.m_start),
	m_end(interval.m_end),
	m_isEmpty(interval.m_isEmpty)
{
}



float Interval::getStart() const {
	return(m_start);
}

float Interval::getEnd() const {
	return(m_end);
}

void Interval::setStart(float start) {
	m_start = start;

	if (m_isEmpty) {
		m_end = m_start;
		m_isEmpty = false;
	}
	else if (m_end < m_start) {	// An error has occurred: the desired start value exceeds the end!
		std::cerr << "Error in Interval::start( T ): tried to set start larger than end.  Setting to empty" << std::endl;
		m_isEmpty = true;
	}
	else {
		m_isEmpty = false;
	}
}

void Interval::setEnd(float end) {
	m_end = end;
	if (m_isEmpty) {
		m_start = m_end;
		m_isEmpty = false;
	}
	else if (m_end < m_start) {	// An error has occurred: the desired end value is less than the start!
		std::cerr << "Error in Interval::end( T ): tried to set end smaller than start.  Setting to empty" << std::endl;
		m_isEmpty = true;
	}
	else {
		m_isEmpty = false;
	}
}

void Interval::set(float start, float end) {
	m_start = start;
	m_end = end;
	m_isEmpty = false;

	if (m_start > m_end) {
		std::cerr << "Error in Interval::set( T, T ): end is less than start!.  Setting to empty" << std::endl;
		m_isEmpty = true;
	}
}

Interval& Interval::operator = (const Interval& interval) {
	m_start = interval.m_start;
	m_end = interval.m_end;
	m_isEmpty = interval.m_isEmpty;

	return(*this);
}

void Interval::copy(Interval interval) {
	m_start = interval.m_start;
	m_end = interval.m_end;
	m_isEmpty = interval.m_isEmpty;
}

bool Interval::isEmpty() const {
	return(m_isEmpty != 0);
}

void Interval::setToEmpty() {
	m_isEmpty = true;
	m_start = 0;
	m_end = 0;
}

float Interval::getSize() const {
	if (m_isEmpty) {
		return(0);
	}

	return(m_end - m_start + 1);
}

float Interval::getNumElems() const {
	if (m_isEmpty) {
		return(0);
	}
	return(m_end - m_start + 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Comparators
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Interval::operator < (const Interval& interval) const {
	if (m_isEmpty) {
		return(!interval.m_isEmpty);
	}
	else if (interval.m_isEmpty) {
		return(false);
	}

	return((m_start < interval.m_start) || (m_start == interval.m_start && m_end < interval.m_end));
}

bool Interval::operator > (const Interval& interval) const {
	if (m_isEmpty) {
		return(false);
	}
	else if (interval.m_isEmpty) {
		return(!m_isEmpty);
	}

	return((m_start > interval.m_start) || (m_start == interval.m_start && m_end > interval.m_end));
}

bool Interval::operator == (const Interval& interval) const {
	return((m_isEmpty && interval.m_isEmpty) || (m_start == interval.m_start && m_end == interval.m_end));
}

bool Interval::operator <= (const Interval& interval) const {
	return((*this < interval) || (*this == interval));
}

bool Interval::operator >= (const Interval& interval) const {
	return((*this > interval) || (*this == interval));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary operations
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void Interval::intersect(const Interval& interval) {
	if (m_isEmpty || interval.m_isEmpty) {
		setToEmpty();
		return;
	}

	if (m_start < interval.m_start) {
		if (m_end < interval.m_start) {
			setToEmpty();
		}
		else if (m_end <= interval.m_end) {
			m_start = interval.m_start;
		}
		else {
			m_start = interval.m_start;
			m_end = interval.m_end;
		}
	}
	else if (m_start > interval.m_end) {
		setToEmpty();
	}
	else {
		if (m_end > interval.m_end) {
			m_end = interval.m_end;
		}
		else {
			// do nothing
		}
	}
}

Interval Interval::intersectAsNew(const Interval& interval) const {
	Interval result(*this);
	result.intersect(interval);
	return(result);
}

void Interval::intervalUnion(const Interval& interval) {
	if (m_start > interval.m_start) {
		m_start = interval.m_start;
	}

	if (m_end < interval.m_end) {
		m_end = interval.m_end;
	}
}

Interval Interval::intervalUnionAsNew(const Interval& interval) const {
	Interval result(*this);
	result.intervalUnion(interval);
	return(result);
}

bool Interval::intersectsWith(const Interval& interval) const {
	return(!m_isEmpty && !interval.m_isEmpty && !((m_end < interval.m_start) || (m_start > interval.m_end)));
}

bool Interval::isInsideOf(const Interval& enclosingInterval) const {
	return(m_start >= enclosingInterval.m_start && m_end <= enclosingInterval.m_end);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void Interval::coalesceOverlapping(std::vector<Interval >& toCoalesce) {
	for (unsigned int i = 1; i < toCoalesce.size(); i++) {
		if (toCoalesce[i].getStart() <= toCoalesce[i - 1].getEnd()) {
			if (toCoalesce[i].getEnd() > toCoalesce[i - 1].getEnd()) {
				toCoalesce[i - 1].setEnd(toCoalesce[i].getEnd());
			}
			toCoalesce.erase(toCoalesce.begin() + i);
			i--;
		}
	}
}

void Interval::coalesceOverlappingOrAdjacent(std::vector<Interval >& toCoalesce) {
	for (unsigned int i = 1; i < toCoalesce.size(); i++) {
		if (toCoalesce[i].getStart() <= toCoalesce[i - 1].getEnd() + 1) {
			if (toCoalesce[i].getEnd() > toCoalesce[i - 1].getEnd()) {
				toCoalesce[i - 1].setEnd(toCoalesce[i].getEnd());
			}
			toCoalesce.erase(toCoalesce.begin() + i);
			i--;
		}
	}
}

bool Interval::contains(float position) {
	return(m_start <= position && m_end >= position);
}

std::string Interval::toString() const {
	std::ostringstream stringStream;

	if (m_isEmpty) {
		stringStream << "[empty]";
	}
	else {
		stringStream << "[" << m_start << "," << m_end << "]";
	}

	return(stringStream.str());
}

