#include <iostream>
#include <array>
#include <ctype.h>

struct FENToFigureLoc
{
    FENToFigureLoc()
    {
        figure_locs.fill(0);
    }

    std::array<std::size_t,12> GetFigureLoc(const std::string& FEN) const
    {
        UNPackFEN(FEN);
        AnalyzeFEN();

        return figure_locs;
    }

private:
    mutable std::array<std::string, 8> fen_board;
    mutable std::array<std::size_t, 12> figure_locs;

    void UNPackFEN(const std::string& FEN) const
    {
        std::size_t start{};
        std::size_t end{};
        for(auto it = std::rbegin(fen_board); it != std::rend(fen_board); ++it)
        {
            end = FEN.find_first_of('/', start);
            if(end != FEN.npos)
            {
                *it = FEN.substr(start, end - start);
            }
            else
            {
                *it = FEN.substr(start, FEN.size());
            }

            start = end + 1;
        }
    }

    void AnalyzeFEN() const
    {
        for(std::size_t h{}, pos{}; h < fen_board.size(); ++h)
        {
            auto h_string = fen_board[h];
            for(const auto i : h_string)
            {
                if(isdigit(i))
                {
                    pos += std::strtol(&i, nullptr, 10);
                }
                else
                {
                    auto bit_pos = 1ul << pos;
                    SetValue(i, bit_pos);
                    pos += 1;
                }
            }
        }
    }

    void SetValue(char Figure, std::size_t pos) const
    {
        std::size_t offset{};
        if(!isupper(Figure))
        {
            offset = 6;
        }
        else
        {
            Figure = static_cast<char>(tolower(Figure));
        }

        std::size_t figure_index{};
        switch(Figure)
        {
            case 'p' : figure_index = 0; break;
            case 'n' : figure_index = 1; break;
            case 'b' : figure_index = 2; break;
            case 'r' : figure_index = 3; break;
            case 'q' : figure_index = 4; break;
            case 'k' : figure_index = 5; break;
            default:
                throw std::logic_error("Unknown figure");
        }

        auto final_index = offset + figure_index;
        figure_locs[final_index] |= pos;
    }
};

int main()
{
    std::string FEN = "8/6k1/2PPP3/2PKP3/2PPPn2/8/8/8";

    FENToFigureLoc fenToFigureLoc;
    auto f = fenToFigureLoc.GetFigureLoc(FEN);

    for(const auto i : f)
    {
        std::cout << i << '\n';
    }

    return 0;
}