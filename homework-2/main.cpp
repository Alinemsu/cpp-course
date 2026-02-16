#include <iostream>
#include "matrix.hpp"

int main()
{
    Matrix<int, 0> matrix;

    for (int i = 0; i < 10; ++i)
        matrix[i][i] = i;

    for (int i = 0; i < 10; ++i) {
        matrix[i][9 - i] = 9 - i;
    }

    std::cout << "Fragment [1,1]..[8,8]: " << std::endl;
    for (int i = 1; i < 9; ++i) {
        for (int j = 1; j < 9; ++j) {
            std::cout << (int)matrix[i][j];
            if (j != 8)
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "cells with value: " << matrix.size() << std::endl;

    std::cout << "the positions of cells with value: " << matrix.size() << std::endl;
    for (auto c : matrix) {
        int i, j, v;
        std::tie(i, j, v) = c;
        std::cout << i << " " << j << " " << v << std::endl;
    }

    {
        Matrix<int, 0, 2> matrix2;

        ((matrix2[100][100] = 314) = 0) = 217;

        std::cout << (int)matrix2[100][100] << std::endl;
        std::cout << matrix2.size() << std::endl;
    }

    Matrix<int, 0, 3> cube;

    for (int i = 0; i < 10; ++i) {
        cube[i][i][i] = i + 1;
    }

    for (int i = 0; i < 10; ++i) {
        cube[i][i][9 - i] = 100 + i;
    }

    std::cout << "3D checks:" << std::endl;
    std::cout << "(0,0,0) = " << (int)cube[0][0][0] << std::endl;
    std::cout << "(0,0,9) = " << (int)cube[0][0][9] << std::endl;
    std::cout << "(1,2,3) = " << (int)cube[1][2][3] << std::endl;

    std::cout << "3D cells with value: " << cube.size() << std::endl;

    for (auto c : cube) {
        Matrix<int, 0, 3>::Key key;
        int value;
        std::tie(key, value) = c;

        std::cout << key[0] << " " << key[1] << " " << key[2]
            << " " << value << std::endl;
    }

    {
        Matrix<int, 0, 3> cube2;
        ((cube2[1][2][3] = 314) = 0) = 217;
        std::cout << "3D chain value: " << (int)cube2[1][2][3] << std::endl;
        std::cout << "3D chain size: " << cube2.size() << std::endl;
    }
    return 0;
}