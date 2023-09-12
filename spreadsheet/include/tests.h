#ifndef SPREADSHEET__TESTS_H
#define SPREADSHEET__TESTS_H
#include <limits>
#include "common.h"
#include "formula.h"
#include "test_runner_p.h"

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
    return output << "(" << size.rows << ", " << size.cols << ")";
}

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit(
        [&](const auto& x) {
          output << x;
        },
        value);
    return output;
}


namespace tests {
    void TestPositionAndStringConversion();

    void TestPositionToStringInvalid();

    void TestStringToPositionInvalid();

    void TestEmpty();

    void TestInvalidPosition();

    void TestSetCellPlainText();

    void TestClearCell();

    void TestFormulaArithmetic();

    void TestFormulaReferences();

    void TestFormulaExpressionFormatting();

    void TestFormulaReferencedCells();

    void TestErrorValue();

    void TestErrorDiv0();

    void TestEmptyCellTreatedAsZero();

    void TestFormulaInvalidPosition();

    void TestPrint();

    void TestCellReferences();

    void TestFormulaIncorrect();

    void TestCellCircularReferences();

    void TestSpreadSheer();
}

#endif //SPREADSHEET__TESTS_H
