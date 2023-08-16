#pragma once

#include "cell.h"
#include "common.h"
#include <functional>
#include <tuple>

class Sheet : public SheetInterface {
    enum class OutMode{ TEXT, VALUE };

    struct Hasher{
        size_t operator()(Position pos) const noexcept{
            return hasher_(pos.row)*Position::MAX_COLS + hasher_(pos.col);
        }
    private:
        std::hash<int> hasher_;
    };

    using Table = std::unordered_map<Position, std::unique_ptr<Cell>, Hasher>;
public:


    ~Sheet() override;

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;
private:
    void TryChangePrintableArea(Position pos) noexcept;

    void ComputePrintableArea();

    void Printer(std::ostream& output, const OutMode& mode) const;

    // Можете дополнить ваш класс нужными полями и методами
private:
    Table table_;
    Size printable_size_;
    // Можете дополнить ваш класс нужными полями и методами
};

inline void ValidatePosition(Position pos){
    if(!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
}