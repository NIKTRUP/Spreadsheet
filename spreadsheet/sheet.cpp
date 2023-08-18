#include "sheet.h"

using namespace std::literals;

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    ValidatePosition(pos);

    auto cell = std::make_unique<Cell>(text, *this);
    CheckCyclic(cell.get(), pos);
    UpdateDependencies(cell.get(), pos);

    table_[pos] = std::move(cell);
    TryChangePrintableArea(pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet&>(*this).GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    ValidatePosition(pos);
    return (table_.count(pos) != 0) ? table_.at(pos).get() : nullptr;
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

bool Sheet::IsCycle(Position position, const std::set<Position>& references, std::set<Position>& verified) const {
    if (references.count(position)){ return true; }

    for (Position pos : references){
        if (pos.IsValid() && !verified.count(pos)){
            verified.insert(pos);
            auto cell = GetCell(pos);
            if (cell) {
                auto ref = cell->GetReferencedCells();
                if (IsCycle(position, {ref.begin(), ref.end()}, verified)){ return true; }
            }
        }
    }

    return false;
}

void Sheet::CheckCyclic(const Cell* cell, Position position) {
    auto positions = cell->GetReferencedCells();
    std::set<Position> referenced(positions.begin(), positions.end()),
            verified;
    if(IsCycle(position, referenced, verified)){
        throw CircularDependencyException("Cycle detected");
    }
}

void Sheet::UpdateDependencies(Cell* cell, Position pos){
    for (const auto& pos_ref : cell->GetReferencedCells()){
        CellInterface *referenced_cell = GetCell(pos_ref);
        if (!referenced_cell){
            table_[pos_ref] = std::make_unique<Cell>(*this);
            referenced_cell = table_.at(pos_ref).get();
        }
        referenced_cell->PushBackDependentCell(pos);
    }
}