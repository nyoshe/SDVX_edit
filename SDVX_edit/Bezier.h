#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>


std::vector<sf::Vector2f> calculateBezierCurve(const std::vector<sf::Vector2f>& points, int numPoints);
sf::Vector2f interpolateBezier(const std::vector<sf::Vector2f>& points, int numPoints);