#include "../include/formula.h"
#include "../include/FormulaAST.h"

#include <algorithm>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError _) {
    return output << "#DIV/0!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression): ast_(ParseFormulaAST(expression)) {}

        [[nodiscard]] Value Evaluate(const SheetInterface& sheet) const override{
            try{
                return ast_.Execute(GetCellTranslator(sheet));
            }catch (const FormulaError& error){
                return error;
            }
        }

        [[nodiscard]] std::string GetExpression() const override{
            std::stringstream ss;
            ast_.PrintFormula(ss);
            return ss.str();
        }

        [[nodiscard]] std::vector<Position> GetReferencedCells() const override{
            const auto& cell_positions = ast_.GetCells();
            std::set<Position> set_positions(cell_positions.begin(), cell_positions.end());
            return {set_positions.begin(), set_positions.end()};
        }

    private:
        [[nodiscard]] static CellTranslator GetCellTranslator(const SheetInterface& sheet) {
            return [&sheet](Position pos){
                if (!pos.IsValid()){ throw FormulaError(FormulaError::Category::Ref);}

                const auto cell = sheet.GetCell(pos);
                if (!cell){ return 0.0; }

                const auto value = cell->GetValue();
                if (std::holds_alternative<double>(value)){
                    return std::get<double>(value);
                }else if(std::holds_alternative<std::string>(value)){
                    std::string text = cell->GetText();
                    if (text[0] == ESCAPE_SIGN){ throw FormulaError(FormulaError::Category::Value); }
                    if (text.empty()){ return 0.0; }

                    try {
                        return std::stod(text);
                    } catch (...) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                }else if(std::holds_alternative<FormulaError>(value)){
                    throw std::get<FormulaError>(value);
                }
                return 0.0;
            };
        }

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } catch (...) {
        throw FormulaException("Parsing Error");
    }
}