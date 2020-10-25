#include <iostream>

std::size_t GetKingTurnsValue(std::size_t pos)
{
    std::size_t k = 1;
    k <<= pos;

    std::size_t kL = k & 0xFEFEFEFEFEFEFEFE;
    std::size_t kR = k & 0x7F7F7F7F7F7F7F7F;

    std::size_t r =
            kL << 7ull | k << 8ull | kR << 9ull |
            kL >> 1ull |             kR << 1ull |
            kL >> 9ull | k >> 8ull | kR >> 7ull ;

    return r;
}

std::size_t GetTurns(std::size_t bitboard_val)
{
    std::size_t r{};
    while(bitboard_val)
    {
        r += 1;
        bitboard_val &= bitboard_val - 1;
    }

    return r;
}

std::size_t GetHorseTurnsValue(std::size_t pos)
{
    std::size_t h = 1;
    h <<= pos;

    std::size_t hr_min = h & 0x7F7F7F7F7F7F7F7F;
    std::size_t hr_max = h & 0x3F3F3F3F3F3F3F3F;
    std::size_t hl_min = h & 0xFEFEFEFEFEFEFEFE;
    std::size_t hl_max = h & 0xFCFCFCFCFCFCFCFC;

    std::size_t r =
            (hr_min << 17 | hr_min >> 15)  |
            (hr_max << 10 | hr_max >> 6 )  |
            (hl_min << 15 | hl_min >> 17)  |
            (hl_max >> 10 | hl_max << 6) ;

    return r;
}

void print_out_info(std::size_t Value, std::size_t Turns)
{
    std::cout << "Value: " << Value << " Turns: " << Turns << '\n';
}

int main()
{
    const auto KingValue = GetKingTurnsValue(56);
    const auto KingTurns = GetTurns(KingValue);

    std::cout << "King ";
    print_out_info(KingValue, KingTurns);

    const auto HorseValue = GetHorseTurnsValue(56);
    const auto HorseTurns = GetTurns(KingValue);

    std::cout << "Horse ";
    print_out_info(HorseValue,HorseTurns);

    return 0;
}