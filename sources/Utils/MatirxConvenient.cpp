
#include "MatirxConvenient.h"

void swap(VectorDynamic &x, LLint i, LLint j)
{
    double t = x(i);
    x(i) = x(j);
    x(j) = t;
}

MatrixDynamic GenerateOneMatrix(int m, int n)
{
    MatrixDynamic M;
    M.resize(m, n);
    M.setZero();
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            M(i, j) = 1;
    }
    return M;
}

// template <class T>
// std::vector<T> Eigen2Vector(const VectorDynamic &input)
// {
//     std::vector<T> res;
//     LLint len = input.rows();
//     res.reserve(len);
//     for (LLint i = 0; i < len; i++)
//         res.push_back(input.coeff(i, 0));
//     return res;
// }

template <class T>
VectorDynamic Vector2Eigen(const std::vector<T> &input)
{

    LLint len = input.size();
    VectorDynamic res;
    res.resize(len, 1);
    for (LLint i = 0; i < len; i++)
        res(i, 0) = input.at(i);
    return res;
}