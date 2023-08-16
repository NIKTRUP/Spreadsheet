#pragma once

#include "common.h"
#include "formula.h"
#include <optional>

class Cell : public CellInterface {
public:

    Cell(std::string text, SheetInterface& sheet);
    explicit Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    static std::unique_ptr<Impl> InitCell(std::string& text, const SheetInterface& sheet);

private:
    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_; // Ссылка на таблицу

    mutable std::optional<Value> cached_value_; // Кэш
    std::vector<Position> referenced_cells_; // Ячейки, от которых зависит текущая ячейка
    std::vector<Position> dependent_cells_; // Ячейки, которые зависят от текущей ячейки
};

class Cell::Impl{
public:
    virtual std::string GetText() const = 0;
    virtual Value GetValue() const = 0;
protected:
    Value value_;
    std::string text_;
};

class Cell::EmptyImpl : public Impl {
public:
    EmptyImpl();

    std::string  GetText() const override;

    Value GetValue() const override;
};

class Cell::TextImpl : public Impl {
public:
    explicit TextImpl(std::string_view text);

    std::string  GetText() const override;

    Value GetValue() const override;
};

class Cell::FormulaImpl : public Impl {
public:
    explicit FormulaImpl(std::string_view expression, const SheetInterface& sheet);

    std::string  GetText() const override;

    Value GetValue() const override;

private:
    std::unique_ptr<FormulaInterface> formula_;
    const SheetInterface& sheet_;
};
