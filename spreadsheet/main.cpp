//#define NDEBUG
#include "./include/tests.h"
#include "./include/sheet.h"

int main() {
#ifndef NDEBUG
    tests::TestSpreadSheer();
#endif

    auto sheet = CreateSheet();

    sheet->SetCell("A1"_pos, "=A4");
    sheet->SetCell("A4"_pos, "27");
    sheet->SetCell("C5"_pos, "=B3");
    sheet->SetCell("B3"_pos, "Ready");
    auto a1 = sheet->GetCell({0, 0});
    std::cout << "A1 value = " << a1->GetValue() << std::endl;
    std::cout << "A1 text = " << a1->GetText() << std::endl;

    sheet->PrintTexts(std::cout);

    return 0;
}