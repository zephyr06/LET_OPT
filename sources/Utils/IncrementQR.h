#pragma once
#include "sources/Factors/JacobianAnalyze.h"

namespace OrderOptDAG_SPACE
{

    struct QR
    {
        MatrixDynamic Q;
        MatrixDynamic R;
        void print()
        {
            std::cout << "Q" << std::endl;
            std::cout << Q << std::endl;
            std::cout << "R" << std::endl;
            std::cout << R << std::endl;
        }
    };
    QR GetEigenQR(const MatrixDynamic &A);
} // namespace OrderOptDAG_SPACE