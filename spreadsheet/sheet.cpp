#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    ValidatePosition(pos);
    if (table_.count(pos) == 0){
        table_[pos] = std::make_unique<Cell>(text, *this);
    }else{
        table_.at(pos)->Set(std::move(text));
    }
    TryChangePrintableArea(pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet&>(*this).GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    ValidatePosition(pos);
    return (table_.count(pos) != 0) ?  table_.at(pos).get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    ValidatePosition(pos);
    const auto& it = table_.find(pos);
    if (it != table_.end()){
        it->second->Clear();
        table_.erase(it);
        ComputePrintableArea();
    }
}

Size Sheet::GetPrintableSize() const {
    return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    Printer(output, OutMode::VALUE);
}

void Sheet::PrintTexts(std::ostream& output) const {
    Printer(output, OutMode::TEXT);
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

void Sheet::TryChangePrintableArea(Position pos) noexcept{
    printable_size_.rows = (printable_size_.rows > pos.row) ? printable_size_.rows : pos.row + 1;
    printable_size_.cols = (printable_size_.cols > pos.col) ? printable_size_.cols : pos.col + 1;
}

void Sheet::ComputePrintableArea() {
    printable_size_ = {0, 0};
    for (const auto& [position, _] : table_){
        TryChangePrintableArea(position);
    }
}

void Sheet::Printer(std::ostream& output, const OutMode& mode) const{
    bool is_not_first = false;
    for (int i = 0; i < printable_size_.rows; ++i, is_not_first = false){
        for (int j = 0; j < printable_size_.cols; ++j, is_not_first = true){
            Position pos{ i, j };
            const auto& cell = GetCell(pos);

            if (is_not_first) { output << '\t';}
            if (cell){
                if(mode == OutMode::VALUE){
                    std::visit([&output](const auto& obj) { output << obj; }, cell->GetValue());
                }else if(mode == OutMode::TEXT){
                    output << cell->GetText();
                }
            }
        }
        output << '\n';
    }
}