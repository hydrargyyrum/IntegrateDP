#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "utils.h"
#include "attractor_ui.h"
#define M_PI 3.14

int attractor_show(attractor_show_args args)
{
    if (args.example_to_show)
        cout << "EXAMPLE attractor" << std::endl;
    else
        cout << "Real attractor" << std::endl;

    constexpr double sigma = 10.0;
    constexpr double rho = 28.0;
    constexpr double beta = 8.0/3.0;
    constexpr double dt = 0.01;

    double x = 10.;
    double y = 0.;
    double z = 0.;
    float hu = 0;
    double angle = 0;
    uint64_t frame = 0;

    std::vector<vertex> dots;
    sf::VertexArray attractor(sf::LineStrip);

    constexpr int window_size = 800;
    sf::RenderWindow window(sf::VideoMode(window_size, window_size), "lorenz");
    window.setVerticalSyncEnabled(true);

    sf::RenderTexture texture;
    texture.create(window_size, window_size);

    auto make_dot = [&](){
        vertex dot;
        dot.pos = sf::Vector3f(x, y, z);
        dot.print();
        dot.color = hsv2rgb(hu);
        dots.push_back(dot);
        if (dots.size()>3000)
            dots.erase(dots.begin());
        hu+=.1;
        if (hu > 360)
            hu = 0;
    };

    auto evolve = [&](){
        double dx = sigma * (y - x);
        double dy = x * (rho - z) - y;
        double dz = x * y - beta * z;
        x+=dx*dt;
        y+=dy*dt;
        z+=dz*dt;
        make_dot();
    };

    auto rotate = [&](){
        for (auto & dot : dots)
            dot.rotate(angle);
        angle+=0.005;
    };

    auto draw_attractor= [&](){
        attractor.clear();
        for (auto dot : dots)
            attractor.append(dot);
        texture.draw(attractor);
    };


    auto save_frame= [&](){
        if (angle < 4*M_PI)
        {
            std::ostringstream str;
            str<<"images/"<<std::setfill('0')<<std::setw(5)<<frame<<".png";
            texture.getTexture().copyToImage().saveToFile(str.str());
            ++frame;
        }
    };

    if (args.example_to_show && !args.example_continiously_draw) {
        for (int i = 0; i < 3000; i++)
            evolve();
    }
    if (!args.example_to_show) {
        auto points = args.points;
        for (auto & p : points) {
            vertex dot;
            dot.pos = sf::Vector3f(p[0], p[1], p[2]);
            dot.color = hsv2rgb(hu);
            if (args.print_dots)
                dot.print();
            dots.push_back(dot);
            hu += .1;
            if (hu > 360)
                hu = 0;
            dots.push_back(dot);
        }
    }
    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
        }
        texture.clear();
        if (args.example_to_show && args.example_continiously_draw) {
            evolve();
        }
        rotate();
        draw_attractor();
        // save_frame();
        window.clear();
        sf::Sprite sprite(texture.getTexture());
        window.draw(sprite);
        window.display();
    }

    return 0;
}