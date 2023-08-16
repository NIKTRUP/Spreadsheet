#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
    class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

using CellTranslator = std::function<double(Position position)>;

class FormulaAST {
public:
    FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr, std::forward_list<Position> cells_position);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(const CellTranslator& function) const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

    const std::forward_list<Position>& GetCellPositions() const;
    std::forward_list<Position>& GetCells();

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::forward_list<Position> cell_positions_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);