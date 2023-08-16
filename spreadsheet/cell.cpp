#include "cell.h"
#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(std::string text, SheetInterface& sheet): impl_(InitCell(text, sheet)), sheet_(sheet) {}

Cell::Cell(SheetInterface& sheet):
    impl_(std::make_unique<EmptyImpl>()),
    sheet_(sheet) {}

Cell::~Cell() = default;

std::unique_ptr<Cell::Impl> Cell::InitCell(std::string& text, const SheetInterface& sheet){
    std::unique_ptr<Impl> impl;
    if(text.size() > 1 && text[0] == FORMULA_SIGN){
        impl = std::make_unique<FormulaImpl>(std::move(text), sheet);
    }else if(text.empty()){
        impl = std::make_unique<EmptyImpl>();
    }else {
        impl = std::make_unique<TextImpl>(std::move(text));
    }
    return impl;
}

void Cell::Set(std::string text) {
    impl_ = InitCell(text, sheet_);
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return std::vector<Position>{};
}

Cell::EmptyImpl::EmptyImpl() { value_ = ""; text_ = ""; }

std::string Cell::EmptyImpl::GetText() const { return text_; }

Cell::Value Cell::EmptyImpl::GetValue() const { return value_; }


Cell::TextImpl::TextImpl(std::string_view text) {
    text_ = text;
    value_ = (text[0] == ESCAPE_SIGN) ? std::string{text.substr(1)} : std::string{text};
}

std::string  Cell::TextImpl::GetText() const { return text_; }

Cell::Value Cell::TextImpl::GetValue() const { return value_; }


Cell::FormulaImpl::FormulaImpl(std::string_view expression, const SheetInterface& sheet): sheet_(sheet){
    auto formula = std::string{expression.substr(1)};
    value_ = formula;
    formula_ = ParseFormula(std::move(formula));
    text_ = FORMULA_SIGN + formula_->GetExpression();
}

std::string  Cell::FormulaImpl::GetText() const { return text_; }

Cell::Value Cell::FormulaImpl::GetValue() const {
    // TODO ::
    auto value = formula_->Evaluate(sheet_);
    if(std::holds_alternative<FormulaError>(value)) {
        return std::get<FormulaError>(value);
    }
    return std::get<double>(value);
}