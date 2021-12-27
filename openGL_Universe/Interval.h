// File: Interval.h
// Author: Lucas Kovar

#ifndef INTERVAL
#define INTERVAL


#include <string>
#include <Vector>


class Interval
{
public:

	Interval();

	Interval(float start, float end);
	Interval(const Interval& interval);

	float getStart() const;
	float getEnd() const;


	void setStart(float start);
	void setEnd(float end);
	void set(float start, float end);

	Interval& operator = (const Interval& interval);
	void copy(Interval interval);
	bool isEmpty() const;
	void setToEmpty();
	float getSize() const;
	float getNumElems() const;

	bool operator < (const Interval& interval) const;
	bool operator <= (const Interval& interval) const;
	bool operator > (const Interval& interval) const;
	bool operator >= (const Interval& interval) const;
	bool operator == (const Interval& interval) const;

	void intersect(const Interval& interval);
	Interval intersectAsNew(const Interval& interval) const;
	void intervalUnion(const Interval& interval);
	Interval intervalUnionAsNew(const Interval& interval) const;
	bool isInsideOf(const Interval& enclosingInterval) const;
	bool intersectsWith(const Interval& interval) const;
	bool contains(float position);
	void coalesceOverlapping(std::vector< Interval >& toCoalesce);
	void coalesceOverlappingOrAdjacent(std::vector<Interval >& toCoalesce);
	std::string toString() const;



private:
	char m_isEmpty;

	float m_start;
	float m_end;
};

#endif