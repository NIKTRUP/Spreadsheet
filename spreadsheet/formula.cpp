#include "formula.h"
#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression): ast_(ParseFormulaAST(expression)) {}

        Value Evaluate(const SheetInterface& sheet) const override{
            // TODO::
//            try {
//                return ast_.Execute();
//            }catch (const FormulaError& exp){
//                return exp;
//            }
        }
        std::string GetExpression() const override{
            std::stringstream ss;
            ast_.PrintFormula(ss);
            return ss.str();
        }

        std::vector<Position> GetReferencedCells() const{
            const auto& cell_positions = ast_.GetCellPositions();
            std::set<Position> set_positions(cell_positions.begin(), cell_positions.end());
            return {set_positions.begin(), set_positions.end()};
        }

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try{
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...){
        throw FormulaException("Formula creation error");
    }
}