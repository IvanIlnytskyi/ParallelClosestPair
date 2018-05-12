#pragma once
#include <math.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <thread>
#include <iostream>
#include <random>

struct Point
{
	float x;
	float y;
	bool operator<(const Point& p) const 
	{
		return x < p.x;
	}
};

float points_distance(const Point & a, const Point & b)
{
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

float boundary_merge(const std::vector<Point> & points, const float left_distance, const float right_distance, const float median)
{
	float min_dist = fmin(left_distance, right_distance);
	std::vector<Point> M(points.size());
	std::vector<Point>::iterator iter = std::copy_if(points.begin(), points.end(), M.begin(), [&](Point p) {return p.x >= median - min_dist && p.x <= median + min_dist; });
	M.resize(std::distance(M.begin(), iter));
	for (size_t i = 0; i+7 < M.size(); ++i)
	{
		for (size_t j = 1; j < 8; ++j)
		{
			if (min_dist > points_distance(M[i], M[i + j]))
				min_dist = points_distance(M[i], M[i + j]);
		}
	}
	return min_dist;
}
	
std::vector<Point> parallel_closest_pair(const std::vector<Point> &, float &);

void closest_pair_invoker(const std::vector<Point> & P, float & dist, std::vector<Point> & P1)
{
	P1 = parallel_closest_pair(P, dist);
}


std::vector<Point> parallel_closest_pair(const std::vector<Point> & P, float & dist)
{
	if (P.size() < 2)
	{
		dist = FLT_MAX/10;
		return P;
	}
	else
	{

		float median = P[P.size() / 2].x;

		std::vector<Point> L(P.begin(), P.begin() + P.size() / 2 );
		std::vector<Point> R(P.begin() + P.size() / 2, P.end());

		std::vector<Point> L1;
		std::vector<Point> R1;

		float L_dist, R_dist;
		
		//closest_pair_invoker(std::ref(L), std::ref(L_dist), std::ref(L1));
		//closest_pair_invoker(std::ref(R), std::ref(R_dist), std::ref(R1));

		
		std::thread t1(closest_pair_invoker, std::ref(L), std::ref(L_dist), std::ref(L1));
		std::thread t2(closest_pair_invoker, std::ref(R), std::ref(R_dist), std::ref(R1));
		t1.join();
		t2.join();
	
		std::vector<Point> mergedPoints(L1.size() + R1.size());
		std::merge(L1.begin(), L1.end(), R1.begin(), R1.end(), mergedPoints.begin(), [](Point left, Point right) {return left.y < right.y; });
		dist = boundary_merge(mergedPoints, L_dist, R_dist, median);
		
		return mergedPoints;
	}
}


float sequential_closest_pair(const std::vector<Point> & points)
{
	float min = FLT_MAX;
	for (int i = 0; i < points.size(); ++i)
		for (int j = i + 1; j < points.size(); ++j)
		{
			float temp_dist = points_distance(points[i], points[j]);
			if (temp_dist < min)
				min = temp_dist;
		}
	return min;
}

std::vector<Point> create_test_vector(const size_t size)
{
	std::vector<Point> result_vec(size);
	srand(static_cast <unsigned> (time(0)));
	for (int i = 0; i < size; ++i)
	{
		float x = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) *1000.0f;
		float y = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) * 1000.0f;
		result_vec[i] = { x,y };
	}
	std::sort(result_vec.begin(), result_vec.end());
	return result_vec;
}