#include "Game.h"

Game::Game()
{
    m_level             = 0;
    m_points            = 0;
    m_currentBlockId    = 0;
    m_nextMoveTime      = 0;
    m_pausedTime        = 0;
    m_linesCompleted    = 0;
    m_activeBlock       = nullptr;
    m_nextBlock         = nullptr;
    m_lastBlockType     = TYPE_NONE;
    m_isPaused          = false;
    m_gameBlocks.clear();
}

Game::~Game()
{
    m_gameBlocks.clear();
}

Game* Game::CreateNewGame(uint32 level /*=DEFAULT_LEVEL*/)
{
    Game* newGame = new Game();
    if (!newGame)
    {
        DEBUG_LOG("Failed to create new game. Stopping...\n");
        return nullptr;
    }

    DEBUG_LOG("Game succesfully created.\n");

    newGame->m_level = level;
    newGame->m_points = 0;
    newGame->m_gameBlocks.clear();
    newGame->m_nextMoveTime = newGame->GetNextMoveTime();

    return newGame;
}

void Game::StartGame()
{
    GenerateBlock(true);
    GenerateBlock(false);
}

void Game::Update()
{
    if (int64(m_nextMoveTime - clock()) <= 0)
    {
        //DebugBlockPositions();
        m_nextMoveTime = GetNextMoveTime();
        HandleDropBlock();
    }
}

void Game::PauseGame()
{
    //m_pausedTime = clock();
}

void Game::ResumeGame()
{
    m_nextMoveTime = GetNextMoveTime();
}

Block* Game::GenerateBlock(bool active, BlockType type /*= TYPE_NONE*/)
{
    if (type == TYPE_NONE)
    {
        // Prevent generate the same block twice in a row
        do
        {
            type = BlockType(rand() % MAX_BLOCK_TYPE);
        }
        while (type == m_lastBlockType);
    }

    float pos[2][2] = { { CENTER, MAX_HEIGHT}, { NEXT_BLOCK_X, NEXT_BLOCK_Y} }; 

    Block* block = new Block(type, this, pos[!active][0], pos[!active][1]);
    if (!block)
    {
        DEBUG_LOG("Failed to create block. Stopping...\n");
        exit(EXIT_FAILURE);
    }

    m_lastBlockType = block->GetType();
    if (!active)
    {
        if (m_activeBlock)
        {
            m_activeBlock->SetPositionX(CENTER);
            m_activeBlock->SetPositionY(MAX_HEIGHT);
        }
        m_nextBlock = block;
    }
    else
        m_activeBlock = block;

    DEBUG_LOG("Block type: %d succesfully created.\n", type);
    return block;
}

void Game::DestroyActiveBlock(bool withSave /*=true*/)
{
    if (!m_activeBlock)
    {
        DEBUG_LOG("Failed at destroy active block.\n");
        return;
    }

    if (withSave)
    {
        for (SubBlock* sub : m_activeBlock->GetSubBlocks())
        {
            sub->SetPositionX(sub->GetPositionX() + m_activeBlock->GetPositionX());
            sub->SetPositionY(sub->GetPositionY() + m_activeBlock->GetPositionY());
            m_gameBlocks.push_back(sub);
            sub->DebugPosition();
        }

        m_activeBlock = m_nextBlock;
        GenerateBlock(false);
    }
    else
        GenerateBlock(true);
}

void Game::HandleDropBlock()
{
    if (!m_activeBlock)
        return;

    if (m_activeBlock->CanDropBlock())
    {
        float posY = std::max(m_activeBlock->GetPositionY() - 1.0f, 0.0f);
        m_activeBlock->SetPositionY(posY);
        CheckLineCompleted();
    }
    else
    {
        DestroyActiveBlock();
        CheckLineCompleted();
        CheckGameLost();
    }
}

void Game::RotateActiveBlock()
{
    if (!m_activeBlock)
        return;

    m_activeBlock->RotateBlock();
}

uint64 Game::GetNextMoveTime()
{
    return uint64(clock() + (DEFAULT_MILLISECONDS / 2.0) + double(DEFAULT_MILLISECONDS) * GetSpeed());
}

double Game::GetSpeed() const
{
    return -1.0f * double(std::log(double(m_level)) / std::log(20.0)) + 2.0;
}

void Game::MoveBlock(bool right)
{
    if (!m_activeBlock)
        return;

    m_activeBlock->MoveBlock(right);
}

void Game::DropBlock()
{
    if (!m_activeBlock)
        return;

    m_activeBlock->Drop();
    DestroyActiveBlock();
    CheckLineCompleted();
    CheckGameLost();
}

void Game::CheckLineCompleted()
{
    std::vector<float> linesCompleted;
    for (uint32 y = 0; y < MAX_HEIGHT; y++)
    {
        uint8 i = 0;
        for (uint32 x = 0; x < MAX_WIDTH; x++)
        {
            if (!GetSubBlockInPosition(float(x), float(y)))
                break;

            i++;
        }

        // Insert line completed if all X positions are filled with subblocks
        if (i >= MAX_WIDTH)
            linesCompleted.push_back(float(y));
    }

    if (linesCompleted.empty())
        return;

    DEBUG_LOG("Lines completed: ");
    for (float i : linesCompleted)
    {
        for (uint32 x = 0; x < MAX_WIDTH; x++)
        {
            SubBlock* sub = GetSubBlockInPosition(float(x), i);
            if (!sub)
            {
                DEBUG_LOG("Incorrect line completed.\n");
                exit(EXIT_FAILURE);
            }

            sub->Delete();
        }
        DEBUG_LOG("[%f]", i);
    }
    DEBUG_LOG("\n");

    m_linesCompleted += linesCompleted.size();
    m_level = (m_linesCompleted / LINE_PER_DIFF) + 1;
    m_points = m_linesCompleted * 100;

    std::sort(m_gameBlocks.begin(), m_gameBlocks.end());

    for (SubBlock* sub : m_gameBlocks)
    {
        //sub->DebugPosition();
        for (uint32 i = 0; i < linesCompleted.size(); i++)
            if (sub->GetPositionY() >= linesCompleted[i])
                sub->SetPositionY(sub->GetPositionY() - 1.0f);
    }
}

void Game::CheckGameLost()
{
    if (!m_activeBlock)
    {
        DEBUG_LOG("Active block not found. Stopping...");
        exit(EXIT_FAILURE);
    }

    if (GetSubBlockInPosition(CENTER, MAX_HEIGHT - 1.0f))
        EndGame();
}

void Game::EndGame()
{
    DEBUG_LOG("END");
}

SubBlock* Game::GetSubBlockInPosition(float x, float y)
{
    SubBlock* sub = nullptr;

    std::vector<SubBlock*> subBlocks = GetSubBlockList();
    for (auto itr = subBlocks.begin(); itr != subBlocks.end(); itr++)
    {
        SubBlock* temp = *(itr);
        if (temp->GetPositionX() == x && temp->GetPositionY() == y)
        {
            sub = temp;
            break;
        }
    }

    return sub;
}

void Game::ChangeBlock()
{
    DestroyActiveBlock(false);
}

void Game::DebugBlockPositions()
{
    if (m_activeBlock)
        m_activeBlock->DebugPosition();

    for (SubBlock* sub : m_gameBlocks)
    {
        sub->DebugPosition();
    }
}

void Game::DeleteSubBlock(SubBlock* subBlock)
{
    m_gameBlocks.erase(std::find(m_gameBlocks.begin(), m_gameBlocks.end(), subBlock));
}

void Game::IncreaseBlockSpeed()
{
    if (m_activeBlock && m_activeBlock->CanDropBlock())
    {
        m_activeBlock->SetPositionY(m_activeBlock->GetPositionY() - 1.0f);
        m_nextMoveTime = GetNextMoveTime();
    }
}
