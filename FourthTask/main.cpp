#include <iostream>
#include <array>

struct Bitboard
{
    explicit Bitboard(const std::string& FEN)
    {
        auto fen_board = UNPackFEN(FEN);
        Init(fen_board);
    }

    static std::size_t GetWhiteBoard() noexcept
    {
        return white_board;
    }

    static std::size_t GetBlackBoard() noexcept
    {
        return black_board;
    }

    std::array<std::size_t,3> GetTurns() const noexcept;

private:
    static std::size_t white_board;
    static std::size_t black_board;

    std::size_t rook_pos;
    std::size_t bishop_pos;
    std::size_t queen_pos;

    std::array<std::string, 8> UNPackFEN(const std::string& FEN) const
    {
        std::array<std::string, 8> fen_board;

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

        return fen_board;
    }

    void Init(const std::array<std::string, 8>& fen_board)
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
                    if(isupper(i))
                    {
                        switch(i)
                        {
                            case 'R': rook_pos = pos; break;
                            case 'B': bishop_pos = pos; break;
                            case 'Q': queen_pos = pos; break;
                        }

                        white_board |= bit_pos;
                    }
                    else
                    {
                        black_board |= bit_pos;
                    }



                    pos += 1;
                }
            }
        }
    }


};

std::size_t Bitboard::white_board = 0;
std::size_t Bitboard::black_board = 0;

struct Figure
{
    explicit Figure(std::size_t ParamPos)
    :
    pos{ParamPos}
    {

    }

protected:

    std::size_t GetPos() const noexcept
    {
        return pos;
    }

    std::size_t GetWhiteBoard() const noexcept
    {
        auto white_board = Bitboard::GetWhiteBoard();
        auto pos = GetPos();
        white_board = white_board xor (1ul << pos);

        return white_board;
    }

    std::size_t GetBlackBoard() const noexcept
    {
        return Bitboard::GetBlackBoard();
    }

private:
    std::size_t pos;

};

struct Rook : Figure
{
    explicit Rook(std::size_t ParamPos)
    :
    Figure(ParamPos)
    {

    }

    std::size_t GetTurns() const noexcept
    {
        // 4 sides UP,DOWN,LEFT,RIGHT
        turns = 0;

        CalcUpTurns();
        CalcDownTurns();
        CalcLeftTurns();
        CalcRightTurns();

        return turns;
    }


private:
    mutable std::size_t turns;

    void CalcUpTurns() const
    {
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();
        auto pos = GetPos();

        pos += 8;
        auto bit_pos = 1ul << pos;

        while(pos <= 63 and (white_board & bit_pos) == 0)
        {
            turns |= bit_pos;

            if(black_board & bit_pos)
            {
                break;
            }
            else
            {
                pos += 8;
                bit_pos = 1ul << pos;
            }
        }
    }

    void CalcDownTurns() const
    {
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();
        int pos = GetPos();

        pos -= 8;
        auto bit_pos = 1ul << pos;

        while(pos > 0 and (white_board & bit_pos) == 0)
        {
            turns |= bit_pos;

            if(black_board & bit_pos)
            {
                break;
            }
            else
            {
                pos -= 8;
                bit_pos = 1ul << pos;
            }
        }
    }

    void CalcLeftTurns() const
    {
        auto mask = 0xFEFEFEFEFEFEFEFE;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();
        int pos = GetPos();
        auto bit_pos = 1ul << pos;

        if((mask & bit_pos) != 0)
        {
            pos -= 1;
            bit_pos = 1ul << pos;
            while(((pos % 8) != 7 && (pos != -1))  and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;
                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    pos -= 1;
                    bit_pos = 1ul << pos;
                }
            }

        }
    }

    void CalcRightTurns() const
    {
        auto mask = 0x7F7F7F7F7F7F7F7F;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();

        auto pos = GetPos();
        auto bit_pos = 1ul << pos;

        if((mask & bit_pos) != 0)
        {
            pos += 1;
            bit_pos = 1ul << pos;
            while((pos % 8) != 0 and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;
                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    pos += 1;
                    bit_pos = 1ul << pos;
                }
            }
        }
    }

};

struct Bishop : Figure
{
    Bishop(std::size_t ParamPos)
    :
    Figure(ParamPos)
    {

    }

    std::size_t GetTurns() const noexcept
    {
        turns = 0;

        CalcRighUpDiagonal();
        CalcLeftDownDiagonal();
        CalcLeftUpDiagonal();
        CalcRightDownDiagonal();

        return turns;
    }

private:
    mutable std::size_t turns;

    void CalcRighUpDiagonal() const
    {
        auto mask = 0x7F7F7F7F7F7F7F7F;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();

        auto pos = GetPos();
        auto bit_pos = 1ul << pos;

        if((mask & bit_pos) != 0)
        {
            pos += 9;
            bit_pos = 1ul << pos;
            while (pos <= 63 and (pos % 8) != 0 and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;

                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    pos += 9;
                    bit_pos = 1ul << pos;
                }
            }
        }
    }

    void CalcLeftDownDiagonal() const
    {
        auto mask = 0xFEFEFEFEFEFEFEFE;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();

        int pos = GetPos();
        auto bit_pos = 1ul << pos;
        if((mask & bit_pos) != 0)
        {
            pos -= 9;
            bit_pos = 1ul << pos;
            while (pos > 0  and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;
                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    if ((pos % 8) == 0)
                    {
                        break;
                    }
                    else
                    {
                        pos -= 9;
                        bit_pos = 1ul << pos;
                    }
                }
            }
        }
    }

    void CalcLeftUpDiagonal() const
    {
        auto mask = 0xFEFEFEFEFEFEFEFE;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();
        auto pos = GetPos();
        auto bit_pos = 1ul << pos;

        if((mask & bit_pos) != 0)
        {
            pos += 7;
            bit_pos = 1ul << pos;
            while (pos <= 63 and (pos % 8) != 7 and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;
                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    pos += 7;
                    bit_pos = 1ul << pos;
                }
            }
        }
    }

    void CalcRightDownDiagonal() const
    {
        auto mask = 0x7F7F7F7F7F7F7F7F;
        auto white_board = GetWhiteBoard();
        auto black_board = GetBlackBoard();
        int pos = GetPos();
        auto bit_pos = 1ul << pos;

        if((mask & bit_pos) != 0)
        {
            pos -= 7;
            bit_pos = 1ul << pos;
            while (pos > 0 and (white_board & bit_pos) == 0)
            {
                turns |= bit_pos;
                if(black_board & bit_pos)
                {
                    break;
                }
                else
                {
                    if ((pos % 8) == 7)
                    {
                        break;
                    }
                    else
                    {
                        pos -= 7;
                        bit_pos = 1ul << pos;
                    }
                }
            }
        }
    }
};

struct Queen : Figure
{
    Queen(std::size_t ParamPos)
    :
    Figure(ParamPos)
    {

    }

    std::size_t GetTurns() const noexcept
    {
        auto pos = GetPos();

        Rook rook{pos};
        Bishop bishop{pos};

        auto turns = rook.GetTurns() | bishop.GetTurns();

        return turns;
    }

};


std::array<std::size_t,3> Bitboard::GetTurns() const noexcept
{
    std::array<std::size_t,3> r;

    Rook rook{rook_pos};
    r[0] = rook.GetTurns();

    Bishop bishop{bishop_pos};
    r[1] = bishop.GetTurns();

    Queen queen{queen_pos};
    r[2] = queen.GetTurns();

    return r;
}


int main()
{

    Bitboard bitboard{"B6Q/8/4q3/8/2n5/5k2/8/K6R"};

    const auto turns = bitboard.GetTurns();

    std::cout << turns[0] << '\n';
    std::cout << turns[1] << '\n';
    std::cout << turns[2] << '\n';

    return 0;
}