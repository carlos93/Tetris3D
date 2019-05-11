#ifndef GAME_H
#define GAME_H

#include "Common.h"
#include "Block.h"

constexpr int32 DEFAULT_LEVEL = 1;
constexpr uint64 DEFAULT_MILLISECONDS = 500;

class Game
{
public:
    Game();
    ~Game();

    static Game* CreateNewGame(uint32 level = DEFAULT_LEVEL);

    void StartGame();
    void Update();
    void EndGame();
    void PauseGame();
    void ResumeGame();

    Block* GenerateBlock(bool active, BlockType type = TYPE_NONE);

    void DestroyActiveBlock(bool withSave = true);

    uint64 GetNextMoveTime();

    void RotateActiveBlock();

    void MoveBlock(bool right);
    void DropBlock();
    void HandleDropBlock();
    void ChangeBlock();
    void DeleteSubBlock(SubBlock* subBlock);
    void IncreaseBlockSpeed();

    void DebugBlockPositions();

    void CheckLineCompleted();
    void CheckGameLost();

    SubBlock* GetSubBlockInPosition(float x, float y);
    
    Block::SubBlockVector GetSubBlockList() const { return m_gameBlocks; }

    uint32 GetPoints() const { return m_points; }
    void SetPoints(uint32 _points) { m_points = _points; }

    uint32 GetLevel() const { return m_level; }
    void SetLevel(uint32 _level) { m_level = std::max<int32>(1, _level); }

    uint32 GetCurrentBlockID() { return m_currentBlockId; }
    void SetCurrentBlockID(uint32 _currentBlockId) { m_currentBlockId = _currentBlockId; }
    void IncreaseCurrentBlockID() { m_currentBlockId++; }

    Block* GetActiveBlock() { return m_activeBlock; }
    const Block* GetActiveBlock() const { return m_activeBlock; }

    void SetActiveBlock(Block* block) { m_activeBlock = block; }
    
    Block* GetNextBlock() { return m_nextBlock; }
    const Block* GetNextBlock() const { return m_nextBlock; }

    void SetNextBlock(Block* block) { m_nextBlock = block; }

    double GetSpeed() const;

private:
    Block::SubBlockVector m_gameBlocks;

    Block* m_activeBlock;
    Block* m_nextBlock;

    uint32 m_points;
    uint32 m_level;
    uint32 m_linesCompleted;
    uint32 m_currentBlockId;

    uint64 m_nextMoveTime;
    uint64 m_pausedTime;

    BlockType m_lastBlockType;

    bool m_isPaused;
};

#endif
