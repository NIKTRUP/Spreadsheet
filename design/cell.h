#pragma once

#include "common.h"
#include "formula.h"
#include <optional>

class Cell : public CellInterface {
public:

    Cell(std::string text, SheetInterface& sheet);
    explicit Cell(SheetInterface& sheet);
    ~Cell() override = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;
    std::vector<Position> GetDependentCells() const;

    void PushBackDependentCell(Position pos) override;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    void ClearCache() override;
    std::unique_ptr<Impl> InitCell(std::string& text);


private:
    SheetInterface& sheet_; // Ссылка на таблицу
    std::unique_ptr<Impl> impl_;

    mutable std::optional<Value> cash_; // Кэш
    std::vector<Position> referenced_cells_; // Ячейки, от которых зависит ячейка
    std::vector<Position> dependent_cells_; // Ячейки, которые зависят от ячейки
};

class Cell::Impl{
public:
    [[nodiscard]] virtual std::string GetText() const = 0;
    [[nodiscard]] virtual Value GetValue() const = 0;
    [[nodiscard]] virtual std::vector<Position> GetReferencedCells() const = 0;
protected:
    Value value_;
    std::string text_;
};

class Cell::EmptyImpl : public Impl {
public:
    EmptyImpl();
    [[nodiscard]] std::string  GetText() const override;
    [[nodiscard]] Value GetValue() const override;
    [[nodiscard]] std::vector<Position> GetReferencedCells() const override;
};

class Cell::TextImpl : public Impl {
public:
    explicit TextImpl(std::string_view text);
    [[nodiscard]] std::string  GetText() const override;
    [[nodiscard]] Value GetValue() const override;
    [[nodiscard]] std::vector<Position> GetReferencedCells() const override;
};

class Cell::FormulaImpl : public Impl {
public:
    explicit FormulaImpl(std::string_view expression, const SheetInterface& sheet);
    [[nodiscard]] std::string  GetText() const override;
    [[nodiscard]] Value GetValue() const override;
    [[nodiscard]] std::vector<Position> GetReferencedCells() const override;

private:
    const SheetInterface& sheet_;
    std::unique_ptr<FormulaInterface> formula_;
};
