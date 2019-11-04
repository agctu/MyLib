#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm\glm.hpp>
#include <log.hpp>
logger llog("data.txt");
namespace mine {
	enum rela {
		inter,parted,tang,paral
	};
	struct point {
		float x;
		float y;
		point(float x, float y) :x(x), y(y) {}
		point() {}
		point(vec2 v) :x(v.x), y(v.y) {}
		point(float x) :x(x), y(x) {}
		point operator-()const
		{
			return point(-x, -y);
		}
		operator vec2()
		{
			return vec2(x, y);
		}
		point& operator+=(const point& p)
		{
			x += p.x;
			y += p.y;
			return *this;
		}
		point& operator*=(float k)
		{
			x *= k;
			y *= k;
		}
	};
	point operator+(const point& p1, const point& p2)
	{
		return point(p1.x + p2.x, p1.y + p2.y);
	}
	point operator-(const point& p1, const point& p2)
	{
		return p1 + (-p2);
	}
	point operator*(const point& p, float n)
	{
		return point(p.x*n, p.y*n);
	}
	point operator*(float n, const point& p)
	{
		return p * n;
	}
	
	bool operator==(const point& p1, const point& p2)
	{
		return (p1.x == p2.x&&p1.y == p2.y);
	}
	point operator/(const point& p, float n)
	{
		return point(p.x / n, p.y / n);
	}
	struct line {
		point p1;
		point p2;
		float k;
		float d;
		line(point p1, point p2) :p1(p1), p2(p2) {}
		line(float x1, float y1, float x2, float y2) :p1(x1, y1), p2(x2, y2) {}
		line() {}
	};
	struct rect {
		float top;
		float bottom;
		float left;
		float right;
		rect(float left, float top, float right, float bottom) :left(left), right(right), top(top), bottom(bottom) {}
		rect() {}
	};
	struct circle {
		point center;
		float r;
		circle(float x, float y, float r) :center(x, y), r(r) {}
		circle() {}
	};
	rela relaof(point p, rect r)
	{
		if (p.x > r.right || p.x < r.left)
			return parted;
		if (p.y > r.top || p.y < r.bottom)
			return parted;
		if (p.x<r.right&&p.x>r.left&&p.y > r.bottom&&p.y < r.top)
			return inter;
		return tang;
	}
	rela relaof(line l1, line l2)
	{
		float a = 0, b = 0;
		point pa = l1.p1 - l1.p2;
		point pb = l2.p1 - l2.p2;
		point pc = l2.p2 - l1.p2;
		//检查是否平行
		if (pa.x == 0&&pb.x==0) {
			return paral;
		}
		else {
			float k = pb.x / pa.x;
			if (pb.y == k * pa.y)
				return paral;
		}

		a = -(pb.x*pc.y - pb.y*pc.x) / (pb.y*pa.x - pb.x*pa.y);
		b = (pa.x*pc.y - pa.y*pc.x) / (pa.y*pb.x - pa.x*pb.y);
		/*if (a < 1)
			llog << "a: " << a << " b: " << b << endl;
		cout <<"a: "<< a << " b: " << b << endl;*/
		if (a < 1 && b < 1 && a>0 && b>0)
			return inter;
		if (a > 1 || a < 0 || b>1 || b < 0)
			return parted;
		return tang;
	}
	rela relaof(line l, rect r)
	{
		//the function is not completed
		if (relaof(l.p1, r) == inter || relaof(l.p2, r) == inter)
			return inter;
		if (relaof(l, line(r.left, r.top, r.right, r.top)) == inter
			|| relaof(l, line(r.left, r.top, r.left, r.bottom)) == inter
			|| relaof(l, line(r.right, r.top, r.right, r.bottom)) == inter
			|| relaof(l, line(r.right, r.bottom, r.left, r.bottom)) == inter)
			return inter;
		return parted;
	}
}
#endif