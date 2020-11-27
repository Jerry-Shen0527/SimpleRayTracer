#pragma once
#include <functional>
#include <Geometry/Vector3.h>
void ParallelFor2D(std::function<void(Point2i)> func, const Point2i& count);
