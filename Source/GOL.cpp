#include "GOL.h"
#include "Random.h"

const color_t white{ 255, 255, 255, 255 };
const color_t black{ 0, 0, 0, 255 };

bool GOL::Initialize() {
    m_renderer.Initialize();
    m_renderer.CreateWindow("Game D Life", 800, 600);

    m_input.Initialize();
    m_input.Update();

    m_framebuffer = std::make_unique<Framebuffer>(m_renderer, m_renderer.m_width / 2, m_renderer.m_height / 2);

    m_cellsA = std::make_unique<Cells<uint8_t>>(m_framebuffer->m_width, m_framebuffer->m_height);
    m_cellsB = std::make_unique<Cells<uint8_t>>(m_framebuffer->m_width, m_framebuffer->m_height);

    return true;
}

void GOL::Update()
{
    Scene::Update();
    m_frame++;

    Cells<uint8_t>* currentCells = (m_frame % 2) ? m_cellsA.get() : m_cellsB.get();
    Cells<uint8_t>* nextCells = (m_frame % 2) ? m_cellsB.get() : m_cellsA.get();

    // Fills nextCellswith '0.'
    std::fill(nextCells->m_data.begin(), nextCells->m_data.end(), 0);

    if (m_input.GetKeyDown(SDL_SCANCODE_SPACE))
    {
        for (int i = 0; i < nextCells->m_data.size(); i++)
        {
            nextCells->m_data[i] = (random(40) == 0) ? 1 : 0;
        }
    }

    for (int y = 1; y < nextCells->m_height - 1; y++)
    {
        for (int x = 1; x < nextCells->m_width - 1; x++)
        {
            int count = 0;

            count += currentCells->Read(x-1, y-1);
            count += currentCells->Read(x+0, y-1);
            count += currentCells->Read(x+1, y-1);
            count += currentCells->Read(x-1, y);
            count += currentCells->Read(x+1, y);
            count += currentCells->Read(x-1, y+1);
            count += currentCells->Read(x+0, y+1);
            count += currentCells->Read(x+1, y+1);

            // Do the game of life rules
            uint8_t currentState = currentCells->Read(x, y);
            if (currentState)
            {
                // We are alive, stay alive if we have 2 or 3 neighbors, else DIE!
                uint8_t nextState = (count == 2 || count == 3) ? 1 : 0;
                nextCells->Write(x, y, nextState);
            }
            else
            {
                // We are dead, make alive if we have 3 neighbors.
                if (count == 3) nextCells->Write(x, y, 1);
            }
        }
    }

    // Write cells to the frame buffer
    m_framebuffer->Clear(black);
    for (int i = 0; i < nextCells->m_data.size(); i++)
    {
        m_framebuffer->m_buffer[i] = (nextCells->m_data[i]) ? white : black;
    }
}

void GOL::Draw()
{
    // update framebuffer
    m_framebuffer->Update();

    // show screen
    m_renderer = *m_framebuffer;
    SDL_RenderPresent(m_renderer.m_renderer);
}