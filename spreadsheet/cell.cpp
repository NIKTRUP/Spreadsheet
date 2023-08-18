#include "cell.h"
#include <string>
#include <optional>

Cell::Cell(std::string text, SheetInterface& sheet):
        sheet_(sheet),
        impl_(InitCell(text)),
        referenced_cells_(impl_->GetReferencedCells()){

}

Cell::Cell(SheetInterface& sheet):
        sheet_(sheet),
        impl_(std::make_unique<EmptyImpl>()){

}

void Cell::ClearCache(){
    cash_.reset();
    for (auto ref_pos : dependent_cells_){ sheet_.GetCell(ref_pos)->ClearCache();}
}

std::unique_ptr<Cell::Impl> Cell::InitCell(std::string& text){
    std::unique_ptr<Impl> impl;
    if(text.size() > 1 && text[0] == FORMULA_SIGN){
        impl = std::make_unique<FormulaImpl>(std::move(text), sheet_);
    }else if(text.empty()){
        impl = std::make_unique<EmptyImpl>();
    }else {
        impl = std::make_unique<TextImpl>(std::move(text));
    }
    return impl;
}

void Cell::Set(std::string text) {
    impl_ = InitCell(text);
    referenced_cells_ = impl_->GetReferencedCells();
    ClearCache();
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    if (!cash_.has_value()){ cash_ = impl_->GetValue(); }
    return cash_.value();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return referenced_cells_;
}

std::vector<Position> Cell::GetDependentCells() const {
    return dependent_cells_;
}

void Cell::PushBackDependentCell(Position pos){
    if (pos.IsValid()){ dependent_cells_.push_back(pos); }
}

Cell::EmptyImpl::EmptyImpl() {
    value_ = ""; text_ = "";
}

std::string Cell::EmptyImpl::GetText() const {
    return text_;
}

Cell::Value Cell::EmptyImpl::GetValue() const {
    return value_;
}

std::vector<Position> Cell::EmptyImpl::GetReferencedCells() const{
    return {};
}

Cell::TextImpl::TextImpl(std::string_view text) {
    text_ = text;
    value_ = (text[0] == ESCAPE_SIGN) ? std::string{text.substr(1)} : std::string{text};
}

std::string  Cell::TextImpl::GetText() const {
    return text_;
}

Cell::Value Cell::TextImpl::GetValue() const {
    return value_;
}

std::vector<Position> Cell::TextImpl::GetReferencedCells() const{
    return {};
}

Cell::FormulaImpl::FormulaImpl(std::string_view expression, const SheetInterface& sheet):
    sheet_(sheet),
    formula_(ParseFormula(std::string{expression.substr(1)})){
    text_ = FORMULA_SIGN + formula_->GetExpression();
}

std::string  Cell::FormulaImpl::GetText() const {
    return text_;
}

Cell::Value Cell::FormulaImpl::GetValue() const {
    auto value = formula_->Evaluate(sheet_);
    if(std::holds_alternative<FormulaError>(value)){
        return std::get<FormulaError>(value);
    }
    return std::get<double>(value);
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const{
    return formula_->GetReferencedCells();
}