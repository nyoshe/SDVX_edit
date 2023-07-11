#include "bezier.h"

sf::Vector2f interpolateBezier(const std::vector<sf::Vector2f>& points, double t)
{
	std::vector<sf::Vector2f> tempPoints = points;
	while (tempPoints.size() > 1) {
		std::vector<sf::Vector2f> newPoints;
		for (int i = 0; i < tempPoints.size() - 1; i++) {
			sf::Vector2f interpolated;
			interpolated.x = (1 - t) * tempPoints[i].x + t * tempPoints[i + 1].x;
			interpolated.y = (1 - t) * tempPoints[i].y + t * tempPoints[i + 1].y;
			newPoints.push_back(interpolated);
		}
		tempPoints = newPoints;
	}
	return tempPoints[0];
}

std::vector<sf::Vector2f> calculateBezierCurve(const std::vector<sf::Vector2f>& points, int numPoints)
{
	std::vector<sf::Vector2f> curve;
	double stepSize = 1.0 / numPoints;

	for (int i = 0; i <= numPoints; i++) {
		double t = i * stepSize;
		sf::Vector2f interpolated = interpolateBezier(points, t);
		curve.push_back(interpolated);
	}

	return curve;
}
