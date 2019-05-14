#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "random.hpp"
using Random = effolkronium::random_static;

int count = 0;

double dist(std::vector<sf::Vector2f> const, std::vector<int> const);
uint64_t factorial(int);
bool nextOrder(std::vector<int>&);


int main() {
    // Setting up basic window properties
    int width = 700, height = 900;
    const unsigned fps = 60, depth = 32;

    sf::RenderWindow app;
        app.create(sf::VideoMode(width, height, depth), "Main Window");
        // app.setFramerateLimit(fps);


    // Randomly generated dots that are represent cities
    int totalCities = 12;
    std::vector<int> order;

    std::vector<sf::Vector2f> cities;
    for (int i = 0; i != totalCities; ++i) {
        auto xPos = Random::get(0, width);
        auto yPos = Random::get(0, height/2);
    
        cities.push_back(sf::Vector2f(xPos, yPos));
        order.push_back(i);
    }

    // Tracking te best, smallest distance between two dots
    int recordDist = 0;
    std::vector<int> bestEver;

    double d = dist(cities, order);
    recordDist = d;
    std::copy(order.begin(), order.end(), std::back_inserter(bestEver));
    

    // Graphical representation of cities and connections
    sf::CircleShape city;
        city.setRadius(8);
        city.setOrigin(city.getRadius(), city.getRadius());
        city.setFillColor(sf::Color::Black);

    std::vector<sf::Vector2f> actPath;
    std:copy(cities.begin(), cities.end(), std::back_inserter(actPath));
    for (auto& t : cities) t = sf::Vector2f(t.x, t.y+(height/2));

    sf::VertexArray lines(sf::LinesStrip, totalCities);
    for (int i = 0; i != totalCities; ++i) {
        lines[i].position = actPath[i];
        lines[i].color = sf::Color::Black;
    }

    sf::VertexArray bestLines(sf::LinesStrip, totalCities);
    for (int i = 0; i != totalCities; ++i) {
        int n = bestEver[i];
        bestLines[i].position = cities[n];
        bestLines[i].color = sf::Color::Red;
    }

    // Calculating overall percentage of the process
    double totalPerms = factorial(totalCities);
    double percent = 0;
    
    sf::Font font;
        font.loadFromFile("arial.ttf");
    sf::Text progress;
        progress.setFont(font);
        progress.setCharacterSize(30);
        progress.setPosition(sf::Vector2f(20, 20));
        progress.setFillColor(sf::Color::Black);


    // Main draw loop
    while(app.isOpen()) {
        sf::Event event;
        while(app.pollEvent(event)) {
            if (event.type == sf::Event::Closed) app.close();
        } app.clear(sf::Color(230, 230, 250));


        // Setting up precision for displaying text on the screen from double
        percent = 100*(count/totalPerms);    
            std::ostringstream sob;
            sob << std::fixed;
            sob << std::setprecision(2);
            sob << percent;
        std::string txt = sob.str();


        for (auto c : cities) {
            city.setPosition(c);

            app.draw(city);
            app.draw(bestLines);
            app.draw(progress);
        }

        // Checking all of the possible connections        
        for (int i = 0; i != order.size(); ++i) {
            int n = order[i];
            lines[i].position = actPath[n];
        }
        for (int i = 0; i != order.size(); ++i) { 
            int n = bestEver[i];
            bestLines[i].position = cities[n];
        }

        // Tracking te best, smallest distance between two dots... Again.
        double d = dist(cities, order);
        if (d < recordDist) {
            recordDist = d;
            bestEver.resize(0);  // This is needed for std::copy in the every next iteration
            std::copy(order.begin(), order.end(), std::back_inserter(bestEver));
            std::cout << recordDist << std::endl;
        } 
        
        if (bool isFinished = nextOrder(order); !isFinished && percent < 100.0) {
            progress.setString(txt + '%');
            app.draw(lines);
        } else {
            progress.setString("100%");
            city.setFillColor(sf::Color::Red);
        }

        app.display();
    }

    return 0;
}


double dist(std::vector<sf::Vector2f> const points, std::vector<int> const order) {
    count++;
    double sum = 0;
    for (int i = 0; i != order.size()-1; ++i) {
        auto cityA = points[order[i]];
        auto cityB = points[order[i+1]];

        // There is no need for abs(), to be honest
        double xd = abs(cityA.x - cityB.x);
        double yd = abs(cityA.y - cityB.y);
        // int d = sqrt(xd*xd + yd*yd);
        double d = (0.9604339f*xd + 0.3978247f*yd);  // Crude distance approximation to within 4%  -  in case of optimization

        sum += d;
    }
    return sum;
}

uint64_t factorial(int n) {
    if (n == 1) return 1;
    else return n * factorial(n-1);
}

// Lexicographic order of numbers
bool nextOrder(std::vector<int>& vo) {
    while (std::next_permutation(vo.begin(), vo.end())) {
        return false;
    } return true;
}