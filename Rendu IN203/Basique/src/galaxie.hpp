#ifndef _GALAXIE_HPP_
#define _GALAXIE_HPP_
#include <vector>
#include <SDL.h>

enum etat_planete
{
    habitable = 0, habitee = 1, inhabitable = -1
};

class galaxie
{
public:
    galaxie(int width, int height);
    galaxie(int width, int height, double chance_habitee);
    virtual ~galaxie() {}

    virtual void rend_planete_habitee(int x, int y);
    virtual void rend_planete_inhabitable(int x, int y);
    virtual void rend_planete_inhabitee(int x, int y);

    int width()  const { return m_width;  }
    int height() const { return m_height; }
    const char* data() const { return m_planetes.data(); }
    char* data() { return m_planetes.data(); }

    void swap(galaxie& g);

private:
    int m_width, m_height;
    std::vector<char> m_planetes;
};

class galaxie_renderer
{
public:
    galaxie_renderer(SDL_Window* win);
    ~galaxie_renderer();

    virtual void rend_planete_habitee(int x, int y);
    virtual void rend_planete_inhabitable(int x, int y);
    virtual void rend_planete_inhabitee(int x, int y);

    void render(const galaxie& g);
private:
    SDL_Renderer*  m_renderer;
    SDL_Texture  * m_texture;
};

#endif