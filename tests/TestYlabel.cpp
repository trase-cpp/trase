#include "trase.hpp"
#include <iostream>
#include <fstream>

using namespace trase;

void test_1() {
    std::string dest = "test_demo_1.svg";

    std::vector<double> predicted_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };
    std::vector<double> exp_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };

    auto fig = figure();
    auto ax = fig->axis();
    int predicted_size = predicted_data.size();
    int exp_size = exp_data.size();

    std::vector<float> x(predicted_size);
    std::vector<float> y(exp_size);
    for (int i = 0; i < predicted_size; ++i) {
        x[i] = predicted_data[i];
    }
    for (int i = 0; i < exp_size; ++i) {
        y[i] = exp_data[i];
    }

    auto data = create_data().x(x).y(y);
    auto plt = ax->points(data);

    ax->xlabel("Pred. Target");
    ax->ylabel("Exp. Target");

    // output to svg
    std::ofstream out;
    out.open(dest);
    BackendSVG backend(out);
    fig->draw(backend);
    out.close();
}

void test_2() {
    std::string dest = "test_demo_2.svg";

    std::vector<double> predicted_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };
    std::vector<double> exp_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };

    auto fig = figure();
    auto ax = fig->axis();
    int predicted_size = predicted_data.size();
    int exp_size = exp_data.size();

    std::vector<float> x(predicted_size);
    std::vector<float> y(exp_size);
    for (int i = 0; i < predicted_size; ++i) {
        x[i] = predicted_data[i] * 10.0;
    }
    for (int i = 0; i < exp_size; ++i) {
        y[i] = exp_data[i] * 10.0;
    }

    auto data = create_data().x(x).y(y);
    auto plt = ax->points(data);

    ax->xlabel("Pred. Target");
    ax->ylabel("Exp. Target");

    // output to svg
    std::ofstream out;
    out.open(dest);
    BackendSVG backend(out);
    fig->draw(backend);
    out.close();
}

int main() {
    
    test_1();
    test_2();

    return 0;
}