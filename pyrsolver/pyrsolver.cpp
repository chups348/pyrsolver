/*
* project:  pyrsolver.cpp
* author:   Sara Chupa
* date:     April 28, 2024
* version:  1.0
* brief:    Using three known attributes, this program calculates all unknown angles and side lengths for a triangle.
			In addition, it calculates the perimeter and area of the triangle. In this manner, a triangle can then be
			classified according to the six determined attributes.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <numbers>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;
using namespace std::numbers;

// Declare and instantiate Pi
constexpr double PI_VAL = std::numbers::pi;

// Function to print a vector of points
void printVector(const vector<pair<double, double>>& points) {
	for (const auto& point : points) {
		cout << "(" << point.first << ", " << point.second << ") ";
	}
	cout << endl;
}


/*
* fn:       getSideType
* brief:    Classifies the triangle by its side lengths
* param:    sideA - Length of side a
*           sideB - Length of side b
*           sideC - Length of side c
* return:   Type string
*/
string getSideType(const vector<double>& v)
{
	if (v[0] == v[1] && v[1] == v[2])
	{
		return "equilateral";
	}
	else if (v[0] == v[1] || v[1] == v[2] || v[0] == v[2])
	{
		return "isosceles";
	}
	else
	{
		return "scalene";
	}
}


struct Options {
	bool isTriangle = false;
	bool isSquare = false;
	bool isRectangle = false;
	bool isPoly = false;
};

struct Point {
	double x, y;
	constexpr Point(double _x, double _y) : x(_x), y(_y) {}
};


// Function to calculate the area of a triangle
double calculateTriangleArea(const vector<double>& sides) {
	double perimeter = sides[0] + sides[1] + sides[2];
	double s = perimeter / 2;
	return sqrt(s * (s - sides[0]) * (s - sides[1]) * (s - sides[2]));
}

// Function to calculate the area of a regular polygon
double calculatePolygonArea(int numSides, double sideLength) {
	return 0.25 * numSides * pow(sideLength, 2) * (1 / tan(PI_VAL / numSides));
}

//slant heights
double calculateSlantHeight(Options& options, const vector<double>& v, int numSides, double height) {
	double slantHeight, baseHeight;
	if (options.isTriangle) {

		baseHeight = (2.0 * calculateTriangleArea(v)) / v[0];
		baseHeight /= 2.0;

	}
	else if (options.isSquare) {
		baseHeight = v[0] / 2.0;
	}
	else if (options.isRectangle) {
		baseHeight = (v[0] / 2.0) + (v[1] / 2.0);
	}
	else {
		// calculate the apothem
		double apothem = (v[0] / (2 * tan(PI_VAL / numSides)));

		// calculate slant height
		slantHeight = sqrt(pow(height, 2) + pow(apothem, 2));
		return slantHeight;
	}

	// calculate slant height for regular polygons
	slantHeight = sqrt(pow(height, 2) + pow(baseHeight, 2));
	return slantHeight;
}

// Function to calculate the perimeter of a shape based on its type
double calculatePerimeter(Options& options, const vector<double>& sides, int numSides) {
	double perimeter;
	if (options.isTriangle) {
		perimeter = sides[0] + sides[1] + sides[2];
	}
	else if (options.isSquare) {
		perimeter = 4 * sides[0];
	}
	else if (options.isRectangle) {
		perimeter = 2 * (sides[0] + sides[1]);
	}
	else {
		perimeter = numSides * sides[0];
	}
	return perimeter;
}

// Function to calculate the vertices of a regular polygon
vector<pair<double, double>> calculatePolygonVertices(int numSides, double sideLength, pair<double, double> baseCenter) {
	vector<pair<double, double>> vertices;
	double angle_increment = 2 * PI_VAL / numSides;

	for (unsigned i = 0; i < numSides; ++i) {
		double angle = i * angle_increment;
		double x = baseCenter.first + sideLength * cos(angle);
		double y = baseCenter.second + sideLength * sin(angle);
		vertices.push_back(make_pair(x, y));
	}

	return vertices;
}

// Function to calculate the vertices of an equilateral triangle
vector<pair<double, double>> calculateTriangleVertices(const vector<double>& sides, pair<double, double> baseCenter) {
	vector<pair<double, double>> vertices;

	// Calculate the height of an equilateral triangle
	double height = sides[0] * sqrt(3.0) / 2.0;

	// Calculate coordinates for each vertex
	vertices.push_back(make_pair(baseCenter.first, baseCenter.second + height / 3.0)); // Top vertex
	vertices.push_back(make_pair(baseCenter.first - sides[0] / 2.0, baseCenter.second - height / 3.0)); // Bottom left vertex
	vertices.push_back(make_pair(baseCenter.first + sides[0] / 2.0, baseCenter.second - height / 3.0)); // Bottom right vertex

	return vertices;
}

// Function to calculate the centroid of a regular polygon
vector<pair<double, double>> calculatePolygonCentroid(int numSides, const vector<double>& sides) {
	vector<pair<double, double>> vertices = calculatePolygonVertices(numSides, sides[0], make_pair(0.0, 0.0));

	double sumX = 0, sumY = 0;
	for (const auto& vertex : vertices) {
		sumX += vertex.first;
		sumY += vertex.second;
	}

	double centroidX = sumX / numSides;
	double centroidY = sumY / numSides;

	return { make_pair(centroidX, centroidY) };
}

// Function to classify the shape based on the number of sides
string determineBaseShape(const int numSides, Options& options, const vector<double>& v) {
	string baseShape;
	switch (numSides) {
	case 3:
		baseShape = "Triangle";
		options.isTriangle = true;
		break;
	case 4:
		if (v.size() == 2 && v[0] == v[1]) {
			baseShape = "Square";
			options.isSquare = true;
		}
		else {
			baseShape = "Rectangle";
			options.isRectangle = true;
		}
		break;
	default:
		baseShape = "Polygon with " + to_string(numSides) + " sides";
		options.isPoly = true;
		break;
	}

	return baseShape;
}

// Main function to solve pyramid
void solvePyramid(Options& options, const vector<double>& sides, int numSides, double height) {
	double baseArea, volume, surfaceArea = 0;
	double perimeter = calculatePerimeter(options, sides, numSides);
	vector<pair<double, double>> centroid;

	if (options.isTriangle) {
		baseArea = calculateTriangleArea(sides);
	}
	else {
		baseArea = calculatePolygonArea(numSides, sides[0]);
	}

	centroid = calculatePolygonCentroid(numSides, sides);

	volume = (1.0 / 3.0) * baseArea * height;
	surfaceArea = baseArea + (0.5 * perimeter * height);

	string shape = determineBaseShape(numSides, options, sides);

	cout << fixed << setw(15) << left << "\n\nBase Shape: " << shape << endl;
	cout << "Base Area: " << baseArea << endl;
	cout << "\nVolume: " << volume << endl;
	cout << "Surface Area: " << surfaceArea << endl;
	cout << "\nHeight: " << height << endl;

	cout << "Centroid: ";
	printVector(centroid);
}




/*
*       DO WHILE "continue loop" condition true
*           Format string is converted to upper case
*           IF the user enters 'quit' or 'exit' or 'bye' in any case
*               UNDO
*           ENDIF
*           Validate that the user entered an attribute string followed by three real numbers all on the same line
*           IF invalid input entered THEN
*               CALL function to print error message and continue to next iteration
*           ELSE
*               CALL function to convert negative inputs to positive
*               CALL triangle type functions based on the user entered format
*                   Functions compute unknown sides and angles
*                       CALL function to print results
*           ENDIF
*       ENDDO
*/
int main(int argc, char* argv[])
{
	Options options;

	cout << "Pyramidsolver 1.1.0 (c) 2024-4, Sara Chupa" << endl;
	deque<string> args;

	for (int i{ 1 }; i < argc; i++)
		args.push_back(argv[i]);

	if (args.size() < 3) {
		cerr << "Insufficient number of arguments provided" << endl;
		return 1;
	}

	// extract number of sides
	unsigned numSides = stoi(args.front());
	args.pop_front();

	// extract the height
	double height = stod(args.back());
	args.pop_back();
	// Add debug output to check the input string
	cout << "Input string for base dimensions: " << args.front() << endl;

	// extract base dimensions
	vector<double> baseDimensions;
	bool validInput = false;
	while (!validInput) {
		baseDimensions.clear(); // Clear any previous input
		string inputDimensions;
		cout << "Enter the base dimensions separated by spaces: ";
		getline(cin, inputDimensions);

		istringstream iss(inputDimensions);
		double dimension;
		while (iss >> dimension) {
			baseDimensions.push_back(dimension);
		}

		if (baseDimensions.size() != numSides) {
			cout << "Number of dimensions provided does not match the expected number of sides for the shape. Please try again.\n";
		}
		else {
			validInput = true;
		}
	}

	// Solve pyramid
	solvePyramid(options, baseDimensions, numSides, height);

} 



