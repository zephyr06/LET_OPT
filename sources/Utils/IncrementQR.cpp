#include "sources/Utils/IncrementQR.h"

namespace OrderOptDAG_SPACE
{

    QR GetEigenQR(const MatrixDynamic &A)
    {
        Eigen::MatrixXd Q(A.rows(), A.rows());
        Eigen::MatrixXd R(A.rows(), A.cols());

        /////////////////////////////////HouseholderQR////////////////////////
        Eigen::MatrixXd thinQ(A.rows(), A.cols()), q(A.rows(), A.rows());

        Eigen::HouseholderQR<Eigen::MatrixXd> householderQR(A);
        q = householderQR.householderQ();
        thinQ.setIdentity();
        Q = householderQR.householderQ() * thinQ;

        std::cout << "HouseholderQR" << std::endl;

        R = householderQR.matrixQR().template triangularView<Eigen::Upper>();
        return QR{Q, R};
    }
} // namespace OrderOptDAG_SPACE