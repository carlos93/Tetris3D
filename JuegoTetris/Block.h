#ifndef BLOCK_H
#define BLOCK_H

#include "Common.h"

enum Color : int8
{
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_PINK,
    COLOR_ORANGE,
    COLOR_GRAY
};
    
enum BlockType : int8
{
    TYPE_NONE = 0,
    TYPE_CUBE,
    TYPE_PRISM,
    TYPE_L,
    TYPE_L_INV,
    TYPE_T,
    TYPE_Z,
    TYPE_Z_INV,
    MAX_BLOCK_TYPE
};

struct Position
{
    Position() { x = 0.0f; y = 0.0f; z = 0.0f; }
    Position(float _x, float _y) { x = _x; y = _y; z = 0.0f; }
    Position(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

    float x, y, z;

    inline bool operator==(const Position &other) { return x == other.x && y == other.y && z == other.z; }
};

class Game;

class SubBlock
{
public:

    typedef std::vector<SubBlock*> SubBlockVector;

    SubBlock();
    SubBlock(Game* game);
    ~SubBlock();

    void Delete();

    uint32 GetID() const { return ID; }

    Position GetPosition() const { return m_position; }
    void SetPosition(Position _position) { m_position = _position; }

    float GetPositionX() const { return m_position.x; }
    void SetPositionX(float x) { m_position.x = x; }

    float GetPositionY() const { return m_position.y; }
    void SetPositionY(float y) { m_position.y = y;}
    
    float GetPositionZ() const { return m_position.z; }
    void SetPositionZ(float z) { m_position.z = z; }

    Game* GetGame() const { return m_game; }
    void SetGame(Game* game) { m_game = game; }

    Color GetColor() const { return m_color; }
    void SetColor(Color color) { m_color = color; }

    bool CanDropSubBlock();

    void DebugPosition();

    inline bool operator<(SubBlock const* other)
    {
        if (this->m_position.y < other->m_position.y)
            return true;
        else if (this->m_position.y > other->m_position.y)
            return false;
        else if (this->m_position.x < other->m_position.x)
            return true;
        else
            return false;
    }

protected:
    Color m_color;
    uint32 ID;

    Position m_position;
    Game* m_game;
};

class Block : public SubBlock
{
public:
    Block(BlockType type, Game* game, float x, float y);
    ~Block();

    BlockType GetType() const { return m_type; }
    void SetType(BlockType type) { m_type = type; }

    void GenerateSubBlocks();
    void RotateBlock();
    void Drop();
    void MoveBlock(bool right);

    bool CanDropBlock();
    bool CanRotateBlock();
    bool CanMoveBlock(bool right);

    static Position* GetPositionsOfType(BlockType type);

    SubBlockVector GetSubBlocks() const { return m_subBlocks; }

    static Color GetColorByType(BlockType type);

    void DebugPosition();

private:
    BlockType m_type;

    SubBlockVector m_subBlocks;
};

#endif
