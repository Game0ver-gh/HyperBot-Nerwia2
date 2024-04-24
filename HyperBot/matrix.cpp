#include "pch.h"
#include "matrix.h"

#include <iostream>
#include <iomanip>      // std::setprecision

using namespace std;

namespace mathing
{
static Scalar identity[16] = {1, 0, 0, 0,
							  0, 1, 0, 0,
							  0, 0, 1, 0,
							  0, 0, 0, 1};
const MatrixCppImpl4x4 MatrixCppImpl4x4::m_Identity(identity);

ostream &operator<<(ostream &os, const MatrixCppImpl4x4 &m)
{
	os << std::fixed << std::setprecision(2);
    os << "[" << std::setw(5) << m.AxisX().x << ", " << std::setw(5) << m.AxisX().y << ", " << std::setw(5) << m.AxisX().z << ", " << std::setw(5) << m.AxisX().w << "]\n";
    os << "[" << std::setw(5) << m.AxisY().x << ", " << std::setw(5) << m.AxisY().y << ", " << std::setw(5) << m.AxisY().z << ", " << std::setw(5) << m.AxisY().w << "]\n";
    os << "[" << std::setw(5) << m.AxisZ().x << ", " << std::setw(5) << m.AxisZ().y << ", " << std::setw(5) << m.AxisZ().z << ", " << std::setw(5) << m.AxisZ().w << "]\n";
    os << "[" << std::setw(5) << m.Pos().x << ", "   << std::setw(5) << m.Pos().y << ", "   << std::setw(5) << m.Pos().z << ", "   << std::setw(5) << m.Pos().w << "]";
	return os;
}

ostream &operator<<(ostream &os, const Matrix &m) {
	return os << m._impl;
}


}  // namespace mathing
